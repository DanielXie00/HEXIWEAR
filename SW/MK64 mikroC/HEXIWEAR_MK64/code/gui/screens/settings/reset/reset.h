// File: reset.h

// Declarations of main functionalities in the reset gui module.

#ifndef _reset_h_
#define _reset_h_

////////////////////////////////////////////////////////////////////////////////
// Header file inclusions.                                                    //
////////////////////////////////////////////////////////////////////////////////

#include "gui_driver.h"

////////////////////////////////////////////////////////////////////////////////
// Global variable declarations.                                              //
////////////////////////////////////////////////////////////////////////////////
 
// screens
extern guiScreen_t resetScreen;

////////////////////////////////////////////////////////////////////////////////
// Global function declarations.                                              //
////////////////////////////////////////////////////////////////////////////////

void reset_Init( void* param );
void reset_CreateTasks( void* param );
void reset_DestroyTasks( void* param );

#endif