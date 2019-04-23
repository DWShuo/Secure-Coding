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
#include <time.h>
#include <arpa/inet.h>
#include <pwd.h>

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

int* getNobodyID(){
    char* username = "nobody";
    static int returnArray[2];
    struct passwd *pwd = calloc(1, sizeof(struct passwd));
    if(pwd == NULL){
        fprintf(stderr, "Failed to allocate struct passwd for getpwnam_r.\n");
        exit(1);
    }
    size_t buffer_len = sysconf(_SC_GETPW_R_SIZE_MAX) * sizeof(char);
    char *buffer = malloc(buffer_len);
    if(buffer == NULL){
        fprintf(stderr, "Failed to allocate buffer for getpwnam_r.\n");
        exit(1);
    }
    getpwnam_r(username, pwd, buffer, buffer_len, &pwd);
    if(pwd == NULL){
        fprintf(stderr, "getpwnam_r failed to find requested entry.\n");
        exit(1);
    }
    returnArray[0] = (int)pwd->pw_uid;
    returnArray[1] = (int)pwd->pw_gid;

    //printf("uid: %d\n", pwd->pw_uid);
    //printf("gid: %d\n", pwd->pw_gid);

    free(pwd);
    free(buffer);
    return returnArray;
}

int main (int argc, char *argv[]) {
    int listenfd, connfd;
    int pid;

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

        pid = fork();

        if(pid == -1){ fprintf(stderr, "forking failed\n"); }
        if(pid > 0){ close(connfd); }
        else{
            int* IDs = getNobodyID();
            int UID = IDs[0];
            int GID = IDs[1];
            if (setuid(UID) != 0 && setgid(GID) != 0){
                fprintf(stderr, "Privileges could not be dropped\n");
                exit(1);
            }
            while (1) {
                int read = recv(connfd, buff, BUFFER_SIZE, 0);

                if (!read) break; // done reading
                if (read < 0) fprintf(stderr, "Client read failed\n");

                int rc = send(connfd, buff, read, 0);
                if (rc < 0) fprintf(stderr, "Client write failed\n");
            }
        }
    }
    close(connfd);
}
