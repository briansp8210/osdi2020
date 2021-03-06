#include "kernel/sched.h"
#include "kernel/lock.h"

#include "kernel/syscall.h"
#include "kernel/lib/ioutil.h"

void mutex_init(struct mutex *lock) {
  lock->lock = 0;
}

void mutex_lock(struct mutex *lock) {
  while (true) {
    preempt_disable();
    if (lock->lock == 0) {
      lock->lock = 1;
      preempt_enable();
      break;
    } else {
      preempt_enable();
      schedule();
    }
  }
}

void mutex_unlock(struct mutex *lock) {
  preempt_disable();
  lock->lock = 0;
  preempt_enable();
}
