#include "CSML_Client.h"

#define PORT "9034"

int main(int argc, char** argv){
	CSML_Client* client = make_client(NULL, PORT, 256);
    int x = 1;
    while(1){
        if(x == 1 || x == 2){
            x++;
            char* c = "1000 LOBBY";
            send_to_server(client, c, strlen(c));
        }
        int resp = poll_server(client);
        if(resp == -2)
            break;
        else if(resp == -1)
            continue;
        else
            printf("MSG RECEIVED\n");
    }
    free_client(client);
    return 0;
}
