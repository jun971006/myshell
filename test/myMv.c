#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	int rfd, wfd, n;			 
	char buf[2];	
	struct stat statBuf;	
	rfd = open(argv[1], O_RDONLY);	
	wfd = open(argv[2], O_CREAT | O_WRONLY);
	
	if(rfd == -1){		
		perror("argv[1] file open error");	
		exit(1);		
	}
	else if(wfd == -1){	
		perror("argv[2] file open error");	
		exit(1);	
	}
	
	while((n = read(rfd, buf, 1))>0)	
		if(write(wfd, buf, n) != n) perror("Write");	
		
	if(n == -1) perror("Read"); 
	
	fstat(rfd, &statBuf);
	fchmod(wfd, statBuf.st_mode); 
	
	remove(argv[1]);	
	
	close(rfd);	
	close(wfd);
	
	return 0;
}
