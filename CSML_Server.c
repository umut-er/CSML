// Thanks Beej! Base code sourced from the great network guide! Check it out: https://beej.us/guide/bgnet/pdf/bgnet_usl_c_1.pdf

#include "CSML_Server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

DYN_ARR_IMPL(_sclient)
DYN_ARR_IMPL(pollfd)
DYN_ARR_IMPL(CSML_Lobby)

CSML_Server* initialize_server(char* host_name, char* serv_name,
                        size_t client_capacity, size_t buffer_size, 
                        size_t lobby_capacity, 
                        on_connect_func connection_initializer){
    assert(client_capacity > 0 && buffer_size > 0);
    CSML_Server* serv = malloc(sizeof (CSML_Server));
    serv->cur_lobby_id = 0;
    serv->connection_initializer = connection_initializer;

    serv->connected_clients = get_array__sclient(client_capacity);
    serv->pfds = get_array_pollfd(client_capacity+1);
    serv->lobbies = get_array_CSML_Lobby(lobby_capacity);

    serv->buffer_size = buffer_size;
    serv->buffer = malloc(serv->buffer_size * sizeof (char));

    serv->host_name = host_name;
    serv->serv_name = serv_name;
    serv->base_server_fd = _get_listener_socket(host_name, serv_name);
    assert(serv->base_server_fd != -1);

    add_to_pfds(serv, serv->base_server_fd);
    return serv;
}

void poll_server(CSML_Server* serv){
    int poll_count = poll(serv->pfds->array, serv->pfds->size, -1);
    assert(poll_count != -1);

    if (serv->pfds->array[0].revents & POLLIN){
        _sclient new_client;
        new_client.addr_size = sizeof (new_client.remote_addr);
        new_client.fd = accept(serv->base_server_fd,
            (struct sockaddr*) &(new_client.remote_addr),
            &new_client.addr_size);
        assert(new_client.fd != -1);

        add_to_pfds(serv, new_client.fd);
        add_sclient(serv, new_client);

        char remote_IP[INET6_ADDRSTRLEN];
        printf("pollserver: new connection from %s on "
            "socket %d\n",
            inet_ntop(new_client.remote_addr.ss_family,
                _get_in_addr((struct sockaddr*)&new_client.remote_addr),
                remote_IP, INET6_ADDRSTRLEN),
                new_client.fd);

        serv->connection_initializer(serv, new_client.fd);
    } 

    // Run through the existing connections looking for data to read
    for(int i = 1; i < serv->pfds->size; i++){
        // Check if someone's ready to read
        if(!(serv->pfds->array[i].revents & POLLIN))
            continue;

        int nbytes = recv(serv->pfds->array[i].fd, serv->buffer, serv->buffer_size, 0);
        int sender_fd = serv->pfds->array[i].fd;

        assert(nbytes >= 0);

        if (nbytes == 0){ // Connection closed
                printf("pollserver: socket %d hung up\n", sender_fd);

            close(serv->pfds->array[i].fd);
            del_from_pfds(serv, i);
            remove_sclient(serv, i-1);
            continue;
        }
        
        size_t buffer_str_size = strlen(serv->buffer);
        char* str;
        int code = -1;
        for(size_t i = 0; i < buffer_str_size; i++){
            if(serv->buffer[i] == ' '){
                str = (char*)malloc((i+1) * sizeof (char));
                strncpy(str, serv->buffer, i+1);
                str[i] = '\0';
                code = atoi(str);
                break;
            }
        }
        if(code == -1){
            str = (char*)malloc((buffer_str_size+1) * sizeof (char));
            strncpy(str, serv->buffer, buffer_str_size+1);
            str[buffer_str_size] = '\0';
            code = atoi(str);
        }

        for(size_t i = 0; i < serv->lobbies->size; i++){
            if(is_client_in_lobby(&serv->lobbies->array[i], sender_fd)){
                // printf("RECEIVED CODE: %d\n", code);
                lobby_respond(&serv->lobbies->array[i], serv->buffer, code);
            }
        }

        free(str);
    }
}

