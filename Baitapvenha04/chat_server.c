#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

void *client_thread(void *);
int numclient = 0;
int clients[10];

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1){
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) {
        perror("listen() failed");
        return 1;
    }

    while (1){
        int client = accept(listener, NULL, NULL);
        if (client == -1){
            perror("accept() failed");
            continue;;
        }
        printf("New client connected: %d\n", client);
        char msg[256];
        strcpy(msg, "Nhap ten clien theo cu phap: client_id: nameclient\n");
        send(client, msg, sizeof(msg), 0);
        
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_thread, &client);
        pthread_detach(thread_id);
    }
    
    close(listener);    

    return 0;
}

void *client_thread(void *param){
    int client = *(int *)param;
    char buf[256];
    char msg1[256],msg2[256],id[256], name[256], tmp[256];
    strcpy(msg1, "sai cu phap. Hay nhap lai.\n ");
    strcpy(msg2, "Dang nhap thanh cong.\n ");
    int stt = 0;
    
    while (1){

        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        
        buf[ret] = 0;
        printf("Received from %d: %s\n", client, buf);
        if(stt == 0){
            int ret1 = sscanf(buf, "%s%s%s", id, name, tmp);
            if((ret1 == 2) && (strcmp(id, "client_id:") == 0)){
                
                    send(client, msg2, sizeof(msg2), 0);
                    clients[numclient] = client;
                    numclient++;
                    stt = 1;
            }else{
                send(client, msg1, sizeof(msg1), 0);
            }
        }else{
            strcpy(tmp, name);
            strcat(tmp, ": ");
            strcat(tmp, buf);
            for(int i = 0; i < numclient; i++){
                if (clients[i] != client){
                    send(clients[i], tmp, sizeof(tmp), 0);
                }
            }
        }
    }

    close(client);
}