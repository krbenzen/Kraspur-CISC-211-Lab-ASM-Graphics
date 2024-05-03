/* ************************************************************************** */
/** Descriptive File Name

  @File Name
    projGlobalDefinitions.h

  @Summary
 defines and constants shared across all files in this project

 */
/* ************************************************************************** */

#ifndef _PROJ_GLOBAL_DEFINITIONS_H    /* Guard against multiple inclusion */
#define _PROJ_GLOBAL_DEFINITIONS_H

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


// Ideally, the C code would be completely parameterized according to these
// values, so that it would be easy to change the display size. However,
// for now, some things (like the cursor control sequence in printBuf())
// do not use these paramaters.    
#define DRAW_BUF_PIX_WIDTH 64
#define DRAW_BUF_PIX_HEIGHT 20
#define DRAW_BUF_NUM_WORDS_IN_ROW ((DRAW_BUF_PIX_WIDTH/32))

#define MAX_PRINT_LEN 3000

#define VERT_SHIFT 0
#define HOR_SHIFT  1
#define RESET_CMD  2
    
/***
 *
 * cmd[VERT_SHIFT]:  -N: move up N pixels; +N: move down N pixels
 * cmd[HOR_SHIFT]:   -N: move left N pixels; +N: move right N pixels
 * cmd[VERT_SHIFT]: 0/1: 0: do shifts specified by VERT_SHIFT and HOR_SHIFT
 *                       1: reset first display buf to original state and display it
 *
 */
typedef int32_t drawCommand[3];


extern char * nameStrPtr;

#ifdef __cplusplus
}
#endif

#endif /* _PROJ_GLOBAL_DEFINITIONS_H */

/* *****************************************************************************
 End of File
 */
