#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *PROGRAM[] = {
    "@x uint8 9",          "@y uint8 60",     "@z uint8 $ add @^x @^y",
    "@.str str \"hello\"", "call putl @.str", "call putl @^z"};

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
  char *name;
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
// cast to type
#define setVariableData(v, data, asType)                                       \
  (v->data = malloc(asType); *(asType *)(v->data) = data)
#define getVariableData(v, type) (*(type *)(v->data))
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

int get_program_size(const char *program[]) {
  int n = 0;
  while (program[n] != NULL)
    n++;
  return n;
}

typedef enum InstrType {
  _IT_none,
  _IT_varDef,
  _IT_funcCall,
} _IT;

#define contains(a, b) strcmp((const char *)a, (const char *)b)
// gets a[idx] and checks if its eaqual with b
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

typedef struct Arg {
  _V *args;
  int readOnly;
  int nArgs;
} _A;

_A *createArg(_V *v, int nArgs, int readOnly) {
  _A *a = malloc(sizeof(_A));
  a->args = v;
  a->nArgs = nArgs;
  a->readOnly = readOnly;
  return a;
}

void destroyArg(_A *a) {
  free(a->args);
  free(a);
}

typedef struct ArgList {
  _A **args;
  int size;
} _AL;

_AL *createArgList() {
  _AL *al = malloc(sizeof(_AL));
  al->args = malloc(sizeof(_A *));
  al->size = 0;
  return al;
}

void destroyArgList(_AL *al) {
  for (int i = 0; i < al->size; i++) {
    destroyArg(al->args[i]);
  }
  free(al->args);
  free(al);
}

void addArg(_AL *al, _A *a) {
  printf("Adding arg %s\n", a->args->name);
  al->args = realloc(al->args, (al->size + 1) * sizeof(_A *));
  al->args[al->size] = a;
  al->size++;
}

typedef struct Return {
  _VDT type;
  int size;
  char *origin;
  void *data;
} _R;

_R *createReturn(_VDT type, int size, char *origin) {
  _R *r = malloc(sizeof(_R));
  r->type = type;
  r->size = size;
  r->origin = origin;
  r->data = NULL;
  return r;
}

void destroyReturn(_R *r) {
  if (r->data != NULL)
    free(r->data);
  free(r);
}

void setReturnData(_R *r, void *data) {
  r->data = malloc(r->size);
  memcpy(r->data, data, r->size);
}

typedef _R (*_FCB)(_AL *al);

typedef struct FunctionCall {
  char *name;
  _VDT args[100];
  int nArgs;
  int retSize;
  _VDT retType;
  int startIdx, endIdx;
  _FCB func;
} _FC;




static _R _std_clb_putl(_AL *al) {
  
  // check the arg dataType
  for (int i = 0; i < al->size; i++) {
    if (al->args[i]->args->dataType == _VDT_void) {
      return *createReturn(_VDT_void, 0, "putl");
    }
    if (al->args[i]->args->dataType == _VDT_str) {
      printf("%s\n", (char *)al->args[i]->args->data);
      return *createReturn(_VDT_void, 0, "putl");
    }
    if (al->args[i]->args->dataType == _VDT_uint8) {
      printf("%d\n", *(uint8_t *)al->args[i]->args->data);
      return *createReturn(_VDT_void, 0, "putl");
    }
    if (al->args[i]->args->dataType == _VDT_uint32) {
      printf("%d\n", *(uint32_t *)al->args[i]->args->data);
      return *createReturn(_VDT_void, 0, "putl");
    }
  }
  return *createReturn(_VDT_void, 0, "putl");  
}



static _FC STD_FUNCS[] = {
    {"putl", {_VDT_any}, 1, 0, _VDT_void, 0, 0, &_std_clb_putl},
};
static _FC *getStdFunc(const char *name) {
  for (int i = 0; i < sizeof(STD_FUNCS) / sizeof(_FC); i++)
    if (strcmp(name, STD_FUNCS[i].name) == 0)
      return &STD_FUNCS[i];
  return NULL;
}
typedef struct FunctionStack {
  _FC *funcs;
  int size;
} _FS;

_FS *createFunctionStack() {
  printf("Creating function stack\n");
  _FS *fs = malloc(sizeof(_FS));
  fs->funcs = NULL;
  fs->size = 0;
  return fs;
}

void destroyFunctionStack(_FS *fs) {
  free(fs->funcs);
  free(fs);
}

void addFunction(_FS *fs, _FC *f) {
  printf("Adding function %s\n", f->name);
  if (fs->funcs != NULL) {
    fs->funcs = realloc(fs->funcs, (fs->size + 1) * sizeof(_FC));
    fs->funcs[fs->size] = *f;
    fs->size++;
  }
}

_FC *getFunction(_FS *fs, const char *name) {
  for (int i = 0; i < fs->size; i++) {
    if (strcmp(fs->funcs[i].name, name) == 0) {
      return &fs->funcs[i];
    }
  }
  return NULL;
}

void setFunctionCall(_FC *f, _FCB c) { f->func = c; }

void parse(const char *line, int *ret, _VS *vs, _FS *fs) {
  size_t len = strlen(line);
  int n;
  char **tokens = splitBySpaces(line, &n);
  _IT it = parseInstrType(line);
  if (it == _IT_varDef) {
    printf("Parsing variable definition %s\n", line);
    _VDT t = getVariableDataType(tokens[1]);
    _V *v = createVariable(_VT_var, t, NULL, tokens[0] + 1);
    switch (t) {
    case _VDT_uint8:
      v->data = malloc(sizeof(uint8_t));
      memset(v->data, atoi(tokens[2]), sizeof(uint8_t));
      break;
    case _VDT_uint32:
      v->data = malloc(sizeof(uint32_t));
      memset(v->data, atoi(tokens[2]), sizeof(uint32_t));
      break;
    case _VDT_str:
      v->data = malloc(sizeof(char) * (len - 2));
      strcpy(v->data, tokens[2]);
      break;
    case _VDT_bool:
      v->data = malloc(sizeof(uint8_t));
      *(uint8_t *)(v->data) = 0;
      break;
    default:
      printf("Unknown data type %s\n", tokens[1]);
      break;
    }
    addVariable(vs, v);
    printVariables(vs);
    freeTokens(tokens, n);
    return;
  } else if (it == _IT_funcCall) {
    printf("Parsing function call %s\n", line);
    char **tokens = splitBySpaces(line, &n);
    _FC *f = getStdFunc(tokens[1]);
    if (f == NULL) {
      f = getFunction(fs, tokens[1]);
    }
    if (f == NULL) {
      printf("Function %s not found\n", tokens[1]);
      freeTokens(tokens, n);
      return;
    }
    if (f->nArgs != n - 2) {
      printf("Function %s expects %d arguments, got %d\n", tokens[1], f->nArgs,
             n - 2);
      freeTokens(tokens, n);
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
        freeTokens(tokens, n);
        return;
      }
      if (f->args[i] != _VDT_any) {
        if (f->args[i] != v->dataType) {
          printf("Variable %s has type %d, expected %d\n", arg, v->dataType,
                 f->args[i]);
          freeTokens(tokens, n);
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
    _R ret = c(al);

    if (ret.data != NULL) {
      printf("Return value: %d\n", *(uint32_t *)ret.data);
    }



    freeTokens(tokens, n);
    return;
  }
  freeTokens(tokens, n);
  // call putl @^z
  return;
}

_VS *vs = {0};
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
