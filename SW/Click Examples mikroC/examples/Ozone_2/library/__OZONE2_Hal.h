/*

    __OZONE2_Hal.h

    Copyright (c) 2011-2017 MikroElektronika.  All right reserved.

------------------------------------------------------------------------------*/
/**
@file       __OZONE2_Hal.h
@brief        Ozone_2 HAL
@addtogroup   OZONE2
@{

@defgroup     OZONE2_HAL
@brief Ozone_2 Click Hardware Abstract Layer
@{

| HAL Info              |                    |
|:---------------------:|:------------------:|
| Version               | **3.0.0**          |
| Date                  | **Aug 2017**       |
| Developer             | **MikroE FW Team** |

### HAL Description : ###

- Public Functions Prefix : ```HAL_OZONE2``` 
- Private Functions Prefix : ```hal_```

MikroC HAL covers all mikroE compilers and it can be divided to 4 parts :
 - GPIO
 - SPI
 - I2C
 - UART

HAL Selection can setup each of this parts to be compiled depend on peripherals
used by the driver (upper layer). For more details about how to use this library 
on non mikroE compilers check OZONE2_PORTING_GUIDE.

*/
/* -------------------------------------------------------------------------- */

#include <stdint.h>

#ifndef _OZONE2_HAL_
#define _OZONE2_HAL_

#ifdef __MIKROC_PRO_FOR_ARM__
#define __MIKROC__
#ifdef MSP432P401R
#define __MSP__
#endif
#endif
#ifdef __MIKROC_PRO_FOR_DSPIC__
#define __MIKROC__
#endif
#ifdef __MIKROC_PRO_FOR_PIC__
#define __MIKROC__
#endif
#ifdef __MIKROC_PRO_FOR_PIC32__
#define __MIKROC__
#endif
#ifdef __MIKROC_PRO_FOR_AVR__
#define __MIKROC__
#endif
#ifdef __MIKROC_PRO_FOR_FT90x__
#define __MIKROC__
#endif

/** 
 * @name                                                         HAL_Selection
 * @{                                                      *///-----------------

 #define __OZONE2_GPIO                                             /**< @brief GPIO HAL Selection */
 #define  __OZONE2_SPI                                              /**< @brief SPI HAL Selection */
//#define  __OZONE2_I2C                                              /**< @brief I2C HAL Selection */
//#define __OZONE2_UART                                             /**< @brief UART HAL Selection */

#define __OBJECT_HAL__                                                          /**< @brief Enables object C HAL compilation */

/// @} 
/** 
 * @name                                           HAL_Function_argument_types
 * @{                                        *///-------------------------------

#ifdef __MIKROC__
#ifdef __OBJECT_HAL__
#define T_HAL_GPIO_OBJ    const uint8_t*
#define T_HAL_SPI_OBJ     const uint8_t*
#define T_HAL_I2C_OBJ     const uint8_t*
#define T_HAL_UART_OBJ    const uint8_t*
#endif
#endif

/// @}
/** 
 * @name                                         Default peripheral parameters
 * @{                                      *///---------------------------------

#ifdef __MIKROC__
extern const uint32_t _OZONE2_SPI_CFG[];
extern const uint32_t _OZONE2_I2C_CFG[];
extern const uint32_t _OZONE2_UART_CFG[];
#endif

/// @}
/** @name                                                       HAL_GPIO_Types
 *  @{                                                   *///-------------------

#ifdef __OZONE2_GPIO
typedef void    (*T_OZONE2_GPIO_Set)(uint8_t);                         
typedef uint8_t (*T_OZONE2_GPIO_Get)();

#ifdef __OBJECT_HAL__
typedef struct
{
    T_OZONE2_GPIO_Set      gpioSet[ 12 ];
    T_OZONE2_GPIO_Get      gpioGet[ 12 ];
  
}T_OZONE2_gpioObj;

#endif
#endif

/// @} 
/** @name                                                        HAL_SPI_Types
 *  @{                                                     *///-----------------

