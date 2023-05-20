#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<time.h>

// Server nhan file tu client

int main(int argc, char *argv[]){
    // Kiem tra tham so hop le
    if (argc != 3){
        printf("Tham so khong hop le!");
        exit(1);
    } 
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

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
    // Kiem tra ten file
    if(strcmp(argv[2], "sv_log") != 0){
            printf("khong tim thay file: %s", argv[2]);
            exit(1);
        }
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(addr);

    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    printf("Client IP: %s, Port: %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    // Lay time client gui du lieu
    time_t rawtime;
    struct tm *info;
    char buffer[80];
    time( &rawtime );
    info = localtime( &rawtime );
    strftime(buffer,80,"%Y-%m-%d%X", info);

    //***
    char buf[2048]; 
    FILE *f = fopen("sv_log.txt", "ab+");
    int ret;
    char log[2048];
    // Viet ham xoa ky tu " " trong chuoi log:
        char* delete_space(char *str,char *result) {
        int pos = 0;
        
        const int len = strlen(str); /*Tìm độ dài chuỗi ban đầu*/
        for(int i = 0; i < len; i++) {
            char c = str[i];

            /*Nếu ký tự lấy ra là ký tự trắng thì bỏ qua*/
            if (c == '\r' || c == '\n' || c == ' ') {
                continue;
            }

            /*Thêm ký tự lấy ra vào kết quả*/
            result[pos++] = c;
        }
        return result;
        };
    // Nhan va ghi du lieu:
    int k = 0;
    while (1)
    {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        char x[2048];
        delete_space(buf, x);
            if (k == 0){
                strcpy(log, x);
                
            }else{
                strcat(log, x);
                
            }
      /*   strcat(log, y); */
        /* printf("%s", buf); */
        k++;
    } 
    strcat(buffer, log);
    printf("%s\n", log);
    fprintf(f, "%s\n", buffer);

    fclose(f);
    
    close(client);
    close(listener);
}