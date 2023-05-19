#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
int main( int argc, char* argv[]) {
        // Kiem tra tham so hop le
        if(argc != 5){
            printf("Tham so khong hop le!");
            exit(1);
        }
        char chao[256] = "Server xin chao Client!\n";
       
        // Tao socket
        int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listener != -1)
        printf("Socket created: %d\n", listener);
        else {
        printf("Failed to create socket.\n");
        exit(1);
        }
        // Khai bao cau truc dia chi server
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(atoi(argv[2]));
        // Gan dia chi voi socket
        if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) 
        {
        printf("bind() failed.\n");
        exit(1);
        }
        if (listen(listener, 5)) 
        {
        printf("listen() failed.\n");
        exit(1);
        }
        printf("Waiting for a new client ...\n");
        // Chap nhan ket noi
        int client = accept(listener, NULL, NULL);
        if (client == -1)
        {
        printf("accept() failed.\n");
        exit(1);
        }
        printf("New client connected: %d\n", client);
        // Nhan du lieu tu client
        char tinnhan[256];
        int ret = recv(client, tinnhan, sizeof(tinnhan), 0);
        if (ret <= 0)
        {
        printf("recv() failed.\n");
        exit(1);
        }
        // Them ky tu ket thuc xau va in ra man hinh
        if (ret < sizeof(tinnhan))
        tinnhan[ret] = 0;
        puts(tinnhan);
        // Gui du lieu sang client
        if(strcmp(argv[3],"chao") == 0) {
            printf("ok");
            send(client, chao, strlen(chao), 0);
        }
        else printf("khong tim thay tap tin %s\n", argv[3]);
       if(strcmp(argv[4],"tinnhan") == 0) {
            printf("ok");
            send(client, tinnhan, strlen(tinnhan), 0);
        }
        else printf("khong tim thay tap tin %s\n", argv[4]);
        // Dong ket noi
        close(client);
        close(listener); 
        return 0;
}
