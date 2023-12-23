
# mp1.S - missile-command version
# Good luck, and have fun!

.data
	
    # Constants for accessing the fields of a struct missile, 
    # struct missile is defined in rtc.h and mp1.h

	NEXT 	  = 0
	X	  = 4
	Y	  = 8
	VX	  = 12
	VY	  = 16
	DEST_X	  = 20
	DEST_Y	  = 24
	EXPLODED  = 28
	C	  = 32

    # Character to draw for an explosion - '@'

	EXPLOSION = 64
	OFFSETY = 160
	OFFSETBASE = 3840
	XSEVNIN = 79
	YTWEFOU = 24


# Data shared between this file and rtc.c helper functions

# This '.globl' directive makes these symbols visible externally
.globl mp1_missile_list, base_alive, mp1_score

mp1_missile_list:	.long 0x0	# Head of list pointer
base_alive:		.long 0x0	# Array of 3 bytes, plus a padding byte
mp1_score:		.long 0x0	# Player's current score


# Data private to this  file

base_pic:     	    .string "/^^^\\"	# Picture of a live base
dead_base_pic: 	    .string "xxxxx"	# Picture of a dead base
crosshairs_x:		.long 0x0	# X-position of the crosshairs
crosshairs_y:		.long 0x0	# Y-position of the crosshairs

	

.text

# void mp1_poke(void);
# You can use this function to write to video memory.
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %cl  - The byte you wish to write
#            %eax - Offset from the start of video memory that you wish
#                   to write to
#   Outputs: Text-mode video screen is written to at location %eax with
#            the byte in %cl
# Registers: Clobbers EDX

mp1_poke:
		movl    vmem_base_addr(,1),%edx
		movb    %cl,(%edx,%eax,1)
		ret

#   ----------------- Exported functions ---------------------

# void mp1_rtc_tasklet(unsigned long garbage);
# Performs three tasks:
#	(1) updates the list of missiles (implement this in update_missiles,
#           below, and call it from here).
#	(2) Redraw the bases - they may have been overwritten with missiles
#	(3) Redraw the crosshairs - it may have been overwritten with missiles
# Inputs   : none
# Outputs  : none
# Registers: Standard C calling convention

.globl mp1_rtc_tasklet
mp1_rtc_tasklet:
		pushl 	%ebp
    	movl 	%esp, %ebp 
		pushl	%eax		
		pushl 	%ecx  
		pushl   %edx
		call update_missiles
		popl 	%edx
		popl	%ecx
		popl	%eax

		pushl	%eax		
		pushl 	%ecx  
		pushl   %edx
		call Redraw_bases
		popl 	%edx
		popl	%ecx
		popl	%eax
		
		pushl	%eax		
		pushl 	%ecx  
		pushl   %edx
		call Redraw_crosshairs
		popl 	%edx
		popl	%ecx
		popl	%eax
		
		leave
		ret

# int mp1_ioctl(unsigned long arg, unsigned int cmd)
# The dispatch function for the MP1 ioctls - should use the cmd argument
# and a jumptable to execute one of the specific ioctls implemented below.
# Inputs   : unsigned long arg - parameter to the mp1_ioctl_....
#          : unsigned int cmd  - specifies which mp1_ioctl_... function 
#          :                     to execute
# Outputs  : Returns an integer - depends on which ioctl() function is called
# Registers: Standard C calling convention

.globl mp1_ioctl
mp1_ioctl:
		cmpl $0, 8(%esp)             # invalid when cmd < 0 or cmd > 4
		jl INVALID
		cmpl $4, 8(%esp)
		jg INVALID
		movl 8(%esp),%eax
		jmp *jump_table(,%eax,4)
INVALID: 
		movl $-1, %eax
		ret


