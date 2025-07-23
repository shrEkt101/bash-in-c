#include <stdio.h>
#include <stdlib.h>


int main(){
	int num;
	char phone[11];
	//int run=1;
	
	
	while(EOF  != scanf("%d %10s", &num, phone)){
	printf("%d\n", num);
	
	if(num == 0){
		printf("%10s\n", phone);
	}else{
		printf("%c\n", phone[num]);
	}
	
}

	return 0;
}
