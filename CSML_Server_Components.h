#ifndef _server_components_h_
#define _server_components_h_

#include <stdlib.h>
#include "dynamic_array.h"

typedef void (*CSML_RPtr)(char* c, void* state); // Response Pointer. request, secret_state.

typedef struct{
    CSML_RPtr func;
    int code;
} CSML_RCP; // Response Code Pair

DYN_ARR_DECL(CSML_RCP)
typedef dyn_arr_CSML_RCP CSML_RCPArray;

DYN_ARR_DECL(int)

typedef struct{
    int id;
    void* secret_state;
    CSML_RCPArray* responses;
    dyn_arr_int* fdarray;
} CSML_Lobby;

CSML_RCPArray* get_rcp(size_t initial_capacity);
void add_func(CSML_RCPArray*, CSML_RPtr, int);
void remove_func(CSML_RCPArray*, size_t);
void free_rcp(CSML_RCPArray*);

void call(CSML_RCPArray*, size_t, char*, void*);

void lobby_add_response(CSML_Lobby*, int, CSML_RPtr);
void lobby_respond(CSML_Lobby*, char*, int);
void free_lobby(CSML_Lobby*);

#endif
