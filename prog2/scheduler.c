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

//struct to build linked list of processes
typedef struct proc_list_node{
	pid_t pid;
  char** args;
  struct proc_list_node *prev;
  struct proc_list_node *next;
} node;

int quantum;
int currentPid = -1;

node *head;

void printUsageAndExit();
void forkFest();
int scheduler();
node* remove_node(pid_t);
void print_list();

void timer_handler(int signum) 
{
  kill(currentPid, SIGSTOP);
} 

int main(int argc, char *argv[]) {
  node *curr;
  int progCount, argCount, parsingArgs = 0;
  
  if (argc < 2 || (quantum = atol(argv[1])) <= 0) {
    printUsageAndExit();
  } else if (argc == 2) {
    return 0;
  }

  argv = argv + 2;

  while (*argv) {
    if (!parsingArgs) {
      parsingArgs = 1;
      node *tailNode = curr;
      
      curr = calloc(1, sizeof(node));
      argCount = 0;
      progCount++;

      if (progCount > MAX_PROGS) {
        printUsageAndExit();
      }

      curr->pid = -1;
      curr->args = argv++;

      if (head == NULL) {
        head = curr->next = curr->prev = curr;
      } else {
        tailNode->next = curr;
        curr->next = head;
        curr->prev = tailNode;
        head->prev = curr;
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

  forkFest();
  
  scheduler();

  return 0;
}

void printUsageAndExit() {
  fprintf(stderr, "usage: schedule quantum [prog 1 [args] [: prog 2 [args] [: prog3 [args] [: … ]]]]\n");
  fprintf(stderr, "\tquantum must be an integer greater than 0.\n");
  fprintf(stderr, "Maximum programs: %d. Maximum arguments per program: % d.\n", MAX_PROGS, MAX_ARGS);
  exit(1);
}

void forkFest()  {
  node *curr = head;
  pid_t pid;
  while (curr->pid == -1) {
    pid = fork();
    if (pid > 0) {
      //Parent
      curr->pid = pid;
    } else if (pid == 0) {
      //Child
      raise(SIGSTOP);

      execvp(curr->args[0], curr->args);
      printf("There was an error executing your process %s. \n", curr->args[0]);
      printf("You should probably try again but do it correctly\n");
      exit(1);
    }

    curr = curr->next;
  }
  print_list();

}
  
int scheduler() {
  node *curr = head;
  struct sigaction sa;  
  struct itimerval timerset;
  int status; //in case we need reference
  pid_t r_pid;

  memset(&sa, 0, sizeof (sa)); 
  sa.sa_handler = &timer_handler; 
  sigaction(SIGALRM, &sa, NULL); 

  timerset.it_interval.tv_usec = 0;
  timerset.it_interval.tv_sec = 0;

  while (curr) {
    timerset.it_value.tv_usec = (quantum % 1000) * 1000;
    timerset.it_value.tv_sec = quantum / 1000;
 
    printf("usec %lu sec %lu\n", timerset.it_value.tv_usec, timerset.it_value.tv_sec);

    setitimer(ITIMER_REAL, &timerset, NULL);

    if(curr->pid < 0) exit(1);
    currentPid = curr->pid;
    kill(curr->pid, SIGCONT);
    r_pid = waitpid(curr->pid, &status, WUNTRACED);  //WUNTRACED returns child information on stopped and terminated
    
    if (r_pid > 0 && (WIFEXITED(status) || WIFSIGNALED(status))) {
      curr = remove_node(r_pid);
    } else {
      curr = curr->next;
    }
  }
}

node* remove_node(pid_t pid){
  printf("removing pid %d\n",pid );
  node *temp = head;
  node *ret = NULL;

  while(temp && (temp->pid != pid))
    temp = temp->next;

  if(temp == head) {
    if(temp->next == head){ //1 node
      free(temp);
      head = NULL;
      return head;
    }
    head = temp->next;
  } 
  ret = (temp->prev)->next = temp->next;
  (temp->next)->prev = temp->prev;
  free(temp);
  return ret;   
}

void print_list(){
  node* curr = head;
  curr = head;
  int j = 0;
  printf("head: %X\n",head);
  if(head){
    do{
      printf("\tcurr %X, prev %X, next %X\n", curr, curr->next, curr->prev);
      printf("\t%X pid: %d\n args: ",curr, curr->pid);

      for(j = 0; curr->args[j]; j++){
        printf("%s ", curr->args[j]);
      }
      printf("\n");
      curr = curr->next;
    } while(head != curr);
  }else{
    printf("Empty list\n");
  }
}