#ifndef SOLUTION_H
#define SOLUTION_H
#include "spinlock_ece391.h"


typedef struct ps_enter_exit_lock {
  	// Fill this out!!!
  	int TA_num;       // The number of TA in lab
  	int STU_num;      // The number of student in lab
  	int PROF_num;     // The number of professor in lab

  	int PROF_wait;    // The number of professor waiting
  	int TA_wait;      // The number of TA waiting
  	spinlock_t *splock;   // The lock
} ps_lock;

ps_lock ps_lock_create(spinlock_t *lock);
void professor_enter(ps_lock *ps);
void professor_exit(ps_lock *ps);
void ta_enter(ps_lock *ps);
void ta_exit(ps_lock *ps);
void student_enter(ps_lock *ps);
void student_exit(ps_lock *ps);

#endif /* SOLUTION_H */
