#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



const char* PROGRAM[] = {
  "@x uint8 9",
  "@y uint8 60",
  "@z uint8 $ add @^x @^y",
  "call putl @.str",
  "call putl @^z"
};



typedef enum VariableDataType {
  _VDT_uint8, _VDT_uint32, 
  _VDT_str,
  _VDT_bool,
  _VDT_custom
} _VDT;


typedef enum VariableType {
  _VT_var, _VT_const
} _VT;

// user defined type sturtucture
typedef struct UserDefinedType {
  char* name;
  int size;
} _UDT;

_UDT* createUDT(const char* name, int size) {
  _UDT* udt = malloc(sizeof(_UDT));
  udt->name = name;
  udt->size = size;
  return udt;
}
void destroyUDT(_UDT* udt) {
  free(udt);
}

typedef struct Variable {
  _VT type;
  _VDT dataType;
  _UDT* udt;
  const char* name;
  void* data;
} _V;

_V* createVariable(_VT type, _VDT dataType, _UDT* udt, const char* name) {
  printf("Creating variable %s\n", name); 
  _V* v = malloc(sizeof(_V));
  v->type = type;
  v->dataType = dataType;
  v->udt = udt;
  v->name = name;
  v->data = NULL;
  return v;
}
// cast to type 
#define setVariableData(v, data, asType) (v->data = malloc(asType); *(asType*)(v->data) = data)
#define getVariableData(v, type) (*(type*)(v->data))
void destroyVariable(_V* v) {
  if (v->data != NULL) free(v->data);
  free(v);
}
 
typedef struct VariableStack {
  _V** vars;
  int size;
} _VS;

_VS* createVariableStack() {
  printf("Creating variable stack\n");
  _VS* vs = malloc(sizeof(_VS));
  vs->vars = NULL;
  vs->size = 0;
  return vs;
}

void destroyVariableStack(_VS* vs) {
  for (int i = 0; i < vs->size; i++) {
    destroyVariable(vs->vars[i]);
  }
  free(vs->vars);
  free(vs);
}

void addVariable(_VS* vs, _V* v) {
  printf("Adding variable %s\n", v->name);
  if (vs->vars != NULL) {
    vs->vars = realloc(vs->vars, (vs->size + 1) * sizeof(_V*));
    vs->vars[vs->size] = v;
    vs->size++;
  }

}

_V* getVariable(_VS* vs, const char* name) {
  for (int i = 0; i < vs->size; i++) {
    if (strcmp(vs->vars[i]->name, name) == 0) {
      return vs->vars[i];
    }
  }
  return NULL;
}

int get_program_size(const char* program[]) {
  int n = 0;
  while (program[n] != NULL) n++;
  return n;
}


typedef enum InstrType {
  _IT_none,
  _IT_varDef
} _IT;


_IT parseInstrType(const char* line) {
  if (line[0] == '@') {
    return _IT_varDef;
  }


  return _IT_none;
}

_VDT getVariableDataType(const char* name) {
  if (name == NULL) assert(0 && "Variable name cannot be null");
  if (strcmp(name, "uint8") == 0) return _VDT_uint8;
  if (strcmp(name, "uint32") == 0) return _VDT_uint32;
  if (strcmp(name, "str") == 0) return _VDT_str;
  if (strcmp(name, "bool") == 0) return _VDT_bool;
  return _VDT_custom;
}

char** splitBySpaces(const char* str, int* n) {
  printf("Splitting by spaces %s\n", str);
  char *cpy = strdup(str);
  char* token = strtok(cpy, " ");

  char** ret = malloc(sizeof(char*) * 100);
  int i = 0;
  while (token != NULL) {
    ret[i] = token;
    token = strtok(NULL, " ");
    i++;
  }
  *n = i;

  return ret;
}

void freeTokens(char** tokens, int n) {
  free(tokens);
}

void parse(const char* line, int* ret, _VS* vs) {
  size_t len = strlen(line);
  
  _IT it = parseInstrType(line);
  if (it == _IT_varDef) {
    int n = 0;
    printf("Parsing variable definition %s\n", line);
    char** tokens = splitBySpaces(line, &n);
    _VDT t= getVariableDataType(tokens[1]);
    _V* v = createVariable(_VT_var, t, NULL, tokens[0]);
    addVariable(vs, v);
    freeTokens(tokens, n);
  }
}


_VS *vs = {0};
int main(void) {
  int size = get_program_size(PROGRAM);
  int *ret = 0;


  _VS* vs = createVariableStack();
  
  for (int i = 0; i < size; i++) {
    printf("%s\n", PROGRAM[i]);
    parse(PROGRAM[i], ret, vs);
  }

  return 0;
}
