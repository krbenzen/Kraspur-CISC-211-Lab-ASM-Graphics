/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project. It is intended to
    be used as the starting point for CISC-211 Curiosity Nano Board
    programming projects. After initializing the hardware, it will
    go into a 0.5s loop that calls an assembly function specified in a separate
    .s file. It will print the iteration number and the result of the assembly 
    function call to the serial port.
    As an added bonus, it will toggle the LED on each iteration
    to provide feedback that the code is actually running.
  
    NOTE: PC serial port should be set to 115200 rate.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include <float.h>
#include <malloc.h>
#include "definitions.h"                // SYS function prototypes
#include "projGlobalDefinitions.h" 
#include "printFuncs.h"  // lab print funcs


#define PROF_MODE 0

#if PROF_MODE
#include "cDraw.h"  // lab print funcs
#endif

drawCommand scene0[] = {
    {0,0,1},
    {1,1,0},
    {5,5,0},
    {-3,0,0},
    {0,-2,0},
    {0,0,0},  // pause
    {0,0,0},  // pause
    {0,0,0}  // pause
};


// Define the global that gives access to the student's name
extern char * nameStrPtr;
extern uint32_t * rowA00ptr;  // for debug

/* RTC Time period match values for input clock of 1 KHz */
#define PERIOD_100MS                            102
#define PERIOD_500MS                            512
#define PERIOD_1S                               1024
#define PERIOD_2S                               2048
#define PERIOD_4S                               4096

static volatile bool isRTCExpired = false;
static volatile bool isUSARTTxComplete = true;

// PROF COMMENT:
// The ARM calling convention permits the use of up to 4 registers, r0-r3
// to pass data into a function. Only one value can be returned from a C
// function call. The assembly language routine stores the return value
// in r0. The C compiler will automatically use it as the function's return
// value.
//
// Function signature
// For this lab, return a pointer to the buffer containing the pixel
// data to be rendered to screen. In asm, it's declared as an array of
// 32bit words. For convenience in the C program, we're treating the pointer
// as a type uint64_t *.
extern uint64_t * asmDraw(uint32_t upDown, uint32_t leftRight, uint32_t resetCmd);

static void rtcEventHandler (RTC_TIMER32_INT_MASK intCause, uintptr_t context)
{
    if (intCause & RTC_MODE0_INTENSET_CMP0_Msk)
    {            
        isRTCExpired    = true;
    }
}
static void usartDmaChannelHandler(DMAC_TRANSFER_EVENT event, uintptr_t contextHandle)
{
    if (event == DMAC_TRANSFER_EVENT_COMPLETE)
    {
        isUSARTTxComplete = true;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main ( void )
{
        
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    DMAC_ChannelCallbackRegister(DMAC_CHANNEL_0, usartDmaChannelHandler, 0);
    RTC_Timer32CallbackRegister(rtcEventHandler, 0);
    RTC_Timer32Compare0Set(PERIOD_1S);
    RTC_Timer32CounterSet(0);
    RTC_Timer32Start();
    

    int32_t idleCount = 0;

    // Toggle the LED to show proof of life
    LED0_Toggle();

    // reset the state variables
    isRTCExpired = false;
    isUSARTTxComplete = false;
    
    drawCommand cmd = {0,0,1}; // vert, hor shift, and reset cmd values

    bool use32bitMode = true; // students use 32bit words

#if PROF_MODE   // call the C demo routine (in PROF repo only)
    uint64_t * bufPtr = cDraw(cmd);
    use32bitMode = false; // prof c demo code uses 64bit words
#else           // call student's asm draw routine
    uint64_t * bufPtr = asmDraw(cmd[0], cmd[1], cmd[2]); // send a reset
#endif
    
    printBuf(nameStrPtr, bufPtr, idleCount, cmd, use32bitMode,
            &isUSARTTxComplete);
    // printBuf(nameStrPtr, rowA00ptr, idleCount, cmd, use32bitMode, &isUSARTTxComplete);

    // spin here until the timer has expired
    //while  (false == isUSARTTxComplete ); 
    while (isRTCExpired == false);

    ++idleCount;

    while(1)
    {
        // Toggle the LED to show proof of life
        LED0_Toggle();
        
        // reset the state variables
        isRTCExpired = false;
        isUSARTTxComplete = false;
        
        // create a non-reset command for testing purposes
        cmd[VERT_SHIFT] = -2;
        cmd[HOR_SHIFT] =  -4;
        cmd[RESET_CMD] =  0;
        
        // bufPtr = asmDraw(cmd[0], cmd[1], cmd[2]);
        
#if PROF_MODE   // call the C demo routine (in PROF repo only)
        bufPtr = cDraw(cmd);
#else           // call student's asm draw routine
        bufPtr = asmDraw(cmd[0], cmd[1], cmd[2]); // send a reset
#endif
        
        printBuf(nameStrPtr, bufPtr, idleCount, cmd, use32bitMode,
                &isUSARTTxComplete);
                
        // spin here until the timer has expired
        //while  (false == isUSARTTxComplete ); 
        while (isRTCExpired == false);
        
        ++idleCount;

    }
                
            
    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}
/*******************************************************************************
 End of File
*/

