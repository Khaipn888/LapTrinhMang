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
void handleReturnTime(char *in1, char *in2, char *out);

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
    
    while (1){
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        
        buf[ret] = 0;
        printf("Received from %d: %s\n", client, buf);
        char lenh[256], format[256], tmp[256];
        int ret1 = sscanf(buf,"%s%s%s", lenh, format, tmp);
        if(ret1 == 2){
            handleReturnTime(lenh, format, tmp);
            send(client, tmp, sizeof(tmp), 0);
        }else{
            char msg[256];
            strcpy(msg, "Sai cu phap.\n");
            send(client, msg, sizeof(msg), 0);
        }
    }

    close(client);
}

void handleReturnTime(char *in1, char *in2, char *out){
    printf("%s", out);
    if(strcmp(in1, "GET_TIME") == 0){
        time_t rawtime;
        struct tm *info;
        time( &rawtime );
        info = localtime( &rawtime );
        if(strcmp(in2, "dd/mm/yyyy") == 0){
            strftime(out ,256,"Time: %d/%m/%Y\n", info);
        }else   if(strcmp(in2, "dd/mm/yy") == 0){
                    strftime(out ,256,"Time: %d/%m/%y\n", info);
                }else   if(strcmp(in2, "mm/dd/yyyy") == 0){
                            strftime(out ,256,"Time: %m/%d/%Y\n", info);
                        }else   if(strcmp(in2, "mm/dd/yy") == 0){
                                    strftime(out ,256,"Time: %x\n", info);
                                }else { strcpy(out, "Khong ho tro format nay.\n"); }

        return;  
    }else{
        strcpy(out,"Lenh sai, moi nhap lai.\n" ) ;
        return;
    }
}