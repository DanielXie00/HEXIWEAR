/**
 * I2C_hal.c
 *
 * Version : 1.0.1
 *
 * Revision Log :
 * 0.0.1 (Sep/2015) Module created                          Richard Lowe
 * 0.0.2 (Sep/2016) Documentation added                     Milos Vidojevic
 * 1.0.0 (Dec/2016) Major Updates                           Milos Vidojevic
 * 		- Static I2C address member removed from the module
 *		- Transfer added for devices with 16bit or greater register size 
 *		- Pin definitions removed for HAL layer. 
 *		- Delay ms function added
 * 1.0.1 (Jan/2017) dsPIC Ponters added                     Milos Vidojevic
 *
 * To Do List :
 * 		- GPIO HAL needed
 * 		- Delay us
 * 
 ******************************************************************************/

#include "air_quality_2_hal.h"

/* 		Preprocessors
 ******************************************************************************/

#define READ_BIT                1
#define WRITE_BIT               0

/*		Variables
 ******************************************************************************/

#if defined( __MIKROC_PRO_FOR_PIC32__ ) || \
    defined( __MIKROC_PRO_FOR_DSPIC__ )
static uint8_t NACK_BIT   = 0x01;
static uint8_t ACK_BIT    = 0x00;
#elif defined( __MIKROC_PRO_FOR_PIC__ ) || \
      defined( __MIKROC_PRO_FOR_AVR__ )
static uint8_t NACK_BIT   = 0x00;
static uint8_t ACK_BIT    = 0x01;
#endif

#if defined( __MIKROC_PRO_FOR_ARM__ )
    #if defined( __STM32__ ) || defined( __KINETIS__ ) || defined( __MCHP__ )
static unsigned int ( *start_i2c_p )            ( void );
static unsigned int ( *write_i2c_p )            ( unsigned char slave_address,
                                                  unsigned char *buffer,
                                                  unsigned long count,
                                                  unsigned long end_mode );
static void         ( *read_i2c_p )             ( unsigned char slave_address,
                                                  unsigned char *buffer,
                                                  unsigned long count,
                                                  unsigned long end_mode );
    #elif defined( __TI__ )
static void         ( *enable_i2c_p )           ( void );
static void         ( *disable_i2c_p )          ( void );
static void         ( *set_slave_address_i2c_p )( char slave_address,
                                                  char dir );
static unsigned char( *write_i2c_p )            ( unsigned char data_in,
                                                  unsigned char mode );
static unsigned char( *read_i2c_p )             ( unsigned char *data_out,
                                                  unsigned char mode );
    #endif
#elif  defined( __MIKROC_PRO_FOR_AVR__ )
static unsigned char( *busy_i2c_p )             ( void );
static unsigned char( *status_i2c_p )           ( void );
static unsigned char( *start_i2c_p )            ( void );
static void         ( *stop_i2c_p )             ( void );
static void         ( *close_i2c_p )            ( void );
static void         ( *write_i2c_p )            ( unsigned char data_out );
static unsigned char( *read_i2c_p )             ( unsigned char ack );

#elif  defined( __MIKROC_PRO_FOR_PIC__ )
static unsigned char( *is_idle_i2c_p )          ( void );
static unsigned char( *start_i2c_p )            ( void );
static void         ( *stop_i2c_p )             ( void );
static void         ( *restart_i2c_p )          ( void );
static unsigned char( *write_i2c_p )            ( unsigned char data_out );
static unsigned char( *read_i2c_p )             ( unsigned char ack );

#elif defined( __MIKROC_PRO_FOR_PIC32__ )
static unsigned int ( *is_idle_i2c_p )          ( void );
static unsigned int ( *start_i2c_p )            ( void );
static void         ( *stop_i2c_p )             ( void );
static unsigned int ( *restart_i2c_p )          ( void );
static unsigned int ( *write_i2c_p )            ( unsigned char data_out );
static unsigned char( *read_i2c_p )             ( unsigned int ack );

#elif defined( __MIKROC_PRO_FOR_DSPIC__ )
static unsigned int ( *is_idle_i2c_p )          ( void );
static unsigned int ( *start_i2c_p )            ( void );
static void         ( *stop_i2c_p )             ( void );
static void         ( *restart_i2c_p )          ( void );
static unsigned int ( *write_i2c_p )            ( unsigned char data_out );
static unsigned char( *read_i2c_p )             ( unsigned int ack );

#elif defined( __MIKROC_PRO_FOR_8051__ )
static unsigned char( *status_i2c_p )           ( void );
static unsigned char( *start_i2c_p )            ( void );
static void         ( *stop_i2c_p )             ( void );
static void         ( *close_i2c_p )            ( void );
static void         ( *write_i2c_p )            ( unsigned char data_out );
static unsigned char( *read_i2c_p )             ( unsigned char ack );

#elif defined( __MIKROC_PRO_FOR_FT90x__ )
static void         ( *soft_reset_i2c_p )       ( void );
static void         ( *set_slave_address_i2c_p )( unsigned char slave_address );
static unsigned char( *write_i2c_p )            ( unsigned char *buffer,
                                                  unsigned int count );
static unsigned char( *read_i2c_p )             ( unsigned char *buffer,
                                                  unsigned int count );
#endif

/*		Public Function Definitions
 ******************************************************************************/