# ----------------- Functions private to this file -------------------
# void update_missile()
# perform three tasks:
# 		1. traverse all the missiles and check whether it is the last one. If not, then update x and y
#		   then check whether is out of bound, if so remove.
#		2. check whether explodes or reach the destnation, then if finised exploding remove it.
#		3. if the missile not explodes, then it just fly in the map, just change it position.
#		no input and output
update_missiles:
		pushl 	%ebp
    	movl 	%esp, %ebp            
		pushl 	%ebx                  
		pushl 	%edi                  
		pushl 	%esi                  
		movl 	mp1_missile_list, %ebx 
		movl 	%ebx, %edi             
	missile_loop:
		cmpl 	$0, %ebx               	# check whether missile left
		je 		no_missile_left
	
		movb    $32, %cl				# just clean the missile right now
        movl    Y(%ebx), %esi
        sarl    $16, %esi
        imull   $OFFSETY, %esi, %eax
        movl    X(%ebx), %esi
        sarl    $16, %esi
        shll    $1, %esi
        addl    %esi, %eax
        pushl   %edx
        call    mp1_poke
        popl    %edx
		
		cmpl 	$0, EXPLODED(%ebx)     # check whether the misislee is exploding
		jne 	now_exploding              

		
		movl	X(%ebx), %ecx			# ecx is used for storing the value of X.
		movl	Y(%ebx), %eax			# eax is used for storing the value of Y.
		addl   	VX(%ebx), %ecx			# ecx is now used for the position-X of missile after updating.
		addl 	VY(%ebx), %eax			# eax is now used for the position-Y of missile after updating.
		movl	%ecx, X(%ebx)			# update the X-position of the current missile
		movl	%eax, Y(%ebx)			# update the Y-position of the current missile

        movl    X(%ebx), %ecx
        movl    Y(%ebx), %eax
		sarl	$16, %ecx				# x
        sarl    $16, %eax
        cmpl    $0, %ecx
		jl		remove_missile			# check whether x < 0
		cmpl	$XSEVNIN, %ecx
		jg		remove_missile			# check whether x > 79

		cmpl	$0, %eax
		jl		remove_missile			# check whether y < 0
		cmpl	$YTWEFOU, %eax
		jg		remove_missile			# check whether y > 24

		movl 	X(%ebx), %esi			# check whether reach the destination
		sarl 	$16, %esi
		cmpl 	DEST_X(%ebx), %esi     
		jne 	not_at_destination         
		movl 	Y(%ebx), %esi
		sarl 	$16, %esi
		cmpl 	DEST_Y(%ebx), %esi     
		je 		now_exploding  

	not_at_destination:

		movb 	C(%ebx), %cl			# if don't reach the destination, just draw it back
		movl 	Y(%ebx), %esi
		sarl 	$16, %esi				
		imull 	$OFFSETY, %esi, %eax      
		movl 	X(%ebx), %esi
		sarl 	$16, %esi				
		shll 	$1, %esi              
		addl 	%esi, %eax             
		pushl 	%edx                 
		call 	mp1_poke
		popl 	%edx

	next_missile:
		cmpl 	$0, %ebx				# if the next missile is zero, then it means no missile left
		je 		no_missile_left
		movl 	%ebx, %edi             
		movl 	NEXT(%ebx), %ebx      
		jmp 	missile_loop

	now_exploding:
		pushl 	%ebx                 
		call 	missile_explode       	# if the missile explodes
		addl 	$4, %esp
		cmpl 	$0, %eax             
		je 		no_chain_hit			# check if there is some chain hit
		pushl	%eax
		pushl	%ecx
		pushl	%edx
		call 	mp1_notify_user
		popl	%edx
		popl	%ecx
		popl	%eax
		

	no_chain_hit:
		subl 	$1, EXPLODED(%ebx)         	# if no chain hit, just decrease the exploded number
		movb 	$32, %cl               		# clean it
		movl 	Y(%ebx), %esi
		sarl 	$16, %esi
		imull 	$OFFSETY, %esi, %eax      
		movl 	X(%ebx), %esi
		sarl 	$16, %esi
		shll 	$1, %esi               
		addl 	%esi, %eax             
		pushl 	%edx                  
		call 	mp1_poke
		popl 	%edx

		cmpl 	$0, EXPLODED(%ebx)          # if finish exploding, remove it 
		jle 	remove_missile

		movb 	$64, %cl         			# redraw the exploding missile
		movl 	Y(%ebx), %esi
		sarl 	$16, %esi				
		imull 	$OFFSETY, %esi, %eax      
		movl 	X(%ebx), %esi
		sarl 	$16, %esi				
		shll 	$1, %esi               
		addl 	%esi, %eax             
		pushl 	%edx                  
		call 	mp1_poke
		popl 	%edx
		jmp 	next_missile
	remove_missile:							# the helper function to call the missile
		pushl 	%edi
		pushl 	%ebx                  
		call 	mp1_missile_remove
		addl 	$8, %esp
		jmp 	next_missile

	no_missile_left:						# no missile left
		popl 	%esi
		popl 	%edi
		popl 	%ebx
		movl 	%ebp, %esp
		popl 	%ebp
		ret

