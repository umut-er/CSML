#include <assert.h>

#include "../include/CSML_Client.h"

void *_get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

CSML_Client* make_client(char* host_name, char* serv_name,
						size_t buffer_capacity){
    int sockfd;  
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	rv = getaddrinfo(host_name, serv_name, &hints, &servinfo);
    assert(rv == 0);

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			continue;
		}
		break;
	}
    assert(p != NULL);

	inet_ntop(p->ai_family, _get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo);

    CSML_Client* client = (CSML_Client*)malloc(sizeof (CSML_Client));
    client->server_fd.fd = sockfd;
    client->server_fd.events = POLLIN;
    client->buffer_size = buffer_capacity;
    client->buffer = (char*)malloc(client->buffer_size * sizeof (char));
    return client;
}

// -1 for no code, -2 for server disconnecting, 1 if the client received something.
int poll_server(CSML_Client* client){
    int poll_count = poll(&client->server_fd, 1, 0);
    assert(poll_count != -1);

    if(!(client->server_fd.revents & POLLIN))
        return -1;
    
    int nbytes = recv(client->server_fd.fd, client->buffer, client->buffer_size, 0);
    assert(nbytes >= 0);

    if(nbytes == 0)
        return -2;
    return 1;
}

void send_to_server(CSML_Client* client, void* message, size_t msg_size){
	send(client->server_fd.fd, message, msg_size, 0);
}

void free_client(CSML_Client* client){
    free(client->buffer);
    free(client);
}
