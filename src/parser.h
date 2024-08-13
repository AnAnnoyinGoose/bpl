#ifndef _PARSER_H
#define _PARSER_H

#include "_std_func_def_.h"
#include "funcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum InstrType {
  _IT_none,
  _IT_varDef,
  _IT_funcCall,
} _IT;

#define contains(a, b) strcmp((const char *)a, (const char *)b)
#define equalsAt(a, b, idx) (a[idx] == b)
_IT parseInstrType(const char *line) {
  if (equalsAt(line, '@', 0)) {
    return _IT_varDef;
  }
  if (contains(line, "call")) {
    return _IT_funcCall;
  }
  return _IT_none;
}

char **splitBySpaces(const char *str, int *n) {
  printf("Splitting by spaces %s\n", str);
  char *cpy = strdup(str);
  char *token = strtok(cpy, " ");

  char **ret = malloc(sizeof(char *) * 100);
  int i = 0;
  while (token != NULL) {
    ret[i] = token;
    token = strtok(NULL, " ");
    i++;
  }
  *n = i;

  return ret;
}

void freeTokens(char **tokens, int n) { free(tokens); }

_R *evalrhs(const char *line, _VS *vs, _FS *fs) {
  // add @^x @^y
  int n;
  char **tokens = splitBySpaces(line, &n);
  _FC *f = getFunction(fs, tokens[0]);
  if (f == NULL) {
    f = getStdFunc(tokens[0]);
    if (f == NULL) {
      printf("Function %s not found\n", tokens[0]);
      freeTokens(tokens, n);
      return createReturn(_VDT_void, 0, "evalrhs");
    }
  }
  printf("Evaluating function %s\n", f->name);
  _AL al = {0};
  int readOnly = 0;
  for (int i = 1; i < n; i++) {
    char *varName = tokens[i] + 1;
    if (strcmp(varName, "^") == 0) {
      readOnly = 1;
    }
    _V *v = getVariable(vs, varName + 1);
    if (v == NULL) {
      printf("Variable %s not found\n", varName);
      freeTokens(tokens, n);
      return createReturn(_VDT_void, 0, "evalrhs");
    }
    _A *a = createArg(v, f->nArgs, readOnly);
    addArg(&al, a);
  }
  if (f->func != NULL) {
    return f->func(&al);
  }

  freeTokens(tokens, n);
  return createReturn(_VDT_void, 0, "evalrhs");
}

void parserParseVarDef(const char *line, char **tokens, _VS *vs, _FS *fs) {

  printf("Parsing variable definition %s\n", line);
  _VDT t = getVariableDataType(tokens[1]);
  _V *v = createVariable(_VT_var, t, NULL, tokens[0] + 1);

  if (tokens[2][0] == '$') {
    // means to eval the right hand side
    size_t start = strchr(line, '$') - line + 2;
    size_t end = strlen(line) - start;
    char *rhs = (char *)malloc(end + 1);
    strncpy(rhs, line + start, end);
    rhs[end] = '\0'; // Null-terminate the string
    printf("Evaluating right hand side: %s\n", rhs);

    const _R *r = evalrhs(rhs, vs, fs);
    if (r->type == _VDT_uint32) {
      v->data = malloc(sizeof(uint32_t));
      memset(v->data, *(uint32_t *)r->data, sizeof(uint32_t));
    }
    if (r->type == _VDT_uint8) {
      v->data = malloc(sizeof(uint8_t));
      memset(v->data, *(uint8_t *)r->data, sizeof(uint8_t));
      printf("uint8 %d\n", *(uint8_t *)r->data);
    }
    if (r->type == _VDT_str) {
      v->data = malloc(sizeof(char));
      memset(v->data, *(char *)r->data, sizeof(char));
    }
    if (r->type == _VDT_bool) {
      v->data = malloc(sizeof(uint8_t));
      memset(v->data, *(uint8_t *)r->data, sizeof(uint8_t));
    }

    addVariable(vs, v);
    return;
  }

  switch (t) {
  case _VDT_uint8:
    v->data = malloc(sizeof(uint8_t));
    memset(v->data, atoi(tokens[2]), sizeof(uint8_t));
    break;
  case _VDT_uint32:
    v->data = malloc(sizeof(uint32_t));
    memset(v->data, atoi(tokens[2]), sizeof(uint32_t));
    break;
  case _VDT_str: {
    char *start = strchr(line, '"');
    char *end = strrchr(line, '"');

    if (start != NULL && end != NULL && start < end) {
      size_t length = end - start - 1;
      char *str_value = (char *)malloc(length + 1);
      strncpy(str_value, start + 1, length);
      str_value[length] = '\0'; // Null-terminate the string

      v->data = str_value;
    } else {
      assert(0 && "Invalid string format");
    }
    break;
  }

  case _VDT_bool:
    v->data = malloc(sizeof(uint8_t));
    *(uint8_t *)(v->data) = 0;
    break;
  default:
    assert(0 && "Invalid variable type");
    break;
  }
  addVariable(vs, v);
  printVariables(vs);
  return;
}

void parserParseFuncCall(const char *line, char **tokens, int n, _VS *vs,
                         _FS *fs) {

  printf("Parsing function call %s\n", line);
  _FC *f = getStdFunc(tokens[1]);
  if (f == NULL) {
    f = getFunction(fs, tokens[1]);
  }
  if (f == NULL) {
    printf("Function %s not found\n", tokens[1]);
    return;
  }
  if (f->nArgs != n - 2) {
    printf("Function %s expects %d arguments, got %d\n", tokens[1], f->nArgs,
           n - 2);
    return;
  }
  _AL *al = createArgList();
  for (int i = 0; i < f->nArgs; i++) {
    int readOnly = 0;
    char *arg = tokens[i + 2];
    arg = arg + 1;

    if (arg[0] == '^') {
      readOnly = 1;
      arg = arg + 1;
    }
    printf("Parsing argument %s\n", arg);

    _V *v = getVariable(vs, arg);

    if (v == NULL) {
      printf("Variable %s not found\n", arg);
      printVariables(vs);
      return;
    }
    if (f->args[i] != _VDT_any) {
      if (f->args[i] != v->dataType) {
        printf("Arg %s has type %d, expected %d\n", arg, v->dataType,
               f->args[i]);
        return;
      }
    }

    if (readOnly) {
      printf("Variable %s is read-only\n", arg);
    }
    _A *a = createArg(v, n - 2, readOnly);
    addArg(al, a);
  }

  _FCB c = f->func;
  _R ret = *c(al);

  if (ret.data != NULL) {
    printf("Return value: %d\n", *(uint32_t *)ret.data);
  }

  return;
}

void parse(const char *line, int *ret, _VS *vs, _FS *fs) {
  size_t len = strlen(line);
  int n;
  char **tokens = splitBySpaces(line, &n);
  _IT it = parseInstrType(line);
  if (it == _IT_varDef) {
    parserParseVarDef(line, tokens, vs, fs);
    freeTokens(tokens, n);
    return;
  } else if (it == _IT_funcCall) {
    parserParseFuncCall(line, tokens, n, vs, fs);
    freeTokens(tokens, n);
    return;
  }
}

#endif // !_P
