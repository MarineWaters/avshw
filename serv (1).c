#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

int num_clients = 0;
int num_pieces = 5;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    int client_fd = *(int*)arg;
    char buffer[1024] = {0};
    sprintf(buffer, "%d", num_pieces);
    sendto(client_fd, buffer, strlen(buffer), 0, (struct sockaddr*)NULL, sizeof(struct sockaddr));
    sleep(1);
    while (1) {
        memset(buffer, 0, 1024);
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        int valread = recvfrom(client_fd, buffer, 1024, 0, (struct sockaddr*)&client_addr, &client_len);
        pthread_mutex_lock(&mutex);
        if (valread == 0) {
            break;
        }
        if (strcmp(buffer, "eat") == 0) 
            if (num_pieces == 0) {
                sprintf(buffer, "Waking up the cook to refill the pot...nYou ate a piece of meat. 4 pieces left.");
                num_pieces = 5;
                printf("\nA savage asked to refill the pot. Pot was refilled.\n");
                fflush(stdout);
            }
            else{
            sprintf(buffer, "You ate a piece of meat. %d pieces left.", num_pieces-1);
            }
                num_pieces--;
                sendto(client_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
                pthread_mutex_unlock(&mutex);
                sleep(1);
        }
    }
    close(client_fd);
    num_clients--;
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t threads[4];
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8888);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is active and working!");
    fflush(stdout);
    while (1) {
        char buffer[1024] = {0};
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        int valread = recvfrom(server_fd, buffer, 1024, 0, (struct sockaddr*)&client_addr, &client_len);
        sleep(1);
        if (num_clients < 4) {
            pthread_create(&threads[num_clients], NULL, handle_client, &server_fd);
            num_clients++;
        } else {
            char *msg = "Maximum number of clients reached.";
            sendto(server_fd, msg, strlen(msg), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
            sleep(1);
        }
    }
    return 0;
}
