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

/*
  Function Declarations for builtin shell commands:
 */
int shell_exit(char **args); // inits exit command 


char *exit_str[] = {
  "exit" // gets built in exit string
};

int (*exit_func[]) (char **) = {
  &shell_exit
};

int shell_num_builtins() {
  return sizeof(exit_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
 
char **tokenify(char *s) {
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
		//printf("%s\n",arr[i]);
		i++;
	}
	free(newS);
	arr[i] = NULL;
        printf(arr[0]);
        printf(arr[1]);
	return arr;	

}

char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}


/*int main(int argc, char **argv) {
	
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