# this is the helper function to help me remove the missile
# Input: the current and the previous pointer to the missile

mp1_missile_remove:
		pushl 	%ebp
    	movl 	%esp, %ebp            
		pushl 	%edi                  
		pushl 	%esi   
		
		movl 	8(%ebp), %ebx          
		movl 	12(%ebp), %edi         
		cmpl 	mp1_missile_list, %ebx 			# two ways to remove the missile, one in the head two in the middile of missile list
		jne 	not_head           
		movl 	NEXT(%ebx), %esi      
		movl 	%esi, mp1_missile_list
		jmp 	freepart
not_head:
		movl 	NEXT(%ebx), %esi       
		movl 	%esi, NEXT(%edi)      
freepart:
		pushl 	%ebx                  
		call 	mp1_free               
		addl 	$4, %esp               
		movl 	%esi, %ebx             
		popl 	%esi
		popl 	%edi
		movl 	%ebp, %esp
		popl 	%ebp
		ret

# this is the function to draw three bases
# three bases are located on (18-22), (38-42), (58-62)
# no input and output
Redraw_bases:
    	pushl   %ebp        
    	movl    %esp, %ebp
		pushl	%ebx
    	pushl   %esi
		pushl	%edi
		movl 	$OFFSETBASE, %ebx						
		movl	$36, %esi						# 18 * 2 = 36
		movl	$0x00000000, %edx
		movl	base_alive, %edi
		andl	$0x00000001, %edi
		cmpl	$0, %edi
		jne		first_city_alive

	first_city_dead:
    	movl    %ebx, %eax						# calculate the position of the first city
    	addl    %esi, %eax
   	 	movb    dead_base_pic(,%edx,1), %cl		# calculate the correct position of the array, edx is used as an offset
		pushl   %edx
    	call    mp1_poke
    	popl    %edx

    	addl    $2, %esi						# move to the next charcter 
    	incl    %edx							# offset plus 1

    	cmpl    $5, %edx						# if we have checked all the 5 charcter, then jump to next base
    	jl      first_city_dead
		jmp		second_city

	first_city_alive:
    	movl    %ebx, %eax						# the same way to draw it as the alive situation
    	addl    %esi, %eax
   	 	movb    base_pic(,%edx,1), %cl

    	pushl   %edx
    	call    mp1_poke
    	popl    %edx

    	addl    $2, %esi
    	incl    %edx

    	cmpl    $5, %edx
    	jl      first_city_alive
	
	second_city:								# the same way as the first base
		movl 	$OFFSETBASE, %ebx
		movl	$76, %esi
		movl	$0x00000000, %edx
		movl	base_alive, %edi
		andl	$0x00000100, %edi
		cmpl	$0, %edi
		jne		second_city_alive

	second_city_dead:							# all the same...
    	movl    %ebx, %eax
    	addl    %esi, %eax
   	 	movb    dead_base_pic(,%edx,1), %cl

    	pushl   %edx
    	call    mp1_poke
    	popl    %edx

    	addl    $2, %esi
    	incl    %edx

    	cmpl    $5, %edx
    	jl      second_city_dead
		jmp		third_city


	second_city_alive:
    	movl    %ebx, %eax
    	addl    %esi, %eax
   	 	movb    base_pic(,%edx,1), %cl

    	pushl   %edx
    	call    mp1_poke
    	popl    %edx

    	addl    $2, %esi
    	incl    %edx

    	cmpl    $5, %edx
    	jl      second_city_alive

	third_city:									# the same way as the first and the second base
		movl 	$OFFSETBASE, %ebx
		movl	$116, %esi
		movl	$0x00000000, %edx
		movl	base_alive, %edi
		andl	$0x00010000, %edi
		cmpl	$0, %edi
		jne		third_city_alive

	third_city_dead:							# all the same...
    	movl    %ebx, %eax	
    	addl    %esi, %eax
   	 	movb    dead_base_pic(,%edx,1), %cl

    	pushl   %edx
    	call    mp1_poke
    	popl    %edx

    	addl    $2, %esi
    	incl    %edx

    	cmpl    $5, %edx
    	jl      third_city_dead
		jmp		Redraw_city_finished

	third_city_alive:
    	movl    %ebx, %eax
    	addl    %esi, %eax
   	 	movb    base_pic(,%edx,1), %cl

    	pushl   %edx
    	call    mp1_poke
    	popl    %edx

    	addl    $2, %esi
    	incl    %edx

    	cmpl    $5, %edx
    	jl      third_city_alive

	Redraw_city_finished:						# finish the bases drawing
		popl	%edi
		popl	%esi
		popl	%ebx
		leave
		ret