#ifdef __OZONE2_SPI
#ifdef __MIKROC_PRO_FOR_ARM__ 
typedef void          ( *T_OZONE2_SPI_Write )(unsigned int);
typedef unsigned int  ( *T_OZONE2_SPI_Read )(unsigned int);
#endif
#ifdef __MIKROC_PRO_FOR_DSPIC__
typedef void                              ( *T_OZONE2_SPI_Write )(unsigned int);
typedef unsigned int         ( *T_OZONE2_SPI_Read )(unsigned int);
#endif
#ifdef __MIKROC_PRO_FOR_AVR__ 
typedef void          ( *T_OZONE2_SPI_Write )(unsigned char);
typedef unsigned char ( *T_OZONE2_SPI_Read )(unsigned char);
#endif
#ifdef __MIKROC_PRO_FOR_PIC__
typedef void                             ( *T_OZONE2_SPI_Write )(unsigned char);
typedef unsigned char ( *T_OZONE2_SPI_Read )(unsigned char);
#endif
#ifdef __MIKROC_PRO_FOR_PIC32__
typedef void                             ( *T_OZONE2_SPI_Write )(unsigned long);
typedef unsigned long        ( *T_OZONE2_SPI_Read )(unsigned long);
#endif
#ifdef __MIKROC_PRO_FOR_FT90x__
typedef void                             ( *T_OZONE2_SPI_Write )(unsigned char);
typedef unsigned char ( *T_OZONE2_SPI_Read )(unsigned char);
#endif

#ifdef __OBJECT_HAL__
typedef struct
{
    T_OZONE2_SPI_Write       spiWrite;
    T_OZONE2_SPI_Read        spiRead;
  
}T_OZONE2_spiObj;

#endif
#endif

/// @} 
/** @name                                                        HAL_I2C_Types
 *  @{                                                     *///-----------------

#ifdef __OZONE2_I2C
#ifdef __MIKROC_PRO_FOR_ARM__
#if defined( __STM32__ ) || defined( __KINETIS__ ) || defined( __MCHP__ )
typedef unsigned int        ( *T_OZONE2_I2C_Start )();
/* DUMMY */ typedef void ( *T_OZONE2_I2C_Stop )();
/* DUMMY */ typedef void ( *T_OZONE2_I2C_Restart )();                                                
typedef unsigned int         ( *T_OZONE2_I2C_Write )(unsigned char, unsigned char*, unsigned long, unsigned long);
typedef void                             ( *T_OZONE2_I2C_Read )(unsigned char, unsigned char*, unsigned long, unsigned long);
    #elif defined( __MSP__ )
/* DUMMY */ typedef void ( *T_OZONE2_I2C_Start )();
/* DUMMY */ typedef void ( *T_OZONE2_I2C_Stop )();
/* DUMMY */ typedef void ( *T_OZONE2_I2C_Restart )();
typedef unsigned int         ( *T_OZONE2_I2C_Write )(unsigned char, unsigned char*, unsigned long, unsigned long);
typedef void                             ( *T_OZONE2_I2C_Read )(unsigned char, unsigned char*, unsigned long, unsigned long);
    #elif defined( __TI__ )
