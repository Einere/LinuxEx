#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSIZE 100
#define max_sock 1024

char *EXIT_STRING = "exit";
char *START_STRING = "connected to char_server\n";
int maxfdp1;
int num_chat = 0;
int clisock_list[max_sock];
int listen_sock;

void addclient(int s, struct sockaddr_in *newcliaddr);
int getmax();
void removeclient(int s);
int tcp_listen(int host, int sort, int backlog);
void errquit(char *mesg){
	perror(mesg);
	exit(1);
}

int main(int argc, char *argv[]) {
	struct sockaddr_in cliaddr;
	char buf[BUFSIZE];
	int i, j, nbyte, accp_sock, fd, fd2;
	int addrlen = sizeof(struct sockaddr_in);
	fd_set read_fds;

	if (argc != 2) {
		//if lack arguments
		printf("usage : %s port\n", argv[0]);
		exit(1);
	}

	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), 5);

	while (1) {
		FD_ZERO(&read_fds);
		FD_SET(listen_sock, &read_fds);
		//add socket fd to read_fds

		char file_name[BUFSIZE];
		memset(file_name, 0x00, BUFSIZE);
		//for file name to save

		for (i = 0; i < num_chat; i++) FD_SET(clisock_list[i], &read_fds);
		//add clients to observ

		maxfdp1 = getmax() + 1;
		puts("wait for client");

		if (select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0) errquit("select fail");
		//select until client's request

		if (FD_ISSET(listen_sock, &read_fds)) {
			//if client's request is received
			accp_sock = accept(listen_sock, (struct sockaddr*)&cliaddr, &addrlen);
			if (accp_sock == -1) errquit("accept fail");

			addclient(accp_sock, &cliaddr);
			
			recv(accp_sock, file_name, BUFSIZE, 0);

			if ((fd = open(file_name, O_WRONLY | O_CREAT | O_EXCL, 0666)) < 0) {
				//open file to write
				errquit("open file to write fail");
				exit(1);
			}else{
				printf("open file to write sucess\n");
			}
			
			while(10){
				memset(buf, 0x00, BUFSIZE);
				nbyte = recv(accp_sock, buf, BUFSIZE, 0);
				printf("received string is %s...\nnbyte is %d...\n",buf,nbyte);
				write(fd, buf, nbyte);

				if(nbyte == 0) break;
				
			}

		}		
		printf("complete\n");
		close(accp_sock);
		close(fd);
	}
	close(listen_sock);
	return 0;

}

void addclient(int s, struct sockaddr_in *newcliaddr){
	char buf[20];
	inet_ntop(AF_INET, &newcliaddr->sin_addr, buf, sizeof(buf));
	printf("new client : %s\n",buf);
	clisock_list[num_chat] = s;
	num_chat++;
}

void removeclient(int s){
	close(clisock_list[s]);
	if(s != num_chat -1) clisock_list[s] = clisock_list[num_chat -1];
	num_chat--;
	printf("logout one user, number of chatting user : %d\n", num_chat);
}

int getmax(){
	int max = listen_sock;
	int i;

	for(i = 0; i< num_chat; i++){
		if(clisock_list[i] > max) max = clisock_list[i];
	}
	return max;
}

int tcp_listen(int host, int port, int backlog){
	int sd;
	struct sockaddr_in servaddr;

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket fail");
		exit(1);
	}

	bzero((char*)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(host);
	servaddr.sin_port = htons(port);

	if(bind(sd, (struct sockaddr*)&servaddr, sizeof(servaddr)) <0){
		perror("bind fail");
		exit(1);
	}
	listen(sd, backlog);
	return sd;
}

