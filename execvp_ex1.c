#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXARG 7

int main()
{
	char *arg[MAXARG];
	char *str1, *str2;
	int argv;
	
	char name[256];
	int pid;
	
	system("clear");
	puts("환영");
	for(;;){
		getcwd(name, 256);
		printf("[%s] @ ", name);
		scanf("%s", str1);
		argv= 0;
		
		arg[0] = str1;
		//arg[1] = str2;
		arg[1] = (char *)0;
		
		if(strcmp(arg[0], "exit") == 0)
		{
			printf("exit\n");
			break;
		}
		if((pid = fork()) == -1)
			puts("fork 함수 에러");
		else if( pid != 0)
		{
			pid = wait(NULL);
			
		}
		else
		{
			execvp(arg[0], arg);
		}
	}
	return 0;
	
}