int add_lobby_to_server(CSML_Server* serv, void* secret_state, 
                        size_t client_count_hard_cap, // not yet tho, to come.
                        size_t response_cap){
    CSML_Lobby lby;
    lby.id = serv->cur_lobby_id;
    serv->cur_lobby_id++;
    lby.secret_state = secret_state;
    lby.responses = get_rcp(response_cap);
    lby.fdarray = get_array_int(client_count_hard_cap);
    array_add_CSML_Lobby(serv->lobbies, lby);

    return lby.id;
}

int add_premade_lobby_to_server(CSML_Server* serv, CSML_Lobby lby){
    lby.id = serv->cur_lobby_id;
    serv->cur_lobby_id++;
    array_add_CSML_Lobby(serv->lobbies, lby);
    return lby.id;
}

// TODO: Don't know if this is complete.
void remove_lobby_from_server(CSML_Server* serv, int lobby_id){
    int lobby_index = get_lobby_index(serv, lobby_id);
    assert(lobby_index != -1);

    array_remove_CSML_Lobby(serv->lobbies, lobby_index);
}

void add_client_to_lobby(CSML_Server* serv, int client_fd, int lobby_id){
    int lobby_index = get_lobby_index(serv, lobby_id);
    assert(lobby_index != -1);

    // Do the capacity check here.
    array_add_int(serv->lobbies->array[lobby_index].fdarray, client_fd);
}

void remove_client_from_lobby(CSML_Server* serv, int client_fd, int lobby_id){
    int lobby_index = get_lobby_index(serv, lobby_id);
    assert(lobby_index != -1);

    int client_index;
    for(client_index = 0; client_index < serv->lobbies->array[lobby_index].fdarray->size; client_index++){
        if(serv->lobbies->array[lobby_index].fdarray->array[client_index] == client_fd){
            break;
        }
    }

    array_remove_int(serv->lobbies->array[lobby_index].fdarray, client_index);
}

bool is_client_in_lobby(CSML_Lobby* lby, int client_fd){
    for(size_t i = 0; i < lby->fdarray->size; i++){
        if(lby->fdarray->array[i] == client_fd)
            return true;
    }
    return false;
}

void lobby_send_to_clients(CSML_Lobby* lby, int sender, bool send_to_sender, 
                            void* msg, size_t msg_size){
    for(size_t i = 0; i < lby->fdarray->size; i++){
        if(lby->fdarray->array[i] == sender && !send_to_sender)
            continue;
        send(lby->fdarray->array[i], msg, msg_size, 0);
    }
}

int get_lobby_index(CSML_Server* serv, int lobby_id){
    for(size_t i = 0; i < serv->lobbies->size; i++){
        if(serv->lobbies->array[i].id == lobby_id)
            return i;
    }
    return -1;
}

CSML_Lobby* get_lobby_from_id(CSML_Server* serv, int lobby_id){
    for(size_t i = 0; i < serv->lobbies->size; i++){
        if(serv->lobbies->array[i].id == lobby_id)
            return &serv->lobbies->array[i];
    }
    return NULL;
}

void free_server(CSML_Server* serv){
    free_array_pollfd(serv->pfds);
    free_array__sclient(serv->connected_clients);
    free(serv->buffer);
    free(serv);
}

void add_sclient(CSML_Server* serv, _sclient client){
    array_add__sclient(serv->connected_clients, client);
}

void remove_sclient(CSML_Server* serv, int idx){
    array_remove__sclient(serv->connected_clients, idx);
}

void* _get_in_addr(struct sockaddr* sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int _get_listener_socket(char* host_name, char* serv_name){
    int listener;
    int yes=1;
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(host_name, serv_name, &hints, &ai)) != 0){
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next){
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
            continue;
        
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0){
            close(listener);
            continue;
        }

        break;
    }

    if (p == NULL)
        return -1;
    freeaddrinfo(ai);

    if (listen(listener, 10) == -1)
        return -1;

    return listener;
}

void add_to_pfds(CSML_Server* serv, int new_fd){
    pollfd pfd;
    pfd.fd = new_fd;
    pfd.events = POLLIN;

    array_add_pollfd(serv->pfds, pfd);
}

void del_from_pfds(CSML_Server* serv, int idx){
    array_remove_pollfd(serv->pfds, idx);
}