typedef void                             ( *T_OZONE2_I2C_Start )(char, char);
/* DUMMY */ typedef void ( *T_OZONE2_I2C_Stop )();
/* DUMMY */ typedef void ( *T_OZONE2_I2C_Restart )();
typedef unsigned char ( *T_OZONE2_I2C_Write )(unsigned char, unsigned char);
typedef unsigned char        ( *T_OZONE2_I2C_Read )(unsigned char*, unsigned char);
#endif
#endif
#ifdef __MIKROC_PRO_FOR_AVR__
typedef unsigned char        ( *T_OZONE2_I2C_Start )();
typedef void                             ( *T_OZONE2_I2C_Stop )();
/* DUMMY */ typedef void ( *T_OZONE2_I2C_Restart )();
typedef void                             ( *T_OZONE2_I2C_Write )(unsigned char);
typedef unsigned char ( *T_OZONE2_I2C_Read )(unsigned char);
#endif
#ifdef __MIKROC_PRO_FOR_PIC__
typedef unsigned char        ( *T_OZONE2_I2C_Start )();
typedef void                             ( *T_OZONE2_I2C_Stop )();
typedef void                             ( *T_OZONE2_I2C_Restart )();
typedef unsigned char        ( *T_OZONE2_I2C_Write )(unsigned char);
typedef unsigned char        ( *T_OZONE2_I2C_Read )(unsigned char);
#endif
#ifdef __MIKROC_PRO_FOR_PIC32__
typedef unsigned int         ( *T_OZONE2_I2C_Start )();
typedef void                             ( *T_OZONE2_I2C_Stop )();
typedef unsigned int         ( *T_OZONE2_I2C_Restart )();
typedef unsigned int         ( *T_OZONE2_I2C_Write )(unsigned char);
typedef unsigned char ( *T_OZONE2_I2C_Read )(unsigned int);
#endif
#ifdef __MIKROC_PRO_FOR_DSPIC__
typedef unsigned int         ( *T_OZONE2_I2C_Start )();
typedef void                             ( *T_OZONE2_I2C_Stop )();
typedef void                             ( *T_OZONE2_I2C_Restart )();
typedef unsigned int         ( *T_OZONE2_I2C_Write )(unsigned char);
typedef unsigned char ( *T_OZONE2_I2C_Read )(unsigned int);
#endif
#ifdef __MIKROC_PRO_FOR_FT90x__
typedef void                             ( *T_OZONE2_I2C_Start )(unsigned char);
/* DUMMY */ typedef void ( *T_OZONE2_I2C_Stop )();
/* DUMMY */ typedef void ( *T_OZONE2_I2C_Restart )();
typedef unsigned char ( *T_OZONE2_I2C_Write )(unsigned char*, unsigned int);
typedef unsigned char ( *T_OZONE2_I2C_Read )(unsigned char*, unsigned int);
#endif

#ifdef __OBJECT_HAL__
typedef struct
{
    T_OZONE2_I2C_Start       i2cStart;
    T_OZONE2_I2C_Stop        i2cStop;
    T_OZONE2_I2C_Restart     i2cRestart;
    T_OZONE2_I2C_Write       i2cWrite;
    T_OZONE2_I2C_Read        i2cRead;
  
}T_OZONE2_i2cObj;

#endif
#endif

/// @} 
/** @name                                                       HAL_UART_Types
 *  @{                                                    *///------------------

#ifdef __OZONE2_UART
#ifdef __MIKROC_PRO_FOR_ARM__
typedef void          ( *T_OZONE2_UART_Write )(unsigned int);
typedef unsigned int  ( *T_OZONE2_UART_Read )();
typedef unsigned int  ( *T_OZONE2_UART_Ready )();
#endif
#ifdef __MIKROC_PRO_FOR_DSPIC__
typedef void          ( *T_OZONE2_UART_Write )(unsigned int);
typedef unsigned int  ( *T_OZONE2_UART_Read )();
typedef unsigned int  ( *T_OZONE2_UART_Ready )();
#endif 
#ifdef __MIKROC_PRO_FOR_PIC32__
typedef void                             ( *T_OZONE2_UART_Write )(unsigned int);
typedef unsigned int         ( *T_OZONE2_UART_Read )();
typedef unsigned int         ( *T_OZONE2_UART_Ready )();
#endif
#ifdef __MIKROC_PRO_FOR_AVR__
typedef void          ( *T_OZONE2_UART_Write )(unsigned char);
typedef unsigned char ( *T_OZONE2_UART_Read )();
typedef unsigned char ( *T_OZONE2_UART_Ready )();
#endif
#ifdef __MIKROC_PRO_FOR_PIC__
typedef void          ( *T_OZONE2_UART_Write )(unsigned char);
typedef unsigned char ( *T_OZONE2_UART_Read )();
typedef unsigned char ( *T_OZONE2_UART_Ready )();
#endif
#ifdef __MIKROC_PRO_FOR_FT90x__
typedef void                             ( *T_OZONE2_UART_Write )(unsigned char);
typedef unsigned char ( *T_OZONE2_UART_Read )();
typedef unsigned char ( *T_OZONE2_UART_Ready )();
#endif

