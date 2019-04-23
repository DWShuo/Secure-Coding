CC=gcc
CFLAGS = -Wall

all: daytimed daytime echod echo
daytimed: daytime_server.c
	cc -o daytimed daytime_server.c
daytime: daytime_client.c
	cc -o daytime daytime_client.c
echod: echo_server.c
	cc -o echod echo_server.c
echo: echo_client.c
	cc -o echo echo_client.c
