/*
	Cameron Dunn
	Michael Crivello
	CPE453 - Fall 2013
	Peterson
	Program 2 - Scheduler
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>
#include <sys/time.h>
#include <assert.h>
#include <signal.h>

#define MAX_ARGS 10
#define MAX_PROGS 10
#define MICRO_SECONDS_PER_SECOND 1000000

//struct to build linked list of processes
typedef struct proc_list_node{
	pid_t pid;
	char* name;   
  char** args;
  struct proc_list_node *prev;
  struct proc_list_node *next;
} node;

int quantum;

node *head;
node *currentRunning;

void printUsageAndExit();
void forkFest();
int scheduler();
void remove_node(pid_t);
void print_list();

void timer_handler (int signum) 
{
  printf("stopping pid %d\n",currentRunning->pid);
  kill(currentRunning->pid, SIGSTOP);
} 

int main(int argc, char *argv[]) {
  node *curr;
  int progCount, argCount, parsingArgs = 0;
  
  if (!(quantum = atol(argv[1]))) {
    printUsageAndExit();
  }

  argv = argv + 2;

  while (*argv) {
    if (!parsingArgs) {
      parsingArgs = 1;

      curr = calloc(1, sizeof(node));
      argCount = 0;
      progCount++;

      if (progCount > MAX_PROGS) {
        printUsageAndExit();
      }

      curr->name = *argv++;
      curr->args = argv;

      if (head == NULL) {
        currentRunning = head = curr->next = curr->prev = curr;
      } else {
        node *tailNode = head->prev;
        curr->next = head;
        head->prev = tailNode->next = curr;
      }
    } else {
      if (**argv == ':') {
        *argv = NULL;
        parsingArgs = 0;
      } else {
        argCount++;

        if (argCount > MAX_ARGS) {
          printUsageAndExit();
        }
      }

      argv++;
    }
  }

  print_list();
  printf("\n");
  forkFest();
  
  scheduler();

  return 0;
}

void printUsageAndExit() {
  fprintf(stderr, "usage: schedule quantum [prog 1 [args] [: prog 2 [args] [: prog3 [args] [: … ]]]]\n");
  fprintf(stderr, "\tquantum must be an integer greater than 0.\n");
  fprintf(stderr, "Maximum programs: %d. Maximum arguments per program: % d.\n",MAX_PROGS, MAX_ARGS);
  exit(1);
}

void forkFest()  {
  node *curr = head;
  pid_t pid;
  while (curr->pid == 0) {
    if (pid = fork()) {
      //Parent
      curr->pid = pid;
    } else {
      //Child
      pause();

      //Bootstrap child process
      execvp(curr->name, curr->args);
    }

    curr = curr->next;
  }
  print_list();
}

int scheduler() {
  node *curr = currentRunning;
  struct sigaction sa;  
  struct itimerval timerset;
  int status, remove; //in case we need reference
  pid_t r_pid;

  memset (&sa, 0, sizeof (sa)); 
  sa.sa_handler = &timer_handler; 
  sigaction (SIGALRM, &sa, NULL); 

  timerset.it_interval.tv_sec = timerset.it_value.tv_sec = 0;;
  timerset.it_interval.tv_usec = timerset.it_value.tv_usec = quantum * 1000;
  setitimer(ITIMER_REAL, &timerset, NULL);

  while (curr) {
    //struct itimerval timerset;
    //struct timeval waitTime;

    //waitTime.tv_usec = quantum * 1000;

    // if (waitTime.tv_usec > MICRO_SECONDS_PER_SECOND) {
    //   waitTime.tv_sec = waitTime.tv_usec / MICRO_SECONDS_PER_SECOND;
    //   waitTime.tv_usec = waitTime.tv_usec % MICRO_SECONDS_PER_SECOND;
    // }

    printf("starting pid %d\n", curr->pid);
    currentRunning = curr;
    kill(curr->pid, SIGCONT);
    r_pid = waitpid(curr->pid, &status, WUNTRACED);  //WUNTRACED returns child information on stopped and terminated
    printf("returned waitpid %d on pid %d\n", r_pid, curr->pid);
    if (r_pid){
      if (WIFEXITED(status)) remove = 1;
    }
    curr = curr->next;
    if (remove) remove_node(r_pid);
    remove = 0;
  }
}



void remove_node(pid_t pid){
  node *temp = head;

  if(head->next == head) { //only 1 in the list
    free(head);
    head = NULL;
  }else{
    while(temp->pid != pid)
      temp = temp->next;
    if(temp == head){
      head = temp->next;
    }     
     temp->prev->next = temp->next;
     temp->next->prev = temp->prev;
     free(temp);   
  }
}

void print_list(){
  node* curr = head;
  curr = head;
  int j = 0;

  if(head){
    do{
      printf("pid: %d\n name: %s\n args: ",curr->pid,curr->name);
      for(j = 0; curr->args[j]; j++){
        printf("%s ", curr->args[j]);
      }
      printf("\n");
      curr = curr->next;
    } while(head != curr);
  }

}