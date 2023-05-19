#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main( int argc, char* argv[]){
    // Kiem tra tham so hop le
    if (argc != 4){
        printf("Tham so khong hop le!");
        exit(1);
    } 
    // Khai bao socket
    int client_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Khai bao dia chi cua server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[2]);
    addr.sin_port = htons(atoi(argv[3]));
    // Ket noi den server
    int res = connect(client_tcp, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1) {
    printf("Khong ket noi duoc den server!");
    return 1;
    }
    // Nhap va gui tin nhan den server
    char msg[255];
    puts("Nhap tin nhan: ");
    fgets(msg, sizeof(msg), stdin);
    send(client_tcp, &msg, strlen(msg), 0);
    char buf[2048];
    int len = recv(client_tcp, buf, sizeof(buf), 0);
    buf[len] = 0;
    printf("Data received: %s\n", buf);
    // Ket thuc, dong socket
    close(client_tcp);
    return 0;
}