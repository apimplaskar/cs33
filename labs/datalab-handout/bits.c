/*
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 Aditya Pimplaskar
 UID: 804900470
 
 Assisted Ryan Wakefield with the final (extra credit) function by discussing a high-level overview of the solution -- no specifics were discussed.
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2012 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* We do support the IEC 559 math functionality, real and complex.  */
/* wchar_t uses ISO/IEC 10646 (2nd ed., published 2011-03-15) /
   Unicode 6.0.  */
/* We do not support C11 <threads.h>.  */
/* 
 * minusOne - return a value of -1 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int minusOne(void) {
  return ~0; // or 0xFFFFFFFF
}
/*
 * isZero - returns 1 if x == 0, and 0 otherwise 
 *   Examples: isZero(5) = 0, isZero(0) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int isZero(int x) {
  return !x;
    //!0 will yield a 1, true
    //!nonzero will yield 0 (boolean operator)
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
    //shift to get desired byte to the end
    int times = n << 3; //(n << 3) to multiply n by 8 to account for 8 bits in a byte
    x = x >> times;
    //extract only the last 8 bits
    return (x & 0xFF);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x + 1;
}
/* 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
    //facts to consider: for overflow to occur, x and y must either both be + or both be -
    //if sign of sum matches sign of x or y, overflow didn't occur
    int sum = x+y;
    int shift = 0x80 << 24;
    int signOfSum = sum & shift;
    int signOfX = x & shift;
    int signOfY = y & shift;
    int sumMatchesX = signOfSum ^ ~signOfX; // if true: 111111....111, if false: 011111.11111
    int sumMatchesY = signOfSum ^ ~signOfY;
    
    sumMatchesX = sumMatchesX & shift; //clears all bits except most significant bit, which holds relevant info
    sumMatchesY = sumMatchesY & shift;
    
    return !!(sumMatchesX | sumMatchesY); //need to use !! to get boolean value back
    
}
/* 
 * bitMask - Generate a mask consisting of all 1's 
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit) {
    //number of ones = highbit-lowbit+1;
        // highbit + (~lowbit + 1) + 1
     //Interesting case to handle: number of ones needed is 32
    
    // x = 11111111111110000000 <- ones up until lowbit
    // y = 00000000111111111111 <- ones starting at highbit
    // x&y
    int y = highbit ^ 31;
    int x = 0x80 << 24;
    x = x >> (32 + ~lowbit); //31 - lowbit
    
    // highbit is 0  -> y = 000000000000000001
    // highbit is 3  -> y = 000000000000001111
    // highbit is 31 -> y = 111111111111111111
    
    //declaration of y pushed up for dlc
    y = !!y; // (1 if highbit != 31 else 0)
    y = ((y << 31) >> (32 + ~highbit)) << 1;
    
    // highbit is 0  -> ~y = 1111111111111110
    // highbit is 3  -> ~y = 1111111111110000
    // highbit is 31 -> ~y = 0000000000000000
    
    y = ~y;
    return (x&y);
}
/* 
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x) {
    //need to distinguish case of x >=0 and x < 0 using bit operators
    //only the sign bit is relevant in deciding; if we can decipher the sign bit, it is just a matter of returning either the twos complement or the same value
    int signBitMask = 0x80 << 24;
    int signBit = (x & signBitMask) >> 31;
    
    //x is positive -- signBit is 0000...0000
    //x is negative -- signBit is 1111...1111

    // return -x if x is negative: signBit & (~x + 1)
        //if x is positive: this evaluates to 0
    int ifNegative = signBit & (~x + 1);
    
    // return x if x is positive: signBit & x
        //if x is negative: this evaluates to 0
    int ifPositive = ~signBit & x;
    
    //put or here because in either case, we will be adding zero for the opposite case
    return ifNegative + ifPositive;
}
    /*
 * greatestBitPos - return a mask that marks the position of the
 *               most significant 1 bit. If x == 0, return 0
 *   Example: greatestBitPos(96) = 0x40
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 70
 *   Rating: 4 
 */
int greatestBitPos(int x) {
    //step 1:
    //find highest nonzero byte and move it to the bottom
    //also want to know how much we shifted to get to that byte
    int shift = 0;
    int mask1;
    int mask2;
    int mask3 = 0xFF;
    int signif;
    
    mask3 = mask3 << 24; //all 1s except in byte 0,1,2 -- 0xFF000000
    mask2 = (mask3) >> 8; //all 1s except in byte 0,1  -- 0xFFFF0000
    mask1 = (mask2) >> 8; //all 1s except in byte 0 -- 0xFFFFFF00

    shift = shift + ((!!(x & mask1)) << 3); //check if something is in 3,2,1 -- if there is, we want shift to += 8
    shift = shift + ((!!(x & mask2)) << 3); //same as above for 3,2
    shift = shift + ((!!(x & mask3)) << 3); //same as above for 3
    
    //now shift holds byte location of most significant bit

    //step2: isolate most significant bit in the shifted byte
    // bring most significant byte down to byte 0 position and isolate it
    signif = (x >> shift) & 0xFF;
    
    //search each half of signif (like binary search) and cut
    //first hexit
    shift = shift + ((!!(signif & 0xF0)) << 2);
    signif = (x >> shift) & 0x0F;
    //first two bits of second hexit
    shift = shift + ((!!(signif & 0x0C)) << 1);
    signif = (x >> shift) & 0x3;
    //2nd bit
    shift = shift + (!!(signif & 0x2));
    signif = (x >> shift) & 0x1;
    
    return (signif << shift);
    
    
}
