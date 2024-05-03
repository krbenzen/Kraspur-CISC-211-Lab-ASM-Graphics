/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _PRINTFUNCS_H    /* Guard against multiple inclusion */
#define _PRINTFUNCS_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

void printBuf(char * namePtr,
        uint64_t *drawBuf, 
        int32_t frameNum, 
        drawCommand cmd,
        bool use32bitMode, // students use 32b mode; prof demo uses 64b
        volatile bool *txCompletePtr);

void printAndWait(char *txBuffer, volatile bool *txCompletePtr);


/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _PRINTFUNCS_H */

/* *****************************************************************************
 End of File
 */
