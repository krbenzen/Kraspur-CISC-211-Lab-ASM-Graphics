/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */



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
#include <malloc.h>
#include <inttypes.h>   // required to print out pointers using PRIXPTR macro
#include "definitions.h"                // SYS function prototypes
#include "projGlobalDefinitions.h" 
#include "printFuncs.h"  // lab print funcs

extern uint32_t * rowA00ptr;  // for debug

#define ROW_PREAMBLE_LEN 3  // space for 2 digit row number + frame character
#define ROW_CRLF_LEN 3   // space for "<frame char>\r\n"

#define NUM_C_DISPLAY_BUFS 2

#define PIXEL_ON  '*'
#define PIXEL_OFF ' '

// static char * frameChar = "|";
static char * col0 = "             111111111122222222223333333333444444444455555555556666\r\n";
static char * col1 = "   0123456789012345678901234567890123456789012345678901234567890123\r\n";
static char * col2 = "  |----------------------------------------------------------------|\r\n";

static uint8_t txBuffer[MAX_PRINT_LEN] = {0};

// define an array of rows of characters, with an extra char for terminating null
static char pixelRow[DRAW_BUF_PIX_WIDTH+1];
// define a 1D array of chars where each row can be cat'd to, with extra chars at end
// of each row for \r\n, and for a terminating null for the whole thing
static char pixelArray[DRAW_BUF_PIX_HEIGHT*(DRAW_BUF_PIX_WIDTH+ROW_PREAMBLE_LEN+ROW_CRLF_LEN)+1];


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

uint64_t rotate(uint64_t v, int n) 
{
    while(n<0) n += 64;
    // n = n & 63U;
    if (n)
    {
        v = (v >> n) | (v << (64-n));        
    }

    return v; 
}



/*  printBuf: for each bit set to a 1, put a '*' in the equivalent location
 *            in a char array. At the end of each display line, add CR-LF.
 *            At the completion of each line, append the current line to
 *            a buffer that contains the entire string that will be printed
 *            to the display. When all lines have been processed, append
 *            a cursor control command to reposition the cursor back to the
 *            top of the display area.
 */
void printBuf (char * namePtr,
        uint64_t * buf, 
        int32_t frameNum, 
        drawCommand cmd,
        bool use32bitMode,
        volatile bool *txCompletePtr)
{
    // FOR DEBUG!!!!!!!!!!!!!!!!!!!
    // buf = (uint64_t *) rowA00ptr;
    
    pixelArray[0] = '\0'; // start with an empty output string
    // create a temp char array to hold the row preamble string
    char rowPreamble[10] = {0}; // place to store the line mum and frame char
    int32_t displayWidth = DRAW_BUF_PIX_WIDTH;
    int32_t displayHeight = DRAW_BUF_PIX_HEIGHT;
    for (int32_t rowNum = 0; rowNum < displayHeight; ++rowNum)
    {
        for (int32_t pixNum = 0; pixNum < displayWidth; ++pixNum)
        {
            // access the two 32b contiguous words as a single
            // 64bit word that contain  the current pixel's data
            //calculate the pointer offset
            uint64_t * buf2 = ((uint64_t *) buf) + rowNum; 
            // access the data
            uint64_t inputWord = *buf2;
            if(use32bitMode == true)
            {
                inputWord = rotate(inputWord,32); // fix endianness
            }
            // shift the current pixel value to the most significant bit.
            // check to see if 1 or 0.
            uint64_t pixValue = (inputWord<<pixNum) & 0x8000000000000000L;
            pixelRow[pixNum] = ' '; // if pixel is 0 (off), set char to blank
            if (pixValue != 0)
            {
                pixelRow[pixNum] = '*'; // if pixel is 1 (on), set char to '*'
            }
    }
        // Row is complete, add it to the output string
        // TODO: Make sure we don't write past the end of the txBuffer!
        pixelRow[displayWidth] = '\0';  // add terminating null
        // build and append the output row to the pixelArr
        snprintf((char *)rowPreamble,(ROW_PREAMBLE_LEN+1),
                "%2ld|",
                rowNum);
        strcat(pixelArray,rowPreamble);
        strcat(pixelArray,pixelRow);
        strcat(pixelArray,"|\r\n");
    }
    snprintf((char*)txBuffer, MAX_PRINT_LEN,
            "==== %s: Frame Number: %4ld; cmd: [u/d: %ld] [l/r: %ld] [rst: %ld]\r\n"
            "buf pointer:  0x%" PRIXPTR "\r\n"
            "%s"
            "%s"
            "%s"
            "%s"
            "%s"
            "\x1B[26A", // move the cursor NN rows up, back to the top of the output
            namePtr, frameNum, cmd[0], cmd[1], cmd[2],
            (uintptr_t) buf,
            col0, col1, col2,
            pixelArray,
            col2);
    printAndWait((char*)txBuffer,txCompletePtr);
}




/** 
  @Function
    int ExampleInterfaceFunctionName ( int param1, int param2 ) 

  @Summary
    Brief one-line description of the function.

  @Remarks
    Refer to the example_file.h interface header for function usage details.
 */
void printAndWait(char *txBuffer, volatile bool *txCompletePtr)
{
    *txCompletePtr = false;
    
    DMAC_ChannelTransfer(DMAC_CHANNEL_0, txBuffer, \
        (const void *)&(SERCOM5_REGS->USART_INT.SERCOM_DATA), \
        strlen((const char*)txBuffer));
    // spin here, waiting for timer and UART to complete
    while (*txCompletePtr == false); // wait for print to finish
    /* reset it for the next print */


}

