#ifndef _TSA_H
#define _TSA_H


#include "variable.h"
typedef struct TypedSizedArray {
  _V **data;
  int size;
  int maxSize;
  _VDT type;
} _TSA;

inline static _TSA *createTSA(_VDT type, int maxSize)  {
  _TSA *tsa = (_TSA *)malloc(sizeof(_TSA));
  tsa->data = (_V **) malloc(sizeof(_V *) * maxSize);
  tsa->size = 0;
  tsa->maxSize = maxSize;
  tsa->type = type;
  return tsa;
}

inline static void destroyTSA(_TSA *tsa) {
  free(tsa->data);
  free(tsa);
}

inline static void addVar_TSA(_TSA *tsa, _V *v) {
  if (tsa->size >= tsa->maxSize) {
    destroyTSA(tsa);
    assert(0 && "TSA is full");
  }
  tsa->data[tsa->size] = v;
  tsa->size++;
}

inline static _V *getVar_TSA(_TSA *tsa, int idx) {
  if (idx >= tsa->size) {
    return NULL;
  }
  return tsa->data[idx];
}

inline static _V *findVar_TSA(_TSA *tsa, const char *name) {
  for (int i = 0; i < tsa->size; i++) {
    if (strcmp(tsa->data[i]->name, name) == 0) {
      return tsa->data[i];
    }
  }
  return NULL;
}



#endif // !_TSA 
