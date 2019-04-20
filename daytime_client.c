#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>
#include <limits.h>
#include <unistd.h>

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

int main(int argc, char **argv) {
    int sockfd, n = 0;
    char recvline[1000 + 1];
    struct sockaddr_in servaddr;
    int port = sec_atoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err(1, "Socket Error");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        err(1, "Connect Error");
    }

    while ((n = read(sockfd, recvline, 1000)) > 0) {
        recvline[n] = 0;
        fputs(recvline, stdout);
    }
    return 0;
}
