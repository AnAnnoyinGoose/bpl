#ifndef _STD_FUNC_DEF__H
#define _STD_FUNC_DEF__H

#include "funcs.h"
static _R *_std_clb_putl(_AL *al) {
  for (int i = 0; i < al->size; i++) {
    if (al->args[i]->args->dataType == _VDT_str) {
      printf("%s\n", (char *)al->args[i]->args->data);
    }
    if (al->args[i]->args->dataType == _VDT_uint8) {
      printf("%d\n", *(uint8_t *)al->args[i]->args->data);
    }
    if (al->args[i]->args->dataType == _VDT_uint32) {
      printf("%d\n", *(uint32_t *)al->args[i]->args->data);
    }
  }
  return createReturn(_VDT_void, 0, "putl");
}

static _R *_std_clb_add(_AL *al) {
  // can be either uint8 or uint32
  uint32_t ret = 0;
  _R *r = NULL;
  for (int i = 0; i < al->size; i++) {
    if (al->args[i]->args->dataType == _VDT_uint8) {
      ret += *(uint8_t *)al->args[i]->args->data;
    }
    if (al->args[i]->args->dataType == _VDT_uint32) {
      ret += *(uint32_t *)al->args[i]->args->data;
    }
    printf("%d\n", ret);
  }
  if (al->args[0]->args->dataType == _VDT_uint8) {
    r = createReturn(_VDT_uint8, 0, "addu8");
  } else if (al->args[0]->args->dataType == _VDT_uint32) {
    r = createReturn(_VDT_uint32, 0, "addu32");
  }

  setReturnData(r, &ret);
  printf("Returning %d\n", *(int *)r->data);
  return r;
}

static _FC STD_FUNCS[] = {
    {"putl", {_VDT_any}, 1, 0, _VDT_void, 0, 0, _std_clb_putl},
    {"addu8", {_VDT_uint8, _VDT_uint8}, 2, 0, _VDT_uint8, 0, 0, _std_clb_add},
    {"addu32",
     {_VDT_uint32, _VDT_uint32},
     2,
     0,
     _VDT_uint32,
     0,
     0,
     _std_clb_add},
};
static _FC *getStdFunc(const char *name) {
  for (int i = 0; i < sizeof(STD_FUNCS) / sizeof(_FC); i++)
    if (strcmp(name, STD_FUNCS[i].name) == 0)
      return &STD_FUNCS[i];
  return NULL;
}


  

#endif // !_ 
