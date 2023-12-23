/*
 * tab:2
 *
 * edit_dist_asm.S - inplementation of computing edit distance between two (linked) lists
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
 * Author:        Xiang Li
 * Version:       1
 * Creation Date: Sat Aug 20 2022
 * History:
 *    XL    1    Sat Aug 20 2022
 *        First written.
 */

.data
  UNUSED  = 0
  VAL   = 4
  NEXT = 8

.text
.global edit_dist_asm

# Edit Dist ASM (Assembly)
# Declaration is in edit_dist.h
#
# Registers:
#   EAX - Return Value
#   EBX - root1
#   ESI - root2
#   you may choose how to store your local/temp variables
#

edit_dist_asm:
  pushl %ebp
  movl %esp, %ebp

  # --- STUDENT CODE STARTS HERE ---
  # we use ecx, edx, edi as temp variables(insert, delete, replace)
  pushl %edi # save replace
  pushl %edx # save delete
  pushl %ecx # save insert

  pushl %ebx # save root1
  pushl %esi # save root2

  movl 8(%ebp), %ebx # get parameter
  movl 12(%ebp), %esi

  xorl %eax,%eax



  CMPL $0, %ebx # check if root1 is NULL
  JE CHECK_R2_1 # if root1 is NULL, jump to check R2 first
  JMP CHECK_R2_2 # if root1 is not NULL, jump to check R2 second

CHECK_R2_1:
  CMPL $0, %esi # check if root2 is NULL
  JE RETURN_0 # if root2 is NULL, return 0
  JMP RETURN_1_R2next # if root2 is not NULL, return 1+R2next

CHECK_R2_2:
  CMPL $0, %esi # check if root2 is NULL
  JE RETURN_1_R1next # if root2 is NULL, return 1+R1next
  JMP NEXT_BLOCK1 # if root2 is not NULL, jump to next_block1


NEXT_BLOCK1:
  movl VAL(%ebx), %ecx # get value of root1
  movl VAL(%esi), %edx # get value of root2
  CMPL %ecx, %edx # compare value of root1 and root2
  JE RETURN_R1NEXT_R2NEXT # if root1 and root2 are equal, return R1next_R2next
  JMP NEXT_BLOCK2 # if root1 and root2 are not equal, jump to next_block2


NEXT_BLOCK2:
  movl NEXT(%ebx), %ebx # get next of root1
  pushl %esi # push root2
  pushl %ebx # push root1
  call edit_dist_asm # call edit_dist_asm
  add $8, %esp # get back parameters
  movl %eax, %ecx # store return value in insert

  movl 8(%ebp), %ebx # get parameter
  movl 12(%ebp), %esi

  movl NEXT(%esi), %esi # get next of root2
  pushl %esi # push root2
  pushl %ebx # push root1
  call edit_dist_asm # call edit_dist_asm
  add $8, %esp # get back parameters
  movl %eax, %edx # store return value in delete

  movl 8(%ebp), %ebx # get parameter
  movl 12(%ebp), %esi

  movl NEXT(%ebx), %ebx # get next of root1
  movl NEXT(%esi), %esi # get next of root2
  pushl %esi # push root2
  pushl %ebx # push root1
  call edit_dist_asm # call edit_dist_asm
  add $8, %esp # get back parameters
  movl %eax, %edi # store return value in replace

  movl 8(%ebp), %ebx # get parameter
  movl 12(%ebp), %esi
  
  JMP NEXT_BLOCK3 # jump to next_block3


NEXT_BLOCK3:


  CMPL %edx, %ecx # compare insert and delete
  JBE CHECK_IR # if insert is smaller or equal to delete, jump to compare insert and replace
  JMP CHECK_DR # if insert is larger than delete, jump to compare delete and replace

CHECK_IR:
  CMPL %edi, %ecx # compare insert and replace
  JBE RETURN_1_I # if insert is smaller or equal to replace, return insert+1
  JMP CHECK_DR # if insert is larger than replace, jump to compare delete and replace

CHECK_DR:
  CMPL %edi, %edx # compare delete and replace
  JBE RETURN_1_D # if delete is smaller or equal to replace, return delete+1
  JMP RETURN_1_R # if delete is larger than replace, return replace+1




RETURN_0:
  xorl %eax, %eax # return 0
  JMP left

RETURN_1_R2next:
  movl NEXT(%esi), %esi # get next of root2
  pushl %esi # push root2
  pushl %ebx # push root1
  call edit_dist_asm # call edit_dist_asm
  add $8, %esp # get back parameters
  addl $1, %eax # add 1 to return value
  JMP left

RETURN_1_R1next:
  movl NEXT(%ebx), %ebx # get next of root1
  pushl %esi # push root2
  pushl %ebx # push root1
  call edit_dist_asm # call edit_dist_asm
  add $8, %esp # get back parameters
  addl $1, %eax # add 1 to return value
  JMP left

RETURN_R1NEXT_R2NEXT:
  movl NEXT(%ebx), %ebx # get next of root1
  movl NEXT(%esi), %esi # get next of root2
  pushl %esi # push root2
  pushl %ebx # push root1
  call edit_dist_asm # call edit_dist_asm
  add $8, %esp # get back parameters
  JMP left

RETURN_1_I:
  movl %ecx, %eax # return insert+1
  addl $1, %eax
  JMP left

RETURN_1_D:
  movl %edx, %eax # return delete+1
  addl $1, %eax
  JMP left


RETURN_1_R:
  movl %edi, %eax # return replace+1
  addl $1, %eax
  JMP left


left:
  popl %esi # restore root2
  popl %ebx # restore root1
  popl %ecx # restore insert
  popl %edx # restore delete
  popl %edi # restore replace
  # --- STUDENT CODE ENDS HERE ---
  leave
  ret

