#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <unistd.h>	
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h> 
#include <string.h> 

#define PORT	1024
#define MAXBUF	1024
int main() { 
	int server_sockfd; 
	int client_sockfd; 
	int fd;	//file descriptor for saving
	struct sockaddr_in serveraddr, clientaddr; //socket structure
	int client_len, read_len, file_read_len; //length
	char buf[MAXBUF]; 
	int check_bind;

	client_len = sizeof(clientaddr);
	if(server_sockfd = socket(AF_INET, SOCK_STREAM, 0) < 0) {
		//make socket 
		perror("socket error : "); 
		exit(0); 
	}

	bzero(&serveraddr, sizeof(serveraddr));
	//init serveraddr
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serveraddr.sin_port	= htons(PORT);
	//set information at serveraddr
	
	if(bind(server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
		//bind server_sockfd and serveraddr
		perror("bind error : "); 
		exit(0);
	}

	if(listen(server_sockfd, 5) < 0) perror("listen error : ");
	//listen client's connect request

	while(1) {
		char file_name[MAXBUF];
		memset(buf, 0x00, MAXBUF);
		//init buf

		if ((client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr, &client_len)) < 0) {
			//accept client's connect request
			perror("accept error : ");
		}

		printf("New Client Connect : %s\n", inet_ntoa(clientaddr.sin_addr));

		if((read_len = read(client_sockfd, buf, MAXBUF)) > 0) {
			//read client_sockfd
			strcpy(file_name, buf); 
			printf("%s > %s\n", inet_ntoa(clientaddr.sin_addr), file_name); 
		}
		else { 
			close(client_sockfd); 
			//close client_sockfd
			break; 
		} 

	FILE_OPEN: 

		if((fd = open(file_name, O_WRONLY | O_CREAT | O_EXCL, 0700)) < 0) {
			//open file
			perror("file open error : "); 
			break;
		} 
		if(errno == EEXIST) { 
			//already exist same named file
			close(fd); 
			size_t len = strlen(file_name); 
			file_name[len++] = '_'; 
			file_name[len++] = 'n';
			file_name[len] = '\0'; 
			goto FILE_OPEN;
		}

		while(1) { 
			//save file
			memset(buf, 0x00, MAXBUF); 
			file_read_len = read(client_sockfd, buf, MAXBUF); 
			write(fd, buf, file_read_len); 
			if(file_read_len == EOF | file_read_len == 0) { 
				printf("finish file\n");
				break; 
			} 
		} 
		close(client_sockfd); 
		close(fd); 
	} 
	close(server_sockfd);
	return 0; 
}
