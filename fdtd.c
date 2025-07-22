#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char *argv[]) {
  printf(argc > 1 ? "Hello, %s!\n" : "Hello, World!\n",
         argc > 1 ? argv[1] : "");
  printf("Hello, FDTD World!\n");
  return 0;
}