#ifdef __OBJECT_HAL__
typedef struct
{
    T_OZONE2_UART_Write      uartWrite;
    T_OZONE2_UART_Read       uartRead;
    T_OZONE2_UART_Ready      uartReady;
  
}T_OZONE2_uartObj;

#endif
#endif

/// @} 

#ifdef __cplusplus
extern "C"{
#endif
#ifdef __OZONE2_GPIO

/** @name                                                 HAL_Public_Functions
 *  @{                                              *///------------------------
/**
 * @brief OZONE2 GPIO HAL Pointer Assignation
 *
 * @param[in] ctlCS     Set CS Pin Function Pointer
 * @param[in] ctlRDY    Get RDY Pin Function Pointer
 */
void 
HAL_OZONE2_gpioInit
(
        T_OZONE2_GPIO_Set      ctlCS,
        T_OZONE2_GPIO_Get      ctlRDY
);

#ifdef __OBJECT_HAL__

/**
 *
 */
void HAL_OZONE2_gpioMap(T_HAL_GPIO_OBJ gpioObj);

#endif
#endif
#ifdef __OZONE2_SPI

/**
 * @brief OZONE2 SPI HAL Pointer Assignation
 *
 * @param[in] fnWrite   SPI Write Function
 * @param[in] fnRead    SPI Read  Function
 *
 * Initialization of OZONE2 SPI driver.
 *
 * @note
 * This function must before usage of other driver functions.
 */
void HAL_OZONE2_spiInit
( 
        T_OZONE2_SPI_Write   fnWrite,
        T_OZONE2_SPI_Read    fnRead 
);

#ifdef __OBJECT_HAL__

/**
 *
 */
void HAL_OZONE2_spiMap(T_HAL_SPI_OBJ spiObj);

#endif
#endif
#ifdef __OZONE2_I2C

/**
 * @brief OZONE2 I2C Driver Initialization
 *
 * @param[in] fnStart   I2C Start   Function
 * @param[in] fnStop    I2C Stop    Function
 * @param[in] fnRestart I2C Restart Function
 * @param[in] fnWrite   I2C Write   Function
 * @param[in] fnRead    I2C Read    Function
 *
 * Initialization of OZONE2 I2C driver.
 *
 * @note
 * This function must be called first.
 *
 * @note
 * Some compilers don't have all the function pointers, for example
 * mikroE ARM compilers don't have the I2C_Stop function, in that case
 * it is enough to provide NULL pointer.
 */
void HAL_OZONE2_i2cInit
( 
        T_OZONE2_I2C_Start   fnStart,
        T_OZONE2_I2C_Stop    fnStop,
        T_OZONE2_I2C_Restart fnRestart,
        T_OZONE2_I2C_Write   fnWrite,
        T_OZONE2_I2C_Read    fnRead 
);

#ifdef __OBJECT_HAL__

/**
 *
 */
void HAL_OZONE2_i2cMap(T_HAL_I2C_OBJ i2cObj);

#endif
#endif
#ifdef __OZONE2_UART

/**
 * @brief OZONE2 UART Driver Initialization
 *
 * @param[in] fnWrite   UART Write Function
 * @param[in] fnRead    UART Ready Function
 * @param[in] fnReady   UART Data Ready Function
 *
 * Initialization of OZONE2 UART driver.
 *
 * @note
 * This function must be called first.
 */
void HAL_OZONE2_uartInit
( 
        T_OZONE2_UART_Write  fnWrite,
        T_OZONE2_UART_Read   fnRead,
        T_OZONE2_UART_Ready  fnReady 
);

#ifdef __OBJECT_HAL__

/**
 *
 */
void HAL_OZONE2_uartMap(T_HAL_UART_OBJ uartObj);

#endif
#endif
/// @}

#ifdef __cplusplus
} // extern "C"
#endif
#endif

/// @}
/// @}
/* -------------------------------------------------------------------------- */ 
/*
  __OZONE2_Hal.h

  Copyright (c) 2011-2017 MikroElektronika.  All right reserved.

    This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.

------------------------------------------------------------------------------*/