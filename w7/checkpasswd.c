#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  
  int p[2];

  int r=fork();
  if(r==-1){
    perror("fork");
    exit(1);
  }

  if (r==0){
    close(p[1]);
    dup2(p[0], STDERR_FILENO);
    exec1("./validate", "main", NULL);
    perror("exec1");
    exit(1);
  }else{
    close(p[0]);
    write(p[1], user_id, MAXLINE);
    write(p[1], password, MAX_PASSWORD);
    close(p[1]);
  }

  int status;
  wait(&status);

  if (WIFEXITED(status)){
    int exit_code = WEXITSTATUS(status);
    if(exit_code){
      printf(SUCCESS);
    }else if(exit_code == 2){
      printf(INVALID);
    }else if(exit_code == 3){
      printf(NO_USER);
    }else{
      exit(1);
    }
  }

  return 0;
}
