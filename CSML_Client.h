#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

typedef struct pollfd pollfd;

typedef struct{
    pollfd server_fd;
    char* buffer;
    size_t buffer_size;
} CSML_Client;

// send flag = 0
void *_get_in_addr(struct sockaddr *sa);
CSML_Client* make_client(char* host_name, char* serv_name,
                        size_t buffer_capacity);
int poll_server(CSML_Client*);
void send_to_server(CSML_Client*, void*, size_t);
void free_client(CSML_Client*);
