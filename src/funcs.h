#ifndef _FUNC_BPL_H
#define _FUNC_BPL_H

#include "variable.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for an argument
typedef struct Arg {
    _V *args;
    int readOnly;
    int nArgs;
} _A;

// Function to create an argument
_A *createArg(_V *v, int nArgs, int readOnly) {
    _A *a = malloc(sizeof(_A));
    if (a == NULL) {
        fprintf(stderr, "Memory allocation failed for _A\n");
        exit(EXIT_FAILURE);
    }
    a->args = v;  // Assuming _V is managed separately
    a->nArgs = nArgs;
    a->readOnly = readOnly;
    return a;
}

// Function to destroy an argument
void destroyArg(_A *a) {
    // Assuming that the memory for `a->args` is managed elsewhere and should not be freed here.
    free(a);
}

// Structure for a list of arguments
typedef struct ArgList {
    _A **args;
    int size;
} _AL;

// Function to create a list of arguments
_AL *createArgList() {
    _AL *al = malloc(sizeof(_AL));
    if (al == NULL) {
        fprintf(stderr, "Memory allocation failed for _AL\n");
        exit(EXIT_FAILURE);
    }
    al->args = NULL;
    al->size = 0;
    return al;
}

// Function to destroy a list of arguments
void destroyArgList(_AL *al) {
    for (int i = 0; i < al->size; i++) {
        destroyArg(al->args[i]);
    }
    free(al->args);
    free(al);
}

// Function to add an argument to the list
void addArg(_AL *al, _A *a) {
    printf("Adding arg %s\n", a->args->name);
    _A **new_args = realloc(al->args, (al->size + 1) * sizeof(_A *));
    if (new_args == NULL) {
        fprintf(stderr, "Memory allocation failed for adding an argument\n");
        exit(EXIT_FAILURE);
    }
    al->args = new_args;
    al->args[al->size] = a;
    al->size++;
}

// Structure for return type
typedef struct Return {
    _VDT type;
    int size;
    char *origin;
    void *data;
} _R;

// Function to create a return type
_R *createReturn(_VDT type, int size, char *origin) {
    _R *r = malloc(sizeof(_R));
    if (r == NULL) {
        fprintf(stderr, "Memory allocation failed for _R\n");
        exit(EXIT_FAILURE);
    }
    r->type = type;
    r->size = size;
    r->origin = strdup(origin);
    r->data = NULL;
    return r;
}

// Function to destroy a return type
void destroyReturn(_R *r) {
    if (r->data != NULL)
        free(r->data);
    if (r->origin != NULL)
        free(r->origin);
    free(r);
}

// Function to set data for return type
void setReturnData(_R *r, void *data) {
    if (r->data != NULL)
        free(r->data);
    r->data = data;
    printf("Setting return data %d\n", *(uint8_t *)r->data);
}

// Typedef for function callback
typedef _R *(*_FCB)(_AL *al);

// Structure for function call
typedef struct FunctionCall {
    char *name;
    _VDT args[100];
    int nArgs;
    int retSize;
    _VDT retType;
    int startIdx, endIdx;
    _FCB func;
} _FC;

// Structure for function stack
typedef struct FunctionStack {
    _FC *funcs;
    int size;
} _FS;

// Function to create a function stack
_FS *createFunctionStack() {
    printf("Creating function stack\n");
    _FS *fs = malloc(sizeof(_FS));
    if (fs == NULL) {
        fprintf(stderr, "Memory allocation failed for _FS\n");
        exit(EXIT_FAILURE);
    }
    fs->funcs = NULL;
    fs->size = 0;
    return fs;
}

// Function to destroy a function stack
void destroyFunctionStack(_FS *fs) {
    free(fs->funcs);
    free(fs);
}

// Function to add a function to the stack
void addFunction(_FS *fs, _FC *f) {
    printf("Adding function %s\n", f->name);
    _FC *new_funcs = realloc(fs->funcs, (fs->size + 1) * sizeof(_FC));
    if (new_funcs == NULL) {
        fprintf(stderr, "Memory allocation failed for adding a function\n");
        exit(EXIT_FAILURE);
    }
    fs->funcs = new_funcs;
    fs->funcs[fs->size] = *f;
    fs->size++;
}

// Function to get a function from the stack
_FC *getFunction(_FS *fs, const char *name) {
    for (int i = 0; i < fs->size; i++) {
        if (strcmp(fs->funcs[i].name, name) == 0) {
            return &fs->funcs[i];
        }
    }
    return NULL;
}

// Function to set a callback for a function
void setFunctionCall(_FC *f, _FCB c) {
    f->func = c;
}


#endif
