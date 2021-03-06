
/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */

#include "gui_driver.h"
#include "screens_common.h"
#include "watch_private.h"
#include "notifications_driver.h"
#include "notifications_events.h"
#include "rtc_driver.h"


/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */

/*! @brief Converts milliseconds to ticks*/
#define MSEC_TO_TICK(msec)  (((uint32_t)(msec)+500uL/(uint32_t)configTICK_RATE_HZ) \
                             *(uint32_t)configTICK_RATE_HZ/1000uL)


/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
********************************************************************************** */

static void watch_TimeUpdateTask(task_param_t param);
static void watch_GetPacketsTask(task_param_t param);
static void watch_GuiUpdateTask(task_param_t param);

static void watch_UpdateBatteryLevel(uint8_t level);

static uint32_t leap (uint32_t year);
static uint32_t zeller (uint32_t year, uint32_t month, uint32_t day);
static uint32_t dow (uint32_t year, uint32_t month, uint32_t day);
static const char * watch_GetDayOfweek(rtc_datetime_t *time);

/*! *********************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
********************************************************************************** */

task_handler_t watch_timeUpdate_handler;
task_handler_t watch_getPackets_handler;
task_handler_t watch_guiUpdate_handler;
task_handler_t watch_notifListen_handler;

/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */

static rtc_datetime_t watch_time;

static hostInterface_packet_t watch_dataPacket;
//static bool watch_notifUnreadFlag;

static bool
	isBatteryCharging = false;

static mE_t
  currTemp = 0xFF;

static linkState_t watch_linkState = linkState_disconnected;

static event_t watch_event;

static event_flags_t linkStateUpdate_eventFlag  = 1;
static event_flags_t timeUpdate_eventFlag      = 1 << 1;
static event_flags_t dateUpdate_eventFlag      = 1 << 2;
static event_flags_t tempUpdate_eventFlag      = 1 << 3;
static event_flags_t batteryUpdate_eventFlag   = 1 << 4;
static event_flags_t chargDetect_eventFlag     = 1 << 5;
static event_flags_t notifDetect_eventFlag     = 1 << 6;

static hostInterface_packet_t watch_dataPacket =
{
	.start1 = gHostInterface_startByte1,
	.start2 = gHostInterface_startByte2,
	.length = 0,
	.data[0] = gHostInterface_trailerByte
};

static uint8_t batteryLevel;

//static const char dayOfWeekStr[7][10] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
static const char dayOfWeekStr[7][4] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};

