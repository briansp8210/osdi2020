#include "kernel/exception.h"
#include "kernel/lib/ioutil.h"
#include "kernel/mini_uart.h"
#include "kernel/mm.h"
#include "kernel/sched.h"
#include "kernel/shell.h"
#include "kernel/syscall.h"
#include "kernel/lib/types.h"
#include "kernel/vfs.h"
#include "kernel/tmpfs.h"

#define assert(cond)                                                    \
  do {                                                                  \
    if (!(cond)) {                                                      \
      printk("Assertion Fail: %s:%u: %s\n", __FILE__, __LINE__, #cond); \
      for (;;) {}                                                       \
    }                                                                   \
  } while (0)

void delay(int t) {
  for (int i = 0; i < t; ++i) {}
}

void reaper(void) {
  while (true) {
    for (int i = 0; i < MAX_TASK_NUM; ++i) {
      if (task_inuse[i] == true && task_pool[i].state == TASK_ZOMBIE) {
        task_inuse[i] = false;
        printk("Task %u is reaped\n", i);
      }
    }
    schedule();
  }
}

void idle(void) {
  while (true) {
    schedule();
  }
}

extern char _binary_user_shell_img_start[];
extern char _binary_user_shell_img_size[];

void user_test(void) {
  do_exec((uint64_t)_binary_user_shell_img_start, (size_t)_binary_user_shell_img_size);
}

void print1(void) {
  while (true) {
    printk("1..." EOL);
    delay(1000000);
    schedule();
  }
}

void print2(void) {
  while (true) {
    printk("2..." EOL);
    delay(1000000);
    schedule();
  }
}

int main(void) {
  page_init();
  gpio_init();
  mini_uart_init();
  exception_init();
  asm("msr daifset, #0xf");
  core_timer_enable();

  char buf[128];
  mini_uart_getn(true, buf, 2);
  buddy_init();

  idle_task_create();
  privilege_task_create(reaper);
  privilege_task_create(user_test);

  idle();
}
