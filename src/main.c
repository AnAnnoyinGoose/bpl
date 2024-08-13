#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "_std_func_def_.h"
#include "parser.h"





const char *PROGRAM[] = {"@x uint8 9",
                         "@y uint8 60",
                         "@z uint8 $ addu8 @^x @^y",
                         "@.str str \"Hello, World\"",
                         "call putl @.str",
                         "call putl @^z"};

int get_program_size(const char *program[]) {
  int n = 0;
  while (program[n] != NULL)
    n++;
  return n;
}

int main(void) {
  int size = get_program_size(PROGRAM);
  int *ret = 0;

  _VS *vs = createVariableStack();
  _FS *fs = createFunctionStack();

  for (int i = 0; i < size; i++) {
    printf("%s\n", PROGRAM[i]);
    parse(PROGRAM[i], ret, vs, fs);
    printf("\n");
  }

  return 0;
}