# this is the function to redraw the crosshair on the screen
Redraw_crosshairs:
		pushl	%ebp
		movl	%esp, %ebp
		pushl	%edi
		pushl	%ebx
		pushl	%esi

		movl	crosshairs_y, %eax				# find the location of the crosshair
		imull	$OFFSETY, %eax, %eax
		movl	%eax, %ebx
		
		movl	crosshairs_x, %eax
		imull	$2, %eax, %eax
		addl	%eax, %ebx
		movl	%ebx, %eax

		movb	$'+', %cl						# crosshair is a signal of '+'
		call	mp1_poke

		popl	%esi
		popl	%ebx
		popl	%edi
		leave
		ret
		
# the start game function
# this function help to start the game
# Input: one input but it should be ignored
# no outpus
mp1_ioctl_startgame:
		pushl 	%ebp
    	movl 	%esp, %ebp             			
		pushl 	%ebx
		pushl 	%esi

		movl	$0, mp1_missile_list
		movl	$0, mp1_score
		movl	$0x00010101, base_alive
		movl 	$40, crosshairs_x  				# crosshairs_x = 40
		movl 	$12, crosshairs_y  				# crosshairs_y = 12

		popl 	%esi
		popl 	%ebx
		movl 	%ebp, %esp             
		popl 	%ebp
		ret
# the function to add a missile
# this function will help add a missile from user space
# Input: a pointer to the struct of missile
# Output: 0(succeed) or -1(failed)
mp1_ioctl_addmissile:
		pushl 	%ebp
    	movl 	%esp, %ebp            
		pushl 	%ebx
		pushl 	%esi                  
		pushl 	%edi

		pushl	%ecx
		pushl	%edx
		pushl	$36
		call	mp1_malloc						# malloc a 36-bytes buffer space
		addl	$4, %esp
		popl	%edx
		popl	%ecx
		cmpl	$0x00000000, %eax				# check whether it is succeed
		jne		malloc_success

	malloc_failed:								# if it fails
		pushl	%ecx
		pushl	%edx
		movl	%eax, %ebx
		pushl	%ebx
		call	mp1_free						# free it and return -1
		addl	$4, %esp
		popl	%ecx
		popl	%edx
		movl	$-1, %eax
		jmp		addmissile_done


	malloc_success:								# if it succeeds
		movl	%eax, %ebx
		movl	8(%ebp), %esi
		pushl	%ecx
		pushl	%edx
		pushl	$36
		pushl	%esi
		pushl	%ebx
		call	mp1_copy_from_user				# then copy from user space
		addl	$12, %esp
		popl	%edx
		popl	%ecx
		cmpl	$0, %eax
		jne		copy_failed						# if copy failed
		movl	mp1_missile_list, %edi
		movl	%edi, NEXT(%ebx)				
		movl	%ebx, mp1_missile_list
		movl	$0, %eax						# if copy succeds, return 0
		jmp		addmissile_done

	copy_failed:
		pushl	%ebx							# free all the things
		call	mp1_free
		addl	$4, %esp
		movl	$-1, %eax

	addmissile_done:
		popl	%edi
		popl	%esi
		popl	%ebx
		
		leave
		ret

