#include "../include/CSML_Server_Components.h"

#include <assert.h>
#include <stdio.h>

DYN_ARR_IMPL(CSML_RCP)
DYN_ARR_IMPL(int)

CSML_RCPArray* get_rcp(size_t initial_capacity){
    return get_array_CSML_RCP(initial_capacity);
}

void add_func(CSML_RCPArray* f, CSML_RPtr ptr, int code){
    CSML_RCP r;
    r.func = ptr; r.code = code;
    array_add_CSML_RCP(f, r);
}

void remove_func(CSML_RCPArray* f, size_t idx){
    array_remove_CSML_RCP(f, idx);
}

void free_rcp(CSML_RCPArray* f){
    free_array_CSML_RCP(f);
}

void call(CSML_RCPArray* f, size_t idx, char* c, void* d){
    (f->array[idx].func)(c, d);
}

void lobby_add_response(CSML_Lobby* lby, int code, CSML_RPtr response_func){
    add_func(lby->responses, response_func, code);
}

void lobby_respond(CSML_Lobby* lby, char* c, int code){
    for(size_t i = 0; i < lby->responses->size; i++){
        if(lby->responses->array[i].code == code)
            call(lby->responses, i, c, lby->secret_state);
    }
}

void free_lobby(CSML_Lobby* lby){
    free_rcp(lby->responses);
    free(lby->fdarray);
    free(lby);
}
