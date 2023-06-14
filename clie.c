#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int num_pieces = 0;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return -1;
    }
    printf("Client is active and working!\n");
    fflush(stdout);
    sendto(sock, "connect", strlen("connect"), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    sleep(1);
    memset(buffer, 0, 1024);
    int serv_len = sizeof(serv_addr);
    valread = recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&serv_addr, &serv_len);
    num_pieces = atoi(buffer);
    printf("There are %d pieces of meat in the pot.\n", num_pieces);
    while (1) {
        printf("Press enter to eat a piece of meat.\n");
        getchar();
        sendto(sock, "eat", strlen("eat"), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        sleep(1);
        memset(buffer, 0, 1024);
        int serv_len = sizeof(serv_addr);
        valread = recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&serv_addr, &serv_len);
        sleep(1);
        printf("%s\n", buffer);
    }
    return 0;
}
