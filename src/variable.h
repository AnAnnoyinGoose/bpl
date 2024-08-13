#ifndef _VAR_BPL_H
#define _VAR_BPL_H
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum VariableDataType {
  _VDT_uint8,
  _VDT_uint32,
  _VDT_str,
  _VDT_bool,
  _VDT_custom,
  _VDT_void,
  _VDT_any,
} _VDT;

typedef enum VariableType { _VT_var, _VT_const } _VT;

// user defined type sturtucture
typedef struct UserDefinedType {
  const char *name;
  int size;
} _UDT;

_UDT *createUDT(const char *name, int size) {
  _UDT *udt = malloc(sizeof(_UDT));
  udt->name = name;
  udt->size = size;
  return udt;
}
void destroyUDT(_UDT *udt) { free(udt); }

typedef struct Variable {
  _VT type;
  _VDT dataType;
  _UDT *udt;
  const char *name;
  void *data;
} _V;

_V *createVariable(_VT type, _VDT dataType, _UDT *udt, const char *name) {
  printf("Creating variable %s\n", name);
  _V *v = malloc(sizeof(_V));
  v->type = type;
  v->dataType = dataType;
  v->udt = udt;
  v->name = name;
  v->data = NULL;
  return v;
}

void destroyVariable(_V *v) {
  if (v->data != NULL)
    free(v->data);
  free(v);
}

typedef struct VariableStack {
  _V **vars;
  int size;
} _VS;

_VS *createVariableStack() {
  printf("Creating variable stack\n");
  _VS *vs = malloc(sizeof(_VS));
  vs->vars = NULL;
  vs->size = 0;
  return vs;
}

void destroyVariableStack(_VS *vs) {
  for (int i = 0; i < vs->size; i++) {
    destroyVariable(vs->vars[i]);
  }
  free(vs->vars);
  free(vs);
}

void addVariable(_VS *vs, _V *v) {
  printf("Adding variable %s\n", v->name);
  if (vs->vars != NULL) {
    vs->vars = realloc(vs->vars, (vs->size + 1) * sizeof(_V *));
    vs->vars[vs->size] = v;
    vs->size++;
  } else {
    vs->vars = malloc(sizeof(_V *));
    vs->vars[0] = v;
    vs->size = 1;
  }
}

_V *getVariable(_VS *vs, const char *name) {
  for (int i = 0; i < vs->size; i++) {
    if (strcmp(vs->vars[i]->name, name) == 0) {
      return vs->vars[i];
    }
  }
  return NULL;
}
void printVariables(_VS *vs) {
  printf("Printing variables %d\n", vs->size);
  for (int i = 0; i < vs->size; i++) {
    if (vs->vars[i]->data != NULL) {
      if (vs->vars[i]->dataType == _VDT_uint8)
        printf("\t%s: %d\n", vs->vars[i]->name, *(int *)vs->vars[i]->data);
      else if (vs->vars[i]->dataType == _VDT_uint32)
        printf("\t%s: %d\n", vs->vars[i]->name, *(int *)vs->vars[i]->data);
      else if (vs->vars[i]->dataType == _VDT_str)
        printf("\t%s: %s\n", vs->vars[i]->name, vs->vars[i]->data);
      else if (vs->vars[i]->dataType == _VDT_bool)
        printf("\t%s: %d\n", vs->vars[i]->name,
               *(uint8_t *)vs->vars[i]->data ? 1 : 0);
    } else
      printf("\t%s: NULL\n", vs->vars[i]->name);
  }
}

_VDT getVariableDataType(const char *name) {
  if (name == NULL)
    assert(0 && "Variable name cannot be null");
  if (strcmp(name, "uint8") == 0)
    return _VDT_uint8;
  if (strcmp(name, "uint32") == 0)
    return _VDT_uint32;
  if (strcmp(name, "str") == 0)
    return _VDT_str;
  if (strcmp(name, "bool") == 0)
    return _VDT_bool;
  return _VDT_custom;
}


#endif // !_VAR_BPL_H
