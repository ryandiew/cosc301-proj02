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

int main(int argc, char **argv) {
	
	loop(); 
    return 0;
}

void loop(void) {
	char *line; 
	char **arguments;
	int isdone; 
	do{
		printf(">");
		line = readlin(); 
		arguments = tokenify(line);
		status = 0; 
		
	}while (isdone)
}

char read_line(void){
	char *line = NULL; 
	ssize_t bufsize = 0; 
	getline(&line, &bufsize, stdin);
	return line; 
}

char** tokenify(const char *s) {
    
	char *newS = strdup(s);
	int count = 0; 
	const char *whitespace = " \t\n";
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

/*

typedef struct process {
   struct process *next; // next process 
   char **argv;
   pid_t pid; // process ID
   char completed; // sets to true if process completed 
   char stopped; // true if process has stopped 
   int status; // reported status value 
}  process;
   
typedef struct job {
   struct job *next; // next active job 
   char *command; // command line 
   process *first_process; // 1st process ID
   char notified_stop; // true if user notified about stopped job
   struct termios tmodes;
   int stdin, stdout, stderr; // i/o channels 
} job;


job *first_job = NULL; // This is its head of active jobs list */ 

/* Find the active job with the indicated pgid.  */ 
/*job *find_job (pid_t pgid)
{
  job *j;

  for (j = first_job; j; j = j->next)
    if (j->pgid == pgid)
      return j;
  return NULL;
} */
/* Return true if all processes in the job have stopped or completed.  */
/*
int job_is_stopped (job *j)
{
  process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed && !p->stopped)
      return 0;
  return 1;
} */
/* Return true if all processes in the job have completed.  */

/*
int job_is_completed (job *j)
{
  process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed)
      return 0;
  return 1;
}
*/
/*int main(int argc, char **argv) {
    cmd_loop(); // runs command loop 
    
    
    return 0;
}*/
