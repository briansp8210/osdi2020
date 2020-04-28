.section .text
.global context_switch_helper
context_switch_helper:
  stp x19, x20, [x0, 16 * 0]
  stp x21, x22, [x0, 16 * 1]
  stp x23, x24, [x0, 16 * 2]
  stp x25, x26, [x0, 16 * 3]
  stp x27, x28, [x0, 16 * 4]
  stp fp, lr, [x0, 16 * 5]
  mov x9, sp
  str x9, [x0, 16 * 6]

  ldp x19, x20, [x1, 16 * 0]
  ldp x21, x22, [x1, 16 * 1]
  ldp x23, x24, [x1, 16 * 2]
  ldp x25, x26, [x1, 16 * 3]
  ldp x27, x28, [x1, 16 * 4]
  ldp fp, lr, [x1, 16 * 5]
  ldr x9, [x1, 16 * 6]
  mov sp,  x9
  msr tpidr_el1, x1
  ret

.global get_current_task
get_current_task:
  mrs x0, tpidr_el1
  ret

/*
 * This hook function will be invoked after a task gets scheduled
 * for the first time, and before it excutes its start function (x19).
 */
.global task_debut_hook
task_debut_hook:
  bl enable_interrupt
  blr x19

.global el1_to_el0
el1_to_el0:
  msr elr_el1, x0
  msr sp_el0, x1
  msr spsr_el1, xzr
  eret