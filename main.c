#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>





int shell_launch(char **args) // launches program 
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child 
    if (execv(args[0], args) == -1) {
      printf("execv failed: Enter correct path bro %s\n");
      exit(0);
    }
  } else if (pid < 0) {
    // Error forking
    printf("error");
    exit(0);
  } else {
    // Parent 
    do {
      waitpid(pid, &status, WUNTRACED); // wait
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}


int shell_execute(char *args)
{
  int i;
  char *newArgs = strdup(args);
  char **args_arr = malloc(sizeof(char **)*(strlen(newArgs)));
  args_arr = &newArgs;
  free(newArgs);
  if (args[0] == NULL) { // if user inputs an empty command 
    
    return 1;
  }
  for(i = 0; i < strlen(args-1); i++) // loop to run multiple args programs 
  {
    if (strcmp(&args[i], "exit") == 0) {
      return 0;
    }
    return shell_launch(&args[i]);
  }

  return shell_launch(&args[i]);
}

char *shell_read_line(void)
{
  int buffersize = 1024;
  int position = 0;
  char *buffer = malloc(sizeof(char) * 1024);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(0);
  }

  while (1) {
    c = getchar();

    if (c == "#" || c == '#'){ // allows user to comment 
      c = '\0';
    }
    if (c == EOF || c == '\n') { 
      buffer[position] = '\0';
      return buffer;
    } 
    else {
      buffer[position] = c;
    }
    position++;

    if (position >= 1024) { // reallocates if exceeding buffer 
      buffersize += 1024;
      buffer = realloc(buffer, buffersize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(0);
      }
    }
  }
}

char **tokenify(const char *s) {
    
	char *newS = strdup(s);
	int count = 0; 
	const char *whitespace = " ;\t\n";
	char *token;
	for(token = strtok(newS, whitespace); token!=NULL; token=strtok(NULL,whitespace)) {
		
		count++;
				
	}  
	free(newS);
	newS = strdup(s);
	char **arr = malloc(sizeof(char *)*(count+1));
	int i = 0;
	for(token = strtok(newS, whitespace); token!=NULL; token=strtok(NULL,whitespace))
	{	
		arr[i] = strdup(token);
		i++;
	}
	free(newS);
	arr[i] = NULL;
	return arr;
	
	
	

}


void shell_loop(void)
{
  char *line;
  char **args;
  int status;

  char buffer[1024];
  printf("> ");
  fflush(stdout);
  while (fgets(buffer, 1024, stdin) != NULL){ 
    while (status){
    printf("> ");
    line = shell_read_line();
    args = tokenify(line);
    status = shell_execute(args);

    free(line);
    free(args);
  } 
 }
}

int main(int argc, char **argv)
{
  
  shell_loop(); // run loop

  return 0;
}
