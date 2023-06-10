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
    int stt = 0;
    
    while (1){
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0) break;
        buf[ret] = 0;
        printf("Received from %d: %s\n", client, buf);
        if(stt == 0){
            char user[32], pass[32], tmp[65], line[65];
            int ret1 = sscanf(buf, "%s%s%s", user, pass, tmp);
            if (ret1 == 2){
                int found = 0;
                sprintf(tmp, "%s %s\n", user, pass);
                FILE *f = fopen("users.txt", "r");
                while (fgets(line, sizeof(line), f) != NULL){
                        line[strlen(line) - 2] = line[strlen(line) -1];
                        line[strlen(line) - 1] = '\0';

                        if (strcmp(line, tmp) == 0){
                                found = 1;
                                break;
                        }                    
                }
                fclose(f);

                if (found){
                        char *msg = "Dang nhap thanh cong. Hay nhap lenh de thuc hien.\n";
                        send(client, msg, strlen(msg), 0);
                        stt = 1;
                }else{
                        char *msg = "Nhap sai tai khoan. Hay nhap lai.\n";
                        send(client, msg, strlen(msg), 0);
                    }
            }else{
                    char *msg = "Nhap sai cu phap. Hay nhap lai.\n";
                    send(client, msg, strlen(msg), 0);
                }
        }else{
                char tmp[256];
                if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = '\0';
                        strcpy(tmp, buf);
                        strcat(tmp, " > out.txt");

                    // Thuc hien lenh
                        system(tmp);

                    // Tra ket qua cho client
                        char res[256];
                        FILE *f = fopen("out.txt", "rb");
                        while (!feof(f)){
                        int ret2 = fread(res, 1, sizeof(res), f);
                        if (ret2 <= 0)
                            break;
                        send(client, res, ret2, 0);
                    }
                    fclose(f);
            }
    }
    close(client);
}