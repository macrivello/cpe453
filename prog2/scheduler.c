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

//struct to build linked list of processes
struct typedef proc_list_node{
	int pid;
	char* name;   //not sure if necessary
	char* args[];
	/*
		other stuff?
	*/
	proc_list_node *next;
}proc_list_node;

proc_list_node *curr;

//dedicated function to quickly, and not so thouroughly
// validate command-line arguments
// Returns: 0 if all good. >0 not so good
int arg_check(int, char, int);
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


int main(int argc, char *argv[]){
  int i, quantum, err;
  proc_list_node *head;

  if(arg_check(argc, argv, &quantum)){
  	 fprintf(stderr, "usage: schedule quantum [prog 1 [args] [: 
  		prog 2 [args] [: prog3 [args] [: … ]]]]\n
  	  \tquantum must be an integer greater than 0.\n
  	  Maximum programs: %d. Maximum arguments per program: % d.\n",
  	  MAX_PROGS, MAX_ARGS);
  	 exit();
  }
  assert(quantum > 0);


  return 0;
}

int arg_check(int argc, char *argv[], int *quantum){
	int err = 0, arg_count = 0, prog_count = 0;
	// immediate check if too many or too few args
  if ( (argc > (MAX_PROGS * MAX_ARGS) + 2) || argc < 3) return(++err);
  
  // check if the 'quantem' value is valid. using atol() to
  // determine if the value is a integer
  if ( !(*quantum = atol(argv[1])) ) return(++err);

  *argv++;
  while(*argv++){
  	if(**argv == ':'){
  		prog_count++;	
  		arg_count = 0;
  	}
  	if(arg_count++ > 10) return(++err);
  }
  if(prog_count > MAX_PROGS - 1) return(++err);

  return err;
}

void* build_proc_list(void *list_head, char** argv, char *delimiter){
	proc_list_node *head = calloc(1, sizeof(proc_list_node));
	proc_list_node *temp = head;


}

int scheduler(proc_list_node* head, int quantum){

}

int remove_node(proc_list_node *head, int pid){
	
}
