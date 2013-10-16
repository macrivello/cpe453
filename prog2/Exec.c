#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
   int childPID, counter;
   char *mockEnvp[] = {"tag1=value1", "tag2=42", "tag3=whatever", NULL};

   if ((childPID = fork()) == 0) {
      execl("/bin/echo", "echo", "Hello", "world", NULL); 
      printf("Execl call Failed.\n");
   }
   else {
      fprintf(stderr, "Still the parent: Echo pid is %d\n", childPID);
      if ((childPID = fork()) == 0) 
         execlp("printenv", "printenv", NULL);
      else {
         fprintf(stderr, "Still the parent: Printenv pid is %d\n", childPID);
         if ((childPID = fork()) == 0)
            execle("/usr/bin/printenv", "printenv", NULL, mockEnvp);
         else
            fprintf(stderr, "Still the parent: Second printenv pid is %d\n",
              childPID);
      }
   }

   for (counter = 0; counter < 3; counter++)
      printf("Child %d is done.\n", wait(NULL));
}

/* Sample run
Still the parent: Echo pid is 29170
Hello world
Still the parent: Printenv pid is 29171
Still the parent: Second printenv pid is 29172
TERM=vt100
SHELL=/bin/bash
USER=cstaley
MAIL=/var/mail/cstaley
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games
PWD=/home/cstaley/Modules
LANG=en_US.UTF-8
HOME=/home/cstaley
LOGNAME=cstaley
SSH_CONNECTION=192.168.1.30 55413 192.168.1.11 22
Child 29171 is done.
tag1=value1
tag2=42
tag3=whatever
Child 29172 is done.
Child 29170 is done.
  */
