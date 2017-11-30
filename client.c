#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define maxline 100
//#define name_len 20

char *EXIT_STRING = "exit";
int tcp_connect(int af, char *servip, unsigned short port);
void errquit(char *mesg){ perror(mesg); exit(1); }

int main(int argc, char *argv[]){
	char file_name[maxline];
	int maxfdp1;
	int s, nbyte, fd;
	int namelen;
	fd_set read_fds;

	if(argc != 3){
		printf("usage : %s server_ip port filename\n", argv[0]);
		exit(1);
	}

	s = tcp_connect(AF_INET, argv[1], atoi(argv[2]));
	if(s == -1) errquit("tcp_connect fail");
	memset(file_name, 0x00, maxline);
	//init file_name
	puts("connect to server");
	maxfdp1 = s+1;
	FD_ZERO(&read_fds);

	while(1){
		FD_SET(0, &read_fds);

		if(select(maxfdp1, &read_fds, NULL, NULL, NULL) <0) errquit("select fail");
		if(FD_ISSET(0, &read_fds)){
			if(fgets(file_name, maxline, stdin)){
				//input file name to file_name
				file_name[strlen(file_name)-1] = '\0';
				
				if(strstr(file_name, EXIT_STRING) != NULL){
					puts("good bye");
					close(s);
					exit(0);
				}
				printf("file name is %s...\n", file_name);
				
				if(send(s, file_name, maxline, 0) < 0) puts("erre : send error on select");
				//send file_name to server
				if((fd = open(file_name, O_RDONLY)) < 0) perror("open source file error :") ;
				//open file using file_name
				
				sleep(1);
				printf("sending...\n");
				memset(file_name, 0x00, maxline);
				nbyte = read(fd, file_name, maxline);
				//read from open file
				send(s, file_name, nbyte, 0);
				//send file data to server
				if(nbyte == 0) break;
			}
		}
	}
}

int tcp_connect(int af, char *servip, unsigned short port){
	struct sockaddr_in servaddr;
	int s;

	if((s = socket(af, SOCK_STREAM, 0)) <0) return -1;

	bzero((char*)&servaddr, sizeof(servaddr));
	servaddr.sin_family = af;
	inet_pton(AF_INET, servip, &servaddr.sin_addr);
	servaddr.sin_port = htons(port);

	if(connect(s, (struct sockaddr*)&servaddr, sizeof(servaddr)) <0) return -1;
	return s;
}