static const char monthsOfYearStr[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Dec"};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void watch_CreateLinkStateUpdateEvent(void)
{
	OSA_EventCreate( &watch_event, kEventAutoClear );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

linkState_t watch_CurrentLinkStateGet(void)
{
	return watch_linkState;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void watch_SendGetLinkStateReq(void)
{
	static hostInterface_packet_t dataPacket =
	{
		.start1 = gHostInterface_startByte1,
		.start2 = gHostInterface_startByte2,
		.length = 0,
		.data[0] = gHostInterface_trailerByte
	};

	dataPacket.type = packetType_linkStateGet;
	while(HostInterface_TxQueueMsgPut((hostInterface_packet_t *)&dataPacket, true) != kStatus_OSA_Success);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * [buttonsGroup_DestroyTasks description]
 * @param param [description]
 */

osa_status_t watch_WaitForLinkStateUpdate(uint32_t timeout)
{
	osa_status_t status;
	event_flags_t setFlags;

	status = OSA_EventWait(&watch_event, linkStateUpdate_eventFlag, false, timeout, &setFlags);

	if ( kStatus_OSA_Success != status )
	{
		return kStatus_OSA_Error;
	}

	return (osa_status_t)status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * [buttonsGroup_DestroyTasks description]
 * @param param [description]
 */

osa_status_t watch_LinkStateUpdate(linkState_t new_linkState)
{
	OSA_EventSet( &watch_event, (event_flags_t)linkStateUpdate_eventFlag);
	watch_linkState = new_linkState;

	switch ( watch_linkState )
	{
		case linkState_disconnected:
		{
  			sensor_SaveTargetsForKW40();

  			Notification_SetUnreadCounter( NOTIF_TYPE_CALL, 0 );
  			Notification_SetUnreadCounter( NOTIF_TYPE_MAIL, 0 );
  			Notification_SetUnreadCounter( NOTIF_TYPE_SMS, 0 );

  			watch_SetNotification();

			if ( true == gui_sensorTag_IsActive() )
			{
			  power_SetSleepMode( POWER_SLEEP_MODE_TOTAL );
			}
		 	break;
		}

		case linkState_connected:
		{
  			sensor_RestoreTargetsForKW40();

			if ( true == gui_sensorTag_IsActive() )
			{
				power_SetSleepMode( POWER_SLEEP_MODE_SENSOR_TAG );
				GuiDriver_NotifyKW40( GUI_CURRENT_APP_SENSOR_TAG );
			}

  			break;
		}
	}

	return kStatus_OSA_Success;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * [watch_Init description]
 * @param param [description]
 */

void watch_Init( void *param )
{
  const char *dayOfWeekPtr;
  const char *monthsOfYearPtr;
  // Add image which indicate battery level to screen buffer.
  GuiDriver_ImageAddToScr(&watch_imgBattery);

  // Create time label, initialize to current time, and add to screen.
  GuiDriver_LabelCreate(&watch_labelTime);
  RTC_GetCurrentTime(&watch_time);
  snprintf( (char*)watch_labelTime.caption, 6, "%02d:%02d", watch_time.hour, watch_time.minute);
  GuiDriver_LabelAddToScr(&watch_labelTime);

  // Create date label, initialize to current date, and add to screen.
  dayOfWeekPtr = watch_GetDayOfweek(&watch_time);
  monthsOfYearPtr = monthsOfYearStr[watch_time.month - 1];
  GuiDriver_LabelCreate(&watch_labelDate);
  snprintf( (char*)watch_labelDate.caption, 12, "%s, %02d %s", dayOfWeekPtr, watch_time.day, monthsOfYearPtr);
  GuiDriver_LabelAddToScr(&watch_labelDate);

  // Create Enter label, and add to screen.
  GuiDriver_LabelCreate(&watch_labelEnter);
  GuiDriver_LabelSetCaption(&watch_labelEnter, (uint8_t *) "Menu");
  GuiDriver_LabelAddToScr(&watch_labelEnter);

  // Init label containing current temperature.
  if  (
            ( GUI_STATUS_SUCCESS == GuiDriver_LabelCreate(&watch_labelTemp) )
        &&  ( 0xFF != currTemp )
      )
  {
    snprintf( (char*)watch_labelTemp.caption, 4, "%d", currTemp);
    GuiDriver_LabelAddToScr(&watch_labelTemp);
  }

  // Check for notifications.
  if( 0 != Notification_GetUnreadCounter( NOTIF_TYPE_CALL ) )
  {
      watch_imgCall.img = watch_phone_call_bmp;
  }
  else
  {
	  watch_imgCall.img = watch_phone_call_gray_bmp;
  }

  if( 0 != Notification_GetUnreadCounter( NOTIF_TYPE_MAIL) )
  {
	  watch_imgMail.img = watch_mail_bmp;
  }
  else
  {
	  watch_imgMail.img = watch_mail_gray_bmp;
  }

  if( 0 != Notification_GetUnreadCounter( NOTIF_TYPE_SMS ) )
  {
	  watch_imgSms.img = watch_sms_bmp;
  }
  else
  {
	  watch_imgSms.img = watch_sms_gray_bmp;
  }

  GuiDriver_ImageAddToScr( &watch_imgCall );
  GuiDriver_ImageAddToScr( &watch_imgMail);
  GuiDriver_ImageAddToScr( &watch_imgSms);

  // Add temp unit to screen.
  GuiDriver_ImageAddToScr(&watch_imgTempUnit);

  if(watch_linkState == linkState_connected)
  {
	  watch_imgBluetoothLogo.img = watch_bluetooth_logo_blue_bmp;
  }
  else
  {
	  watch_imgBluetoothLogo.img = watch_bluetooth_logo_white_bmp;
  }

  // Add bluetooth icon image to screen.
  GuiDriver_ImageAddToScr(&watch_imgBluetoothLogo);

  // Register for packets we want to receive.

	if ( false == gui_sensorTag_IsActive() )
	{
		GuiDriver_RegisterMinPollDelay( 100 );
		// sensor_SetPushTargets( PACKET_TEMP, sensor_GetPushTargets( PACKET_TEMP ) | PACKET_PUSH_KW40, false );
		GuiDriver_RegisterForSensors( PACKET_TEMP, 30000, true );
		// sensor_SetPushTargets( PACKET_BAT, sensor_GetPushTargets( PACKET_BAT ) | PACKET_PUSH_KW40, false );
		GuiDriver_RegisterForSensors( PACKET_BAT, 31000, false );
	}

	else
	{
		GuiDriver_RegisterForSensors( PACKET_BAT,  -1, false );
		GuiDriver_RegisterForSensors( PACKET_TEMP, -1, true );
	}
//	GuiDriver_NotifyKW40( GUI_CURRENT_APP_WATCH );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * [watch_Callback description]
 * @param param [description]
 */

void watch_CreateTasks( void *param )
{
  osa_status_t status;

  status = OSA_TaskCreate (
                            watch_TimeUpdateTask,
                            (uint8_t *) "Time Update",
                            WATCH_STACK_SIZE,
                            NULL,
                            WATCH_PRIO + 1,
                            (task_param_t)0,
                            false,
                            &watch_timeUpdate_handler
                          );

  if ( kStatus_OSA_Success != status )
  {
    catch( CATCH_INIT );
  }

  // initialize the watch task
  status = OSA_TaskCreate (
                            watch_GetPacketsTask,
                            (uint8_t*)"Get Packets",
                            WATCH_STACK_SIZE,
                            NULL,
                            WATCH_PRIO + 1,
                            (task_param_t)0,
                            false,
                            &watch_getPackets_handler
                          );

  if ( kStatus_OSA_Success != status )
  {
    catch( CATCH_INIT );
  }

  // initialize the watch task
    status = OSA_TaskCreate (
                              watch_GuiUpdateTask,
                              (uint8_t*)"watch_GuiUpdateTask",
                              WATCH_STACK_SIZE,
                              NULL,
                              WATCH_PRIO,
                              (task_param_t)0,
                              false,
                              &watch_guiUpdate_handler
                            );

    if ( kStatus_OSA_Success != status )
    {
      catch( CATCH_INIT );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * [watch_Callback description]
 * @param param [description]
 */

void watch_DestroyTasks( void *param )
{
  osa_status_t status = kStatus_OSA_Success;

  if ( kStatus_OSA_Success != status )
  {
    catch( CATCH_INIT );
  }

  // unregister from the sensors
  // if ( false == gui_sensorTag_IsActive() )
  // {
	 //  sensor_SetPushTargets( PACKET_BAT, sensor_GetPushTargets( PACKET_BAT ) & ~PACKET_PUSH_KW40, false );
	 //  sensor_SetPushTargets( PACKET_TEMP, sensor_GetPushTargets( PACKET_TEMP ) & ~PACKET_PUSH_KW40, false );
  // }
  GuiDriver_UnregisterFromSensors( PACKET_BAT, false );
  GuiDriver_UnregisterFromSensors( PACKET_TEMP, true );

  OLED_DestroyDynamicArea();

  GuiDriver_LabelDestroy(&watch_labelTime);
  GuiDriver_LabelDestroy(&watch_labelDate);
  GuiDriver_LabelDestroy(&watch_labelTemp);

  status = OSA_TaskDestroy(watch_timeUpdate_handler);
  status |= OSA_TaskDestroy(watch_getPackets_handler);
  status |= OSA_TaskDestroy(watch_guiUpdate_handler);

//  status |= OSA_TaskDestroy(watch_notifListen_handler);

  if ( kStatus_OSA_Success != status )
  {
    catch( CATCH_DEINIT );
  }

  // OSA_TimeDelay( 100 );
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/**
 * [watch_App description]
 * @param param [description]
 */

static void watch_TimeUpdateTask(task_param_t param)
{
  bool dateUpdateFlag, timeUpdateFlag;

  while (1)
  {
    RTC_WatchUpdateEventWait (&watch_time, &dateUpdateFlag, &timeUpdateFlag, OSA_WAIT_FOREVER);

    if(timeUpdateFlag)
    {
      snprintf( (char*)watch_labelTime.caption, 6, "%02d:%02d", watch_time.hour, watch_time.minute );
      OSA_EventSet( &watch_event, (event_flags_t)timeUpdate_eventFlag);
    }

    if(dateUpdateFlag)
    {
      const char *dayOfWeekPtr;
      const char *monthsOfYearPtr;

      dayOfWeekPtr    = watch_GetDayOfweek(&watch_time);
      monthsOfYearPtr = monthsOfYearStr[watch_time.month - 1];
      snprintf( (char*)watch_labelDate.caption, 12, "%s, %02d %s", dayOfWeekPtr, watch_time.day, monthsOfYearPtr);
      OSA_EventSet( &watch_event, (event_flags_t)dateUpdate_eventFlag);
    }
  }
}


void watch_SetBatteryChargeEvent()
{
	OSA_EventSet( &watch_event, (event_flags_t)chargDetect_eventFlag);
}


void watch_SetNotification()
{
	OSA_EventSet( &watch_event, (event_flags_t)notifDetect_eventFlag );
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/**
 * [watch_App description]
 * @param param [description]
 */

static void watch_GetPacketsTask(task_param_t param)
{
  mE_t sensorValue;

  while(1)
  {
    GuiDriver_QueueMsgGet(&watch_dataPacket , OSA_WAIT_FOREVER);

    // Extract data
    sensorValue = (mE_t)( watch_dataPacket.data[0] | (mE_t)watch_dataPacket.data[1] << 8 );

    switch (watch_dataPacket.type)
    {
      // Battery Service
      case packetType_batteryLevel:
      {
	    if ( false == isBatteryCharging )
	    {
	    	batteryLevel = (uint8_t)sensorValue;
	    	OSA_EventSet( &watch_event, (event_flags_t)batteryUpdate_eventFlag);
	    }
        break;
      }

      case packetType_temperature:
      {
        currTemp = sensorValue / 100;
        snprintf( (char*)watch_labelTemp.caption, 4, "%d", currTemp);
        // TODO: Degree ascii code is 176. Check if we want to increase font to 255 chars, so we can display such char.
        OSA_EventSet( &watch_event, (event_flags_t)tempUpdate_eventFlag);
        break;
      }

      default: {}
    }
  }
}


static void watch_GuiUpdateTask(task_param_t param)
{
	osa_status_t status;
	event_flags_t setFlags;

	// Read active button group
	//watch_SendGetLinkStateReq();

	// check once in the beginning
	watch_CheckBatteryStatus();

	while(1)
	{
		status = OSA_EventWait(
								&watch_event,
								( linkStateUpdate_eventFlag | timeUpdate_eventFlag | dateUpdate_eventFlag |
							    tempUpdate_eventFlag | batteryUpdate_eventFlag | chargDetect_eventFlag | notifDetect_eventFlag ),
							    false,
								OSA_WAIT_FOREVER,
								&setFlags
							  );

		if ( kStatus_OSA_Success != status )
		{
			continue;
		}

		if(setFlags & timeUpdate_eventFlag)
		{
			GuiDriver_LabelDraw(&watch_labelTime);
		}

		if(setFlags & dateUpdate_eventFlag)
		{
			GuiDriver_LabelDraw(&watch_labelDate);
		}

		if(setFlags & batteryUpdate_eventFlag)
		{
			watch_UpdateBatteryLevel(batteryLevel);
		}

		if(setFlags & chargDetect_eventFlag)
		{
			if ( true == watch_IsBatteryCharging() )
			{
				watch_imgBattery.img = (const uint8_t*)battery_charge_bmp;
				GuiDriver_ImageDraw( &watch_imgBattery );
			}
			else
			{
				sensor_ResetPacketcounter( PACKET_BAT );
			}
		}

		if(setFlags & tempUpdate_eventFlag)
		{
			GuiDriver_LabelDraw(&watch_labelTemp);
		}

		if(setFlags & linkStateUpdate_eventFlag)
		{
			if(watch_linkState == linkState_connected)
			{
				watch_imgBluetoothLogo.img = watch_bluetooth_logo_blue_bmp;
			}
			else
			{
				watch_imgBluetoothLogo.img = watch_bluetooth_logo_white_bmp;
			}
			GuiDriver_ImageDraw(&watch_imgBluetoothLogo);
		}

		if ( setFlags & notifDetect_eventFlag )
		{
			  // Check for notifications.
			  if( 0 != Notification_GetUnreadCounter( NOTIF_TYPE_CALL ) )
			  {
			      watch_imgCall.img = watch_phone_call_bmp;
			  }
			  else
			  {
				  watch_imgCall.img = watch_phone_call_gray_bmp;
			  }

			  if( 0 != Notification_GetUnreadCounter( NOTIF_TYPE_MAIL) )
			  {
				  watch_imgMail.img = watch_mail_bmp;
			  }
			  else
			  {
				  watch_imgMail.img = watch_mail_gray_bmp;
			  }

			  if( 0 != Notification_GetUnreadCounter( NOTIF_TYPE_SMS ) )
			  {
				  watch_imgSms.img = watch_sms_bmp;
			  }
			  else
			  {
				  watch_imgSms.img = watch_sms_gray_bmp;
			  }

			GuiDriver_ImageDraw( &watch_imgCall );
			GuiDriver_ImageDraw( &watch_imgMail);
		    GuiDriver_ImageDraw( &watch_imgSms );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * [watch_UpdateBatteryLevel description]
 * @param level [description]
 */

static void watch_UpdateBatteryLevel(uint8_t level)
{
  const uint8_t *img;

  if ( 100 == level  )
  {
	img = battery_4_bmp;
  }
  else if ( level >= 50 )
  {
    img = battery_3_bmp;
  }
  else if ( level >= 30 )
  {
    img = battery_2_bmp;
  }
  else if (level >= 10 )
  {
    img = battery_1_bmp;
  }
  else
  {
	img = battery_0_bmp;
  }

  if( watch_imgBattery.img != img )
  {
    watch_imgBattery.img = img;
    GuiDriver_ImageDraw(&watch_imgBattery);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * check charging status on the interrupt
 */
void watch_CheckBatteryStatus()
{

    uint8_t
       chargePinVal = ( GPIOC_PDIR & ( 1 << 12 ) ) >> 12;

   if ( 0 == chargePinVal )
   {
        // indicate that the battery is charging
        isBatteryCharging = true;
        // power_DisablePowerSave();
   }

   else
   {
        // no charging, display normal battery levels
        isBatteryCharging = false;
//        power_EnablePowerSave();
   }

   RTC_UpdateAlarm();
   watch_SetBatteryChargeEvent();
}

/**
 * return battery charge flag
 * @return
 */
bool watch_IsBatteryCharging()
{
	return isBatteryCharging;
}

/* Returns the number of days to the start of the specified year, taking leap
 * years into account, but not the shift from the Julian calendar to the
 * Gregorian calendar. Instead, it is as though the Gregorian calendar is
 * extrapolated back in time to a hypothetical "year zero".
 */
static uint32_t leap (uint32_t year)
{
  return year*365 + (year/4) - (year/100) + (year/400);
}

/* Returns a number representing the number of days since March 1 in the
 * hypothetical year 0, not counting the change from the Julian calendar
 * to the Gregorian calendar that occured in the 16th century. This
 * algorithm is loosely based on a function known as "Zeller's Congruence".
 * This number MOD 7 gives the day of week, where 0 = Monday and 6 = Sunday.
 */
static uint32_t zeller (uint32_t year, uint32_t month, uint32_t day)
{
  year += ((month+9)/12) - 1;
  month = (month+9) % 12;
  return leap (year) + month*30 + ((6*month+5)/10) + day + 1;
}

/* Returns the day of week (1=Monday, 7=Sunday) for a given date.
 */
static uint32_t dow (uint32_t year, uint32_t month, uint32_t day)
{
  return (zeller (year, month, day) % 7) + 1;
}


static const char * watch_GetDayOfweek(rtc_datetime_t *time)
{
	uint32_t dayOfWeek;

	dayOfWeek = dow(time->year, time->month, time->day);
	return dayOfWeekStr[dayOfWeek - 1];
}


