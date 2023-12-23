/*
 * tab:2
 *
 * mystery.c
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:        Yan Miao
 * Version:       1
 * Creation Date: Sun Aug 29 2021
 * Author:        Xiang Li
 * Version:       2
 * Modified Date: Sun Aug 21 2022
 * History:
 *    YM    1    Sun Aug 29 2021
 *    XL    2    Sun Aug 21 2022
 */

#include "mystery.h"

uint32_t mystery_c(uint32_t x, uint32_t y) {
  //------- YOUR CODE HERE -------
	if(y>24)										//First judgement to check whether y less than 24.
	{
		return 0;									//If so, return 0.
	}
	if(x>=42)										//Second judgement to check whether x less than 42.
	{
		return 0;									//If so, return 0.
	}
	uint32_t EDI=0, ESI=1;									//Int EDI to store first part of final answer.
	for(;ESI<=x;ESI++) 			//Calculate the sum from 1 to x.
	{
		EDI+=ESI;
	}
	uint32_t EDX=1;									//Int EDX to store second part of final answer.
	ESI=1;
	for(;ESI<=y;ESI++)			//Calculate the product from 1 to y.
	{
		EDX*=ESI;
	}
	EDX=EDX | EDI;
	return EDX;								//Calculate the final answer and return.
  //------------------------------
}
