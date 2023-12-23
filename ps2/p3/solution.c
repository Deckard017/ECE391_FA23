#include "spinlock_ece391.h"
#include "solution.h"
#include <bits/types.h>
#include <stdlib.h>


ps_lock ps_lock_create(spinlock_t *lock) {
  // Fill this out!!!
  	
	ps_lock new_lock;		// Malloc some space for two locks
	//new_lock->splock = (spinlock_t*) malloc(sizeof(spinlock_t));			
	new_lock.splock = lock;
	spinlock_init_ece391(new_lock.splock);									// Initialize the lock
	// pthread_mutex_unlock(&new_lock->splock.mutex);
	new_lock.PROF_num=0;										// Initialize all the contents in the lock
	new_lock.STU_num=0;
	new_lock.TA_num=0;
	new_lock.TA_wait=0;
	new_lock.PROF_wait=0;
	return new_lock;
}

void professor_enter(ps_lock *ps) {
  // Fill this out!!!
  	spinlock_lock_ece391(ps->splock);							// Before change the number of waiting professor, lock it
  	ps->PROF_wait++;											// Change the number of waiting professor
	spinlock_unlock_ece391(ps->splock);							// Unlock after change
  	while(1)
	{
		spinlock_lock_ece391(ps->splock);
		if(ps->STU_num==0 && ps->TA_num==0 && ps->PROF_num<20)	// To fit the condition
		{
			//spinlock_lock_ece391(ps->splock);
			ps->PROF_num++;										// Then change the number of professor in lab and waiting
			ps->PROF_wait--;
			spinlock_unlock_ece391(ps->splock);					// Unlock and return
			return;
		}
		spinlock_unlock_ece391(ps->splock);						// Unlock to make sure other procedure could change the content
	
  	}
}

void professor_exit(ps_lock *ps) 
{
  	// Fill this out!!!
  	spinlock_lock_ece391(ps->splock);
  	if(ps->PROF_num>0)											// exit the professor, no need to lock because there is no restriction of exiting the lab
	{
		//spinlock_lock_ece391(ps->splock);
  		ps->PROF_num--;
		//spinlock_unlock_ece391(ps->splock);
  	}
	spinlock_unlock_ece391(ps->splock);
  	
}

void ta_enter(ps_lock *ps) {
  	// Fill this out!!!
  	spinlock_lock_ece391(ps->splock);							// Before change the number of waiting TA, lock it
  	ps->TA_wait++;												// Change the number
   	spinlock_unlock_ece391(ps->splock);							// Unlock after change
  	while(1)
	{
		spinlock_lock_ece391(ps->splock);
	
		if(ps->STU_num+ps->TA_num<20 && ps->PROF_num==0 && ps->PROF_wait==0){ // To fit the conditions
			//spinlock_lock_ece391(ps->splock);
			ps->TA_num++;										// Change the number in lab and waiting
			ps->TA_wait--;
			spinlock_unlock_ece391(ps->splock);					// Unlock and return
			return;
		}
		spinlock_unlock_ece391(ps->splock);
	
  	}
}

void ta_exit(ps_lock *ps) {
  	// Fill this out!!!
  	spinlock_lock_ece391(ps->splock);
  	if(ps->TA_num>0)									// To exit a TA
	{
		//spinlock_lock_ece391(ps->splock);
  		ps->TA_num--;	
		//spinlock_unlock_ece391(ps->splock);								
  	}
  	spinlock_unlock_ece391(ps->splock);
}

void student_enter(ps_lock *ps) {
  // Fill this out!!!

  	while(1)
  	{
		spinlock_lock_ece391(ps->splock);				// Lock 
		if(ps->STU_num+ps->TA_num<20 && ps->PROF_num==0 && ps->PROF_wait==0 && ps->TA_wait==0)	// To fit the conditions
		{
			//spinlock_lock_ece391(ps->splock);				// Lock 
			ps->STU_num++;								// The number of student in the lab plus one
			spinlock_unlock_ece391(ps->splock);			// Unlock and return
			return;
		}
		spinlock_unlock_ece391(ps->splock);
	
  	}
}

void student_exit(ps_lock *ps) {
  	// Fill this out!!!
  	spinlock_lock_ece391(ps->splock);
  	if(ps->STU_num>0)									// To exit a student
	{
		// spinlock_lock_ece391(ps->splock);				// Lock 
  		ps->STU_num--;
		// spinlock_unlock_ece391(ps->splock);

  	}
  	spinlock_unlock_ece391(ps->splock);
}
