#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv) {

	int i;
	int n;
	int num_kids;

	if (argc != 2) {
		fprintf(stderr, "Usage: parentcreates <numkids>\n");
		exit(1);
	}

	num_kids = strtol(argv[1], NULL, 10);

	for (i = 0; i < num_kids; i++) {
		n = fork();
		if (n < 0) {
			perror("fork");
			exit(1);
		}
		else if(n == 0){
			printf("pid = %d, ppid = %d, i = %d\n", getpid(), getppid(), i);
			exit(0);
		}else{
			printf("pid = %d, ppid = %d, i = %d\n", getpid(), getppid(), i);
		}
	}

	return 0;
}
