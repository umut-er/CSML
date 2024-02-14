#include <stdio.h>
#include <stdlib.h>

#include "CSML_Server.h"

#define PORT "9034"

void connection_initializer(CSML_Server* serv, int client_fd){
    // Connect to a base server, I will maybe implement this natively.
    add_client_to_lobby(serv, client_fd, 0);
}

void response1000(char* c, void* state){
    printf("Hello, World\n");
}

int main(void){
    CSML_Server* exmple_server = initialize_server(
                                NULL, PORT,
                                3, 256, 1, 
                                &connection_initializer);

    char* sstate = "Hello, World\n";
    int lobby_id = add_lobby_to_server(exmple_server, sstate, 10, 10);
    // Add response1000 function as a response to a request with code 1000.
    lobby_add_response(&exmple_server->lobbies->array[0], 1000, &response1000);
    while(1){
        poll_server(exmple_server);
    }
    free_server(exmple_server);
    return 0;
}
