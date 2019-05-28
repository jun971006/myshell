#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	int pid;
	pid = fork();
	
	switch(pid){
		case -1:
			printf("fork failed");
			break;
		case 0:
			execl("/bin/ls", "ls", "-l", (char*) 0);
			printf("exec failed");
			break;
		default:
			wait((int*) 0);
			printf("ls completed\n");
			exit(0);
	}
	return 0;
}
