#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <err.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 1024

static int sec_atoi(const char* str){
    errno = 0;
    char *temp;
    long val = strtol(str, &temp, 0);

    if (temp == str || *temp != '\0' || ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE)) {
        fprintf(stderr, "Failed to parse an integer\n");
        exit(EXIT_FAILURE);
    }

    int result = (int)val;
    long val2 = result;
    if (val != val2) {
        fprintf(stderr, "Failed to parse an integer\n");
        exit(EXIT_FAILURE);
    }
    return result;
}

int main (int argc, char *argv[]) {
    int listenfd, connfd;

    if (argc != 2){
        printf("usage: ./a.out <port>\n");
        exit(1);
    }
    int port = sec_atoi(argv[1]);

    struct sockaddr_in servaddr;
    char buff[1000];
    time_t ticks;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, 8);

    while (1) {
        connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
        if (connfd < 0){
            fprintf(stderr, "Could not establish new connection\n");
        }
        while (1) {
            int read = recv(connfd, buff, BUFFER_SIZE, 0);

            if (!read) break; // done reading
            if (read < 0) fprintf(stderr, "Client read failed\n");

            int rc = send(connfd, buff, read, 0);
            if (rc < 0) fprintf(stderr, "Client write failed\n");
        }
    }
    return 0;
}
