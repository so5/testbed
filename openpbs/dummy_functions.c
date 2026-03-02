#include <stdio.h>
#include <unistd.h>
#include <string.h>

void safe_log(const char *msg) {
  write(STDERR_FILENO, msg, strlen(msg));
}

int ruserok(const char *rhost, int superuser, const char *ruser, const char *luser) {
  safe_log("DEBUG: ruserok called\n");
  return 0;
}

void rcmd(char **ahost, unsigned short inport, const char *locuser, const char *remuser, const char *cmd, int *fd2p) {
  safe_log("DEBUG: rcmd called\n");
}

// backtrace implementation for Alpine
int backtrace(void** buffer, int size) {
  return 0;
}

char** backtrace_symbols(void* const* buffer, int size) {
  return NULL;
}

void backtrace_symbols_fd(void* const* buffer, int size, int fd) {
}
