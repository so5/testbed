#include <stdio.h>
int ruserok() {
  fprintf(stderr, "ruserok is called!\n");
  return 0;
}

void rcmd() {
  fprintf(stderr, "rcmd is called!\n");
}
