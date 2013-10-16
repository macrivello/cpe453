#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
   int childPID, status, counter;

   printf("Buffer up a bit of output..");

   if ((childPID = fork()) < 0)
      fprintf(stderr, "Bad fork call\n");
   else if (childPID > 0) {
      printf(" parent has id %d with child %d\n", getpid(), childPID);
      for (counter = 0; counter < 200000000; counter++) 
         if (counter % 50000000 == 0)              // Every 50,000,000..
            printf("Parent at %d\n", counter);
      printf("Parent is done with loop...\n");
      printf("Child %d exits ", wait(&status));
      printf("with status %d\n", WEXITSTATUS(status));
   }
   else {
      printf(" child has id %d\n", getpid());
      for (counter = 0; counter < 200000000; counter++) 
         if (counter % 50000000 == 0) {            // Every 50,000,000..
            printf("Child at %d\n", counter);
            sleep(1);                              // Slow child down a little
         }
      return 42;  // or exit(42)
   }
   return 0;
}

/* Sample run
Buffer up a bit of output.. parent has id 13880 with child 20876
Parent at 0
Buffer up a bit of output.. child has id 20876
Child at 0
Parent at 50000000
Parent at 100000000
Parent at 150000000
Parent is done with loop...
Child at 50000000
Child at 100000000
Child at 150000000
Child 20876 exits with status 42
*/
