/*** asmEncrypt.s   ***/
// Tell the assembler to allow both 16b and 32b extended Thumb instructions
.syntax unified

//   #include <xc.h>

// Declare the following to be in data memory 
.data  

/* create a string */
.global nameStr
.type nameStr,%gnu_unique_object
    
/*** STUDENTS: Change the next line to your name!  **/
nameStr: .asciz "Inigo Montoya"  
.align
 
/* initialize a global variable that C can access to print the nameStr */
.global nameStrPtr
.type nameStrPtr,%gnu_unique_object
nameStrPtr: .word nameStr   /* Assign the mem loc of nameStr to nameStrPtr */

 // Define the globals so that the C code can access them
// (in this lab we return the pointer, so strictly speaking,
// doesn't really need to be defined as global)

.equ NUM_WORDS_IN_BUF, 40
.equ NUM_BYTES_IN_BUF, (4 * NUM_WORDS_IN_BUF)
 
.align
 
@ records the current frame number so asmDraw can choose appropriate buffer
asmFrameCounter: .word 0

.global rowA00ptr
.type rowA00ptr,%gnu_unique_object
rowA00ptr: .word rowA00

// Flying Saucer!!!
// If you choose to modify this starting graphic, make sure your replacement
// is exactly 2 words wide and 20 rows high, just like this one.
rowA00: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA01: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA02: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA03: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA04: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA05: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA06: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA07: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA08: .word 0b00000000000000000000000000000011,0b11000000000000000000000000000000
rowA09: .word 0b00000000000000000000000000000100,0b00100000000000000000000000000000
rowA10: .word 0b00000000000000000000000011111111,0b11111111000000000000000000000000
rowA11: .word 0b00000000000000000000000100000000,0b00000000100000000000000000000000
rowA12: .word 0b00000000000000000000000011111111,0b11111111000000000000000000000000
rowA13: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA14: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA15: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA16: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA17: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA18: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000
rowA19: .word 0b00000000000000000000000000000000,0b00000000000000000000000000000000


// display buffers 0 and 1: 2 words (64 bits) wide, by 20 words high,
// initialized at boot time to pre-determined values
// REMEMBER! These are only initialized once, before the first time asmDraw
// is called. If you want to clear them (i.e. set all bits to 0), you need to
// add a function to do this in your assembly code.
 
buf0: .space NUM_BYTES_IN_BUF, 0xF0
buf1: .space NUM_BYTES_IN_BUF, 0x0F

// Tell the assembler that what follows is in instruction memory    
.text
.align


    
/********************************************************************
function name: asmDraw(downUp, rightLeft, reset)
function description:
Note: r0 and r1 are optional. The C test code uses them 
as shown below. However, your code can choose to ignore them, and update
buf0 and buf1 any way you'd like whenever asmDraw is called.
However, r2 should always reset your animation to its starting value
         
Inputs: r0: upDown:    -N: move up (towards row00) N pixels
                        0: do not move in the vertical direction
                        N: (positive number): move down (towards row19)
        r1: leftRight: -N: move left N pixels
                        0: do not move in the horizontal direction
                        N: (positive number): move right N pixels
        r2: reset:      0: do the commands specified by other input
                           parameters
                        1: ignore the other input parameters. Reset the
                           display to its original state.
 Outputs: r0: pointer to memory buffer containing updated display data

 Notes: * Do not modify the data in any of the rowA** loctions! Use
          it as your reset data, to start over when commanded.
          Use the space allocated for buf0 and buf1 to capture your
          output data. 
        * The first call to the asmDraw code will always be a reset, so that
          you can copy clean data from rowA** into an output buffer.
        * The reset should always return the address of buf0 in r0,
                 e.g.   LDR r0,=buf0
        * Each subsequent call with a valid non-reset command should return
          the address of the other buffer. This allows you to copy and
          modfiy data from the previous buffer to generate your next
          buffer. So, if the last call returned buf0, the current call 
          should return buf1. And the call after that should once again 
          return buf0.
        * You can create more bufs if you need them for some reason.
        * You should create your own mem locations to store info such
          as which buf was the last one used.

********************************************************************/     
.global asmDraw
.type asmDraw,%function
asmDraw:   

    // Students: The code below is provided as a starting point. It ignores
    //           the inputs in r0, r1, and r2. It just alternates between
    //           buf0 and buf1 and returns the addresses of one or the
    //           other buffer. The C code displays the default values stored
    //           in those buffers. You can completely delete this code and 
    //           replace it with your own creation.
    
    // save the caller's registers, as required by the ARM calling convention
    push {r4-r11,LR}
    
    cbz r2, getNextFrame
    
    // reset the frame counter
    LDR r4,=asmFrameCounter
    LDR r5,=0
    STR r5,[r4]
    // TODO: copy rowA** data to buf0
    // for now, just use whatever is currently stored in buf0
    LDR r0,=buf0
    b done
    
getNextFrame:
    // This is where you decide what to do in the next animation frame.
    // increment the frame counter
    LDR r4,=asmFrameCounter
    LDR r5,[r4]
    ADD r5,r5,1
    STR r5,[r4]  // store it back to mem
    
    LDR r0,=buf0 // set the return value to buf0
    // if the cycle count is an odd number set it to the alternate buffer
    TST r5,1
    LDRNE r0,=buf1 
    B done // branch for clarity...
        
    done:
    
    // Students:
    // If you just want to see the UFO, uncomment the next line
    // But this is only for demonstration purposes! Your final code should
    // shold flip between buf0 and buf1 and return one of those two.
    // LDR r0,=rowA00
 
    // restore the caller's registers, as required by the ARM calling convention
    pop {r4-r11,LR}

    mov pc, lr	 /* asmEncrypt return to caller */
   

/**********************************************************************/   
.end  /* The assembler will not process anything after this directive!!! */
           




