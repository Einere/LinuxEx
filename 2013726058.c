#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define SIZE 10
#define PERM 0655

//read R_file's data, write to W_file.
int copy(const char* R_file, const char* W_file){
	int R_fd = 0, W_fd = 0;
	ssize_t nread = 0;
	char buf[SIZE];

	//file open error implement
	if((R_fd = open(R_file, O_RDONLY)) < 0){
		printf("file for read opening is failed!!\n");
		return -1;
	}

	//file open error implement
	if((W_fd = open(W_file, O_WRONLY | O_CREAT, PERM)) < 0){
		printf("file for write opening is failed!!\n");
		close(R_fd);
		return -1;
	}
	
	//read R_file's data
	if((nread = read(R_fd, buf, sizeof(buf))) > 0){
		lseek(W_fd, sizeof(buf), SEEK_SET);
		write(W_fd, buf, nread);
	}
	else{
		printf("file reading is failed!!\n");
	}

	//close file descriptor
	close(R_fd);
	close(W_fd);

	printf("nread = %ld,buffer is %s\n", nread, buf);

	return 0;
}

int main(){
	copy("Input","Output");
	return 0;
}

