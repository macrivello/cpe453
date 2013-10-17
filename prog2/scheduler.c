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

#define MAX_ARGS 10
#define MAX_PROGS 10

const int quantum;

//struct to build linked list of processes
struct typedef proc_list_node{
	int pid;
	char* name;   //not sure if necessary
	char* args[];
	/*
		other stuff?
	*/
  proc_list_node *prev;
	proc_list_node *next;
}proc_list_node;

proc_list_node *curr;
static int count = 0;

void printUsageAndExit() 
//takes void* head pointer, argv, and delimiter to build 
//linked list of processes. This function does the parsing,
// forking (including sending stop signal)
//returns head of linked list
void* build_proc_list(void*, char**, char);
//the round robing scheduling
//takes head pointer to proc_list and quantum value
//runs until all processes in linked list are terminated
//returns 0 if all good, >0 on error
int scheduler(proc_list_node*, int);
//called once a processes is finished to remove node from 
//linked list
//takes head pointer of list and pid of node to be removed.
//returns 0 if OK, >0 on error
int remove_node(proc_list_node*, int);
//print the linked list
void print_list(void*);

int main(int argc, char *argv[]) {
  proc_list_node *head = NULL;
  int progCount, argCount, parsingArgs = 0;
  if (!(quantum = atol(argv[1]))) {
    printUsageAndExit();
  }

  argv = argv + 2;

  while (*argv) {
    if (!parsingArgs) {
      parsingArgs = 1;

      proc_list_node *node = calloc(1, sizeof(proc_list_node));
      count++;
      argCount = 0;
      progCount++;

      if (progCount > MAX_PROGS) {
        printUsageAndExit();
      }

      node->name = argv++;
      node->args = argv;

      if (head == NULL) {
        node->next = node->prev = head = curr = node;
      } else {
        node->prev = curr;
        curr = curr->next = node;
        curr->next = head;
      }
    } else {
      if (*argv == ':') {
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

  curr = curr->next;

  return 0;
}

void printUsageAndExit() {
  fprintf(stderr, "usage: schedule quantum [prog 1 [args] [: 
      prog 2 [args] [: prog3 [args] [: … ]]]]\n
      \tquantum must be an integer greater than 0.\n
      Maximum programs: %d. Maximum arguments per program: % d.\n",
      MAX_PROGS, MAX_ARGS);
  exit(1);
}

void forkFest()  {
  int pid;

  while (curr->pid == 0) {
    if (pid = fork()) {
      //Parent
      curr->pid = pid;
    } else {
      //Child
      //Create FILE pointer for exec
      pause();
    }
  }
}

int scheduler(proc_list_node* head, int quantum){

}

int remove_node(proc_list_node *head, int pid){
	proc_list_node* head = curr;
  proc_list_node* temp = head;

  if(count > 1){
    while(head && head->pid != pid){
      temp = head;
      head = head->next;  
    } 
    if(head->pid == pid){
      temp = head;
      head->prev->next = head->next;
      free(temp);
    }    
  }else if(count == 1){
    free(curr);
    curr = NULL;
  }else{
    //no node to free, empty list
    return 1; 
  }
  count--; 
  return 0;
   
}

void print_list(void* node){
  proc_list_node* start;
  proc_list_node* head = (proc_list_node*) node;
  start = head;
  int i = 0;

  if(head){
    do{
      printf("%d pid: %d\nname: %s\nargs: %s\n\n", i, head->pid,head->name,head->args);
      i++;
      head = head->next;
    } while(head && head != start);
  }

}