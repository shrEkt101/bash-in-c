#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	int ret;

	printf("A\n");
	ret = fork();

	printf("B\n");
	//returns -1? error handling
	if(ret < 0) {
		perror("fork");
		exit(1);

	} 
	// if a child returns
	else if (ret == 0) {
		printf("C\n");

	}
	//if a parent returns
	else {
		printf("D\n");
	}

	printf("E\n");
	return 0;
}
