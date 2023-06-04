#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include<time.h>

void signalHandler(int signo) {
    int pid = wait(NULL);
    printf("Child %d terminated.\n", pid);
}
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

    signal(SIGCHLD, signalHandler);

    char buf[256];
    while (1){
        int client = accept(listener, NULL, NULL);
        printf("New client connected: %d\n", client);

        if (fork() == 0){
            // Tien trinh con
            close(listener);

            // Xu ly ket noi tu client       
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
                    char *msg = "Nhap sai cu phap hay nhap lai.\n";
                    send(client, msg, sizeof(msg), 0);
                }
            }

            close(client);
            exit(0);
        }

        // Tien trinh cha
        close(client);
    }
    
    close(listener);    

    return 0;
}
void handleReturnTime(char *in1, char *in2, char *out){
    printf("%s", out);
    if(strcmp(in1, "GET_TIME") == 0){
        printf("true\n");
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
                                }else { out = "Khong ho tro format nay."; }

        return;  
    }else{
        out = "Lenh sai, moi nhap lai.\n";
        return;
    }

}