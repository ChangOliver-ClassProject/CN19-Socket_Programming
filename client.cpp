#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <sys/timeb.h>

int main(int argc, char *argv[]){

	int packet_num = 0;
	int timeout = 1000;
	char hostname[512] = "\0";
	int port = -1;

	//argument process
	for (int i = 1; i < argc; i++){
		if (!strcmp(argv[i], "-n")){
			if (i+1 >= argc){
				fprintf(stderr, "%s", "Error: No packet_num given");
				exit(EXIT_FAILURE);
			}
			packet_num = atoi(argv[i+1]);
			i++;
		}
		else if (!strcmp(argv[i], "-t")){
			if (i+1 >= argc){
				fprintf(stderr, "%s", "Error: No timeout given");
				exit(EXIT_FAILURE);			
			}
			timeout = atoi(argv[i+1]);
			i++;
		}
		else{
			strcpy(hostname, strtok(argv[i], ":"));
			port = atoi(strtok(NULL, ":"));
		}
	}

	if (port == -1){
		fprintf(stderr, "%s", "Error: No host given");
		exit(EXIT_FAILURE);
	}

	//create socket
	int cli_sock;
	cli_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_sock < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //connect
    struct sockaddr_in srv;
    bzero(&srv ,sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    struct hostent *server = gethostbyname(hostname);
    srv.sin_addr = *((struct in_addr *)server->h_addr_list[0]);

    if (connect(cli_sock, (struct sockaddr*) &srv, sizeof(srv)) < 0){
    	//perror("connect");
    	//exit(EXIT_FAILURE);
    	printf("timeout when connect to %s:%d\n", hostname, port);
    }

    char buf[64] = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\0";
    int nbytes;
    struct timeb sent_t, rcv_t;
    unsigned short RTT;
	if (packet_num == 0){
		while(1){
			ftime(&sent_t);
		    if ((nbytes = write(cli_sock, buf, sizeof(buf)) < 0)){
		    	printf("timeout when connect to %s:%d\n", hostname, port);	
		    	perror("write");
		    	exit(EXIT_FAILURE);
		    }
		    if ((nbytes = read(cli_sock, buf, sizeof(buf))) < 0){
		        printf("timeout when connect to %s:%d\n", hostname, port);		        
		        perror("read");
		        exit(EXIT_FAILURE);
		    }
		    ftime(&rcv_t);
		    RTT = (rcv_t.millitm - sent_t.millitm) + (rcv_t.time - sent_t.time)*1000;
		    if (RTT > timeout)
		    	printf("timeout when connect to %s:%d\n", hostname, port);
		    else
			    printf("recv from %s:%d, RTT = %hu msec\n", hostname, port, RTT);
		    sleep(1);
		}
	}
	else{
		for (int i = 0; i < packet_num; i++){
			ftime(&sent_t);
		    if ((nbytes = write(cli_sock, buf, sizeof(buf)) < 0)){
		    	printf("timeout when connect to %s:%d\n", hostname, port);	
		    	perror("write");
		    	exit(EXIT_FAILURE);
		    }
		    if ((nbytes = read(cli_sock, buf, sizeof(buf))) < 0){
		        printf("timeout when connect to %s:%d\n", hostname, port);		        
		        perror("read");
		        exit(EXIT_FAILURE);
		    }
		    ftime(&rcv_t);
		    RTT = (rcv_t.millitm - sent_t.millitm) + (rcv_t.time - sent_t.time)*1000;
		    if (RTT > timeout)
		    	printf("timeout when connect to %s:%d\n", hostname, port);
		    else
			    printf("recv from %s:%d, RTT = %hu msec\n", hostname, port, RTT);
		    sleep(1);			
		}
	}

    close(cli_sock);
	return 0;
}