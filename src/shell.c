#include "mini_uart.h"
#include "shell.h"

int strcmp(const char *s1, const char *s2) {
  for (; *s1 != '\0' && *s1 == *s2; ++s1, ++s2);
  return *s1 - *s2;
}

uint32_t strlen(const char *s) {
  uint32_t len = 0;
  for (; s[len] != '\0'; ++len);
  return len;
}

int isspace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

char *strtrim(char *s) {
  char *begin = s;
  for (; isspace(*begin); ++begin);
  char *end = begin + strlen(begin) - 1;
  for (; end > begin && isspace(*end); --end);
  *(end + 1) = '\0';
  return begin;
}

char *uitos(uint64_t num, char *buf) {
  char *cur = buf;
  uint8_t len = 0;

  do {
    *cur = '0' + (num % 10);
    num /= 10;
    ++cur;
    ++len;
  } while (num > 0);
  *cur = '\0';

  for (int i = 0; i < len / 2; ++i) {
    char tmp = buf[len - i - 1];
    buf[len - i - 1] = buf[i];
    buf[i] = tmp;
  }

  return buf;
}

void help(void) {
  mini_uart_puts("hello: print Hello World!" CRLF);
  mini_uart_puts("help: help" CRLF);
  mini_uart_puts("reboot: reboot rpi3" CRLF);
  mini_uart_puts("timestamp: get current timestamp" CRLF);
}

void hello(void) {
  mini_uart_puts("Hello World!" CRLF);
}

void timestamp(void) {
  uint64_t count, frequency;
  asm("mrs %0, cntpct_el0" : "=r"(count));
  asm("mrs %0, cntfrq_el0" : "=r"(frequency));
  uint64_t time_int = count / frequency;
  uint64_t time_fra = (count * 1000000 / frequency) % 1000000;

  char buf[32];
  mini_uart_puts("[");
  mini_uart_puts(uitos(time_int, buf));
  mini_uart_puts(".");
  mini_uart_puts(uitos(time_fra, buf));
  mini_uart_puts("]" CRLF);
}

void reboot(void) {
  // full reset
  *PM_RSTC = PM_PASSWORD | 0x20;
  mini_uart_puts("Reboot..." CRLF);
  while (1);
}

void shell(void) {
  mini_uart_puts("               _ _       _          _ _ " CRLF);
  mini_uart_puts("  ___  ___  __| (_)  ___| |__   ___| | |" CRLF);
  mini_uart_puts(" / _ \\/ __|/ _` | | / __| '_ \\ / _ \\ | |" CRLF);
  mini_uart_puts("| (_) \\__ \\ (_| | | \\__ \\ | | |  __/ | |" CRLF);
  mini_uart_puts(" \\___/|___/\\__,_|_| |___/_| |_|\\___|_|_|" CRLF);
  mini_uart_puts(CRLF);

  while (1) {
    mini_uart_puts("# ");
    char buf[MAX_CMD_LEN];
    mini_uart_gets(buf);
    mini_uart_puts(CRLF);

    char *cmd = strtrim(buf);
    if (strlen(cmd) != 0) {
      if (!strcmp(cmd, "help")) {
        help();
      } else if (!strcmp(cmd, "hello")) {
        hello();
      } else if (!strcmp(cmd, "reboot")) {
        reboot();
      } else if (!strcmp(cmd, "timestamp")) {
        timestamp();
      } else {
        mini_uart_puts("Error: command ");
        mini_uart_puts(cmd);
        mini_uart_puts(" not found, try <help>" CRLF);
      }
    }
  }
}