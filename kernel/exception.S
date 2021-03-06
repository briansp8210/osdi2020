.macro exception_prologue
  sub sp, sp, 34 * 8
  stp x0, x1, [sp ,16 * 0]
  stp x2, x3, [sp ,16 * 1]
  stp x4, x5, [sp ,16 * 2]
  stp x6, x7, [sp ,16 * 3]
  stp x8, x9, [sp ,16 * 4]
  stp x10, x11, [sp ,16 * 5]
  stp x12, x13, [sp ,16 * 6]
  stp x14, x15, [sp ,16 * 7]
  stp x16, x17, [sp ,16 * 8]
  stp x18, x19, [sp ,16 * 9]
  stp x20, x21, [sp ,16 * 10]
  stp x22, x23, [sp ,16 * 11]
  stp x24, x25, [sp ,16 * 12]
  stp x26, x27, [sp ,16 * 13]
  stp x28, x29, [sp ,16 * 14]

  mrs x0, sp_el0
  mrs x1, elr_el1
  mrs x2, spsr_el1
  stp x30, x0, [sp, 16 * 15]
  stp x1, x2, [sp, 16 * 16]
.endm

.macro exception_epilogue
  ldp x30, x0, [sp, 16 * 15]
  ldp x1, x2, [sp, 16 * 16]
  msr sp_el0, x0
  msr elr_el1, x1
  msr spsr_el1, x2

  ldp x0, x1, [sp ,16 * 0]
  ldp x2, x3, [sp ,16 * 1]
  ldp x4, x5, [sp ,16 * 2]
  ldp x6, x7, [sp ,16 * 3]
  ldp x8, x9, [sp ,16 * 4]
  ldp x10, x11, [sp ,16 * 5]
  ldp x12, x13, [sp ,16 * 6]
  ldp x14, x15, [sp ,16 * 7]
  ldp x16, x17, [sp ,16 * 8]
  ldp x18, x19, [sp ,16 * 9]
  ldp x20, x21, [sp ,16 * 10]
  ldp x22, x23, [sp ,16 * 11]
  ldp x24, x25, [sp ,16 * 12]
  ldp x26, x27, [sp ,16 * 13]
  ldp x28, x29, [sp ,16 * 14]
  add sp, sp, 34 * 8
  eret
.endm

.macro vec_entry handler
  b \handler
  .align 7 /* entry size is 0x80, .align will pad 0 */
.endm

curr_el_spx_sync:
  exception_prologue
  bl curr_el_spx_sync_handler
  bl post_exception_hook
  exception_epilogue

curr_el_spx_irq:
  exception_prologue
  bl curr_el_spx_irq_handler
  bl post_exception_hook
  exception_epilogue

lower_el_aarch64_sync:
  exception_prologue
  bl is_syscall
//  cmp x0, #0
//  beq 0f
  cbz x0, 0f

//  bl enable_interrupt
  ldr x8, [sp, 16 * 4]
  ldp x0, x1, [sp, 16 * 0]
  ldp x2, x3, [sp, 16 * 1]
  ldp x4, x5, [sp, 16 * 2]
  adr x9, syscall_table
  ldr x9, [x9, x8, lsl #3]
  blr x9
  str x0, [sp, 16 * 0]
  b 1f
0:
  bl curr_el_spx_sync_handler
1:
  bl post_exception_hook
  exception_epilogue

lower_el_aarch64_irq:
  b curr_el_spx_irq

not_implemented:
  exception_prologue
  bl not_implemented_handler
  exception_epilogue

.align 11 /* vector table should be aligned to 0x800 */
.global vector_table
vector_table:
  vec_entry not_implemented
  vec_entry not_implemented
  vec_entry not_implemented
  vec_entry not_implemented

  vec_entry curr_el_spx_sync
  vec_entry curr_el_spx_irq
  vec_entry not_implemented
  vec_entry not_implemented

  vec_entry lower_el_aarch64_sync
  vec_entry lower_el_aarch64_irq
  vec_entry not_implemented
  vec_entry not_implemented

  vec_entry not_implemented
  vec_entry not_implemented
  vec_entry not_implemented
  vec_entry not_implemented

.global enable_interrupt
enable_interrupt:
  msr daifclr, #0xf
  ret

.global disable_interrupt
disable_interrupt:
  msr daifset, #0xf
  ret
