#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>

int main(int argc, char *argv[]) {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) 
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) 
    {
        perror("listen() failed");
        return 1;
    }

    // Ham kiem tra ten client
    
    int checkname(char name[]){
        char cmp[2048];
        int len = strlen(name);
        int k = 1;
        for (int i = 0; i < len; i++){
            if (i < 11) {cmp[i] = name[i];} else{
                if (name[i] == ' '){
                    printf("Cu phap client khong dung: \n");
                    k = 0;
                    break;
                }
            }
        }
        if(strcmp(cmp, "client_id: ") != 0){
            k = 0;
        }
        return k;
    }

    struct pollfd fds[64];
    int nfds = 1;

    fds[0].fd = listener;
    fds[0].events = POLLIN;

    char buf[256];
    char *names[64];

    while (1){
        int ret = poll(fds, nfds, -1);
        if (ret < 0){
            perror("poll() failed");
            break;
        }
        
        if (fds[0].revents & POLLIN){ 
            int client = accept(listener, NULL, NULL);
            if (nfds == 64){
                // Tu choi ket noi
                close(client);
            }
            else{
                fds[nfds].fd = client;
                fds[nfds].events = POLLIN;
                nfds++;

                printf("New client connected: %d\n", client);
            }
        
            char msg[] = "Nhap ten clien theo cu phap: client_id: nameclient";
            char msg1[] = "Cu phap khong dung moi nhap lai: ";
            char clientname[128];
            send(client, msg , sizeof(msg), 0);
        
            while (1){
                ret = recv(client, clientname, sizeof(clientname), 0);
                printf("clientname: %s", clientname);
                if (ret <= 0) break;
                if (checkname(clientname)){
                    break;
                }else{
                    send(client, msg1, sizeof(msg1), 0);
                }
            }
            names[client - 4] =  malloc(strlen(clientname) + 1);
            strcpy(names[client - 4], clientname);
           /*  names[client - 4] = clientname; */
            printf("%s\n", names[client-4]);
        }
    

        for (int i = 1; i < nfds; i++){
            
            
            if (fds[i].revents & POLLIN){
                ret = recv(fds[i].fd, buf, sizeof(buf), 0);
                
                if (ret <= 0){
                    close(fds[i].fd);
                    // Xoa khoi mang
                    if (i < nfds - 1)
                        fds[i] = fds[nfds - 1];
                    nfds--;
                    i--;
                }
                else{
                    char name[128];
                    strcpy(name, names[fds[i].fd - 4]);
                    char client_id[strlen(name)-11];
                    for (int i = 11; i < strlen(name); i++){
                        if(name[i] == '\n')break;
                        client_id[i-11] = name[i];
                    }
                    client_id[strlen(name)-11] = 0;
                    char msg[256];
                    strcpy(msg, client_id);
                    strcat(msg, ": ");
                    buf[ret] = 0;
                    strcat(msg, buf);
                    
                    for(int j = 1; j < nfds; j++){
                        if(j != i) send(fds[j].fd, msg, sizeof(msg), 0);
                    }
                    
                    printf("%s\n", msg);
                }
            }
        }
    }
    
    close(listener);    

    return 0;
}