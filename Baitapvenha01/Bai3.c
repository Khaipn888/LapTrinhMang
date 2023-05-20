#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Client gửi file sang server

int main(int argc, char *argv[]){
// Kiem tra tham so hop le
    if (argc != 3){
        printf("Tham so khong hop le!");
        exit(1);
    } 

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }

    char *filename = "information.txt";

    FILE *infor = fopen(filename, "wb");
    char ht[255];
    char ns[255];
    char mssv[255];
    char cpa[255];
    puts("Ho va ten: ");
    fgets(ht, sizeof(ht), stdin);
    fprintf(infor, "%s", ht);
    puts("Ngay sinh: ");
    fgets(ns, sizeof(ns), stdin);
    fprintf(infor, "%s", ns);
    puts("Mssv: ");
    fgets(mssv, sizeof(mssv), stdin);
    fprintf(infor, "%s", mssv);
    puts("CPA: ");
    fgets(cpa, sizeof(cpa), stdin);
    fprintf(infor, "%s", cpa);
    fclose(infor);

    char buf[2048];

    int name_size = strlen(filename);
    send(client, &name_size, sizeof(int), 0);
    strcpy(buf, filename);
    send(client, buf, strlen(filename), 0);

    FILE *infor1 = fopen(filename, "rb");
    while (!feof(infor))
    {
        ret = fread(buf, 1, sizeof(buf), infor1);
        if (ret <= 0)
            break;
        send(client, buf, ret, 0);
        printf("%s",buf);
    }

    fclose(infor);
    
    close(client);
}