void hal_delay( uint16_t ms )
{
#if defined(__MIKROC_PRO_FOR_ARM__)     || \
    defined( __MIKROC_PRO_FOR_FT90x__ ) || \
    defined( __MIKROC_PRO_FOR_AVR__ )   || \
    defined( __MIKROC_PRO_FOR_8051__ )  || \
    defined( __MIKROC_PRO_FOR_DSPIC__ ) || \
    defined( __MIKROC_PRO_FOR_PIC32__ ) || \
    defined( __MIKROC_PRO_FOR_PIC__ )
    while( ms-- )
        Delay_1ms();
#endif
}

int hal_air_quality_2_init( void )
{
#if defined( __MIKROC_PRO_FOR_ARM__ )
    #if defined( __STM32__ ) || defined( __KINETIS__ ) || defined( __MCHP__ )
    start_i2c_p                 = I2C_Start_Ptr;
    write_i2c_p                 = I2C_Write_Ptr;
    read_i2c_p                  = I2C_Read_Ptr;

    #elif defined( __TI__ )
    enable_i2c_p                = I2C_Enable_Ptr;
    disable_i2c_p               = I2C_Disable_Ptr;
    set_slave_address_i2c_p     = I2C_Master_Slave_Addr_Set_Ptr;
    write_i2c_p                 = I2C_Write_Ptr;
    read_i2c_p                  = I2C_Read_Ptr;
    #endif

#elif defined( __MIKROC_PRO_FOR_AVR__ )
    busy_i2c_p                  = TWI_Busy;
    status_i2c_p                = TWI_Status;
    close_i2c_p                 = TWI_Close;
    start_i2c_p                 = TWI_Start;
    stop_i2c_p                  = TWI_Stop;
    write_i2c_p                 = TWI_Write;
    read_i2c_p                  = TWI_Read;

#elif defined( __MIKROC_PRO_FOR_PIC__ )
    is_idle_i2c_p               = I2C1_Is_Idle;
    start_i2c_p                 = I2C1_Start;
    stop_i2c_p                  = I2C1_Stop;
    restart_i2c_p               = I2C1_Repeated_Start;
    write_i2c_p                 = I2C1_Wr;
    read_i2c_p                  = I2C1_Rd;

#elif defined( __MIKROC_PRO_FOR_PIC32__ )
    is_idle_i2c_p               = I2C_Is_Idle_Ptr;
    start_i2c_p                 = I2C_Start_Ptr;
    stop_i2c_p                  = I2C_Stop_Ptr;
    restart_i2c_p               = I2C_Restart_Ptr;
    write_i2c_p                 = I2C_Write_Ptr;
    read_i2c_p                  = I2C_Read_Ptr;

#elif defined( __MIKROC_PRO_FOR_DSPIC__ )
    is_idle_i2c_p               = I2C_Is_Idle;
    start_i2c_p                 = I2C_Start;
    stop_i2c_p                  = I2C_Stop;
    restart_i2c_p               = I2C_Restart;
    write_i2c_p                 = I2C_Write;
    read_i2c_p                  = I2C_Read;

#elif defined( __MIKROC_PRO_FOR_8051__ )
    status_i2c_p                = TWI_Status;
    close_i2c_p                 = TWI_Close;
    start_i2c_p                 = TWI_Start;
    stop_i2c_p                  = TWI_Stop;
    write_i2c_p                 = TWI_Write;
    read_i2c_p                  = TWI_Read;

#elif defined( __MIKROC_PRO_FOR_FT90x__ )
    soft_reset_i2c_p            = I2CM_Soft_Reset_Ptr;
    set_slave_address_i2c_p     = I2CM_Set_Slave_Address_Ptr;
    write_i2c_p                 = I2CM_Write_Bytes_Ptr;
    read_i2c_p                  = I2CM_Read_Bytes_Ptr;
#endif


    return 0;
}

int hal_air_quality_2_read
(
        uint8_t i2c_address,
        uint8_t* buffer,
        uint16_t count 
)
{
    int res = 0;
    uint8_t *ptr = buffer;

#if defined(__MIKROC_PRO_FOR_ARM__)
    #if defined( __STM32__ ) || defined( __KINETIS__ ) || defined( __MCHP__ )
    res |= start_i2c_p();
    read_i2c_p( i2c_address, ptr, count, END_MODE_STOP );
    #endif

#elif defined( __MIKROC_PRO_FOR_FT90x__ )
    set_slave_address_i2c_p( i2c_address );
    res |= read_i2c_p( ptr, count );

#elif defined( __MIKROC_PRO_FOR_AVR__ )    
    start_i2c_p();
    write_i2c_p( ( i2c_address << 1 ) | READ_BIT );
    while( --count )
        *ptr++ = read_i2c_p( ACK_BIT );

    *ptr++ = read_i2c_p( NACK_BIT );
    stop_i2c_p();
    
#elif defined( __MIKROC_PRO_FOR_PIC32__ )  || \
      defined( __MIKROC_PRO_FOR_8051__ )   || \
      defined( __MIKROC_PRO_FOR_PIC__ )    || \
      defined( __MIKROC_PRO_FOR_DSPIC__ )
    res |= start_i2c_p();
    res |= write_i2c_p( ( i2c_address << 1 ) | READ_BIT );
    while( --count )
        *ptr++ = read_i2c_p( ACK_BIT );

    *ptr++ = read_i2c_p( NACK_BIT );
    stop_i2c_p();
#endif
    return res;
}


/* 								End of File
 ******************************************************************************/
