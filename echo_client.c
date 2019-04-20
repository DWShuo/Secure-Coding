#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
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

int main(int argc, char **argv){

    if (argc !=3) {
        printf("usage: ./a.out <server> <echo message>\n");
        exit(1);
    }

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

    int len = strlen(argv[2])+1;
    int r = write(sockfd, argv[2], len);
    printf("echo client: sending\n");
    printf("%s\n", argv[2]);

    char *buf = malloc(len);
    buf[len-1] = '\0';
    r = read(sockfd, buf, len);
    printf("echo client: received\n");
    printf("%s\n", buf);

}