# the function to move xhairs
# this function is to help move the crosshair in the screen
# Input: a 32-bit integer (low 16 bits for x, high 16 bits for y)
# Output: never fail so always return 0
mp1_ioctl_movexhairs:
		pushl 	%ebp
    	movl 	%esp, %ebp             # stack frame
		pushl 	%ebx
		                 
		pushl 	%esi                  # x offset
		pushl 	%edi

		imull 	$OFFSETY, crosshairs_y, %eax
		imull 	$2, crosshairs_x, %ebx
		addl 	%ebx, %eax
		movb 	$32, %cl
		pushl 	%edx
		call 	mp1_poke
		popl 	%edx

		movl 	8(%ebp), %edi          
		sarl 	$16, %edi              
		movl 	8(%ebp), %esi
		shll 	$16, %esi
		sarl 	$16, %esi              # get the lower 16 bits for the x offset

		addl 	crosshairs_x, %esi     # esi <- x + offset
		addl 	crosshairs_y, %edi     # edi <- y + offset
		
		cmpl 	$0, %esi               # check the range of x
		jl 		x_smaller_0
		cmpl 	$XSEVNIN, %esi
		jg 		x_larger_79
		movl 	%esi, crosshairs_x
	checkfory:
		cmpl 	$0, %edi               # check the range of y
		jl 		y_smaller_0
		cmpl 	$YTWEFOU, %edi
		jg 		y_larger_24
		movl 	%edi, crosshairs_y
		jmp 	finalpart
	x_smaller_0:
		movl 	$0, crosshairs_x
		jmp 	checkfory
	x_larger_79:
		movl 	$XSEVNIN, crosshairs_x
		jmp 	checkfory
	y_smaller_0:
		movl 	$0, crosshairs_y
		jmp 	finalpart
	y_larger_24:
		movl 	$YTWEFOU, crosshairs_y
	finalpart:
		movb 	$'+', %cl
		movl 	crosshairs_x, %esi           
		shll 	$1, %esi
		imull 	$OFFSETY, crosshairs_y, %eax      
		addl 	%esi, %eax           
		pushl 	%edx
		call 	mp1_poke               
		popl 	%edx

		movl 	$0, %eax				# always return 0
		popl 	%edi
		popl 	%esi
		popl 	%ebx
		
		leave
		ret
# the function to change the status
# this function is to help change the status and update the score
# Input: a pointer to user status
# Output: 0(succeed) or -1(failed)
mp1_ioctl_getstatus:
		pushl 	%ebp
    	movl 	%esp, %ebp             # stack frame
		pushl 	%ebx
		
		pushl 	%esi
		pushl 	%edi

		movl	mp1_score, %esi		# mp1_score
		andl	$0x0000FFFF, %esi	# clear higher 16 bits
		
		cmpb	$0, base_alive
		je 		city2_check
		addl	$0x00010000, %esi	# bit 16

	city2_check:
		movl	$1, %eax
	
		cmpb	$0, base_alive(%eax)
		
		je		city3_check
		addl		$0x00020000, %esi

	city3_check:
		movl	$2, %eax
	
		cmpb	$0, base_alive(%eax)
		je		getstatus_done_base_alive
		addl		$0x00040000, %esi

	getstatus_done_base_alive:
		
		pushl	%esi
		movl 	%esp, %edi
		pushl	%ecx
		pushl	%edx
		pushl 	$4   
		pushl 	%edi 
		pushl	8(%ebp)
		call 	mp1_copy_to_user
		addl 	$12, %esp
		popl	%edx 
		popl	%ecx
		popl	%esi
		
		cmpl 	$0, %eax 
		je 	done_success
		

	failstatus:
		movl	$-1, %eax

	done_success:
		popl	%edi
		popl	%esi
		popl	%ebx

		leave
		ret
# the function to end the game
# this funciton is to help ending the game and free all the things
# Input: a unsigned long should be ignored
# Output: nothing
mp1_ioctl_endgame:
		pushl 	%ebp
    	movl 	%esp, %ebp             # stack frame
		pushl 	%ebx
		pushl 	%esi
		pushl 	%edi
		movl	mp1_missile_list, %esi	
	looptoend:
		cmpl	$0, %esi
		je		loopdone
		movl	%esi, %edi
		movl	NEXT(%esi), %esi		# store the next one
		pushl	%eax
		pushl	%ecx			
		pushl	%edx
		pushl	%edi
		call	mp1_free
		addl	$4, %esp
		popl	%edx
		popl	%ecx			
		popl	%eax

		jmp		looptoend
	loopdone:
		movl	$0, %eax
		popl	%edi
		popl	%esi
		popl	%ebx			
		leave
		ret

jump_table:
		.long mp1_ioctl_startgame, mp1_ioctl_addmissile, mp1_ioctl_movexhairs, mp1_ioctl_getstatus, mp1_ioctl_endgame
