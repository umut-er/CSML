// Thanks Beej! Base code sourced from the great network guide! Check it out: https://beej.us/guide/bgnet/pdf/bgnet_usl_c_1.pdf

#ifndef _server_helper_h_
#define _server_helper_h_

#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>
#include <stdbool.h>

#include "dynamic_array.h"
#include "CSML_Server_Components.h"

typedef struct pollfd pollfd;

typedef struct{
    int fd;
    struct sockaddr_storage remote_addr;
    socklen_t addr_size;
} _sclient;

DYN_ARR_DECL(_sclient)
typedef dyn_arr__sclient client_arr;
DYN_ARR_DECL(pollfd)
typedef dyn_arr_pollfd pollfd_arr;
DYN_ARR_DECL(CSML_Lobby)
typedef dyn_arr_CSML_Lobby CSML_LobbyArray;

typedef struct CSML_Server{
    char* host_name;
    char* serv_name;

    int cur_lobby_id;
    int base_server_fd;

    void (*connection_initializer)(struct CSML_Server*, int);

    char* buffer;
    size_t buffer_size;

    CSML_LobbyArray* lobbies;
    client_arr* connected_clients;
    pollfd_arr* pfds;
} CSML_Server;

typedef void (*on_connect_func)(CSML_Server*, int);

CSML_Server* initialize_server(char* host_name, char* serv_name,
                            size_t client_capacity, size_t buffer_size,
                            size_t lobby_capacity,
                            on_connect_func connection_initializer);
void poll_server(CSML_Server*);

int add_lobby_to_server(CSML_Server*, void* secret_state, size_t client_count_hard_cap, size_t response_cap);
int add_premade_lobby_to_server(CSML_Server*, CSML_Lobby);
void remove_lobby_from_server(CSML_Server*, int lobby_id);

void add_client_to_lobby(CSML_Server*, int, int);
void remove_client_from_lobby(CSML_Server*, int, int);
bool is_client_in_lobby(CSML_Lobby*, int);

void lobby_send_to_clients(CSML_Lobby*, int sender, bool send_to_sender, 
                            void* msg, size_t msg_size);
// TODO: Implement these
// void lobby_send_to_typed_clients(CSML_Lobby*, int tag, int sender, bool send_to_sender, void* msg, size_t msg_size)
// void lobby_send_if()

int get_lobby_index(CSML_Server*, int lobby_id); // Deprecate this.
CSML_Lobby* get_lobby_from_id(CSML_Server*, int lobby_id);

void free_server(CSML_Server*);

void add_sclient(CSML_Server*, _sclient client);
void remove_sclient(CSML_Server*, int idx);

void* _get_in_addr(struct sockaddr*);
int _get_listener_socket(char* host_name, char* serv_name);
void add_to_pfds(CSML_Server*, int new_fd);
void del_from_pfds(CSML_Server*, int idx);

#endif
