#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>

int main (int argc, char *argv[]){

	if (argc < 2){
		fprintf(stderr, "%s", "Error: No listen_port given\n");
		exit(0);
	}

	int listen_port = atoi(argv[1]);

    //create socket
	int srv_sock;
	srv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_sock < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in srv;
    srv.sin_family = AF_INET;
    srv.sin_port = htons(listen_port);
    srv.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind
    if (bind(srv_sock, (struct sockaddr *) &srv, sizeof(srv)) < 0){
        perror("bind");
        exit(EXIT_FAILURE);
    } 

    //listen
    if (listen(srv_sock, 5) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept
    struct sockaddr_in cli;
    int cli_sock;
    unsigned int cli_len = sizeof(cli);
    cli_sock = accept(srv_sock, (struct sockaddr*) &cli, &cli_len);
    if (cli_sock <  0){
        perror("accept");
        exit(EXIT_FAILURE);
    }

    char buf[64]; //= "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\0";
    int nbytes;
    while(1){
        if ((nbytes = read(cli_sock, buf, sizeof(buf))) < 0){
            perror("read");
            exit(EXIT_FAILURE);
        }
        printf("%s%s:%d\n", "recv from ", inet_ntoa(cli.sin_addr), (int)ntohs(cli.sin_port));
        if ((nbytes = write(cli_sock, buf, sizeof(buf)) < 0)){
            perror("write");
            exit(EXIT_FAILURE);
        }        
    }

    close(srv_sock);

	return 0;
}