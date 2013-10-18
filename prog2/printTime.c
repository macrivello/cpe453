#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	struct timeval startTime;
	gettimeofday(&startTime, NULL);

	printf("Arc count: %d\n", argc);
	while (argc--) {
		printf("arg[%d]: %s\n", argc, argv[argc]);
	}

	while (1) {
		struct timeval currentTime;
		gettimeofday(&currentTime, NULL);
	
		if ((currentTime.tv_usec - startTime.tv_usec) % 50000 == 0) {
			printf("%s time: %lu seconds, %lld microseconds\n", argv[1], currentTime.tv_sec - startTime.tv_sec, currentTime.tv_usec - startTime.tv_usec);

			if (currentTime.tv_sec - startTime.tv_sec > atol(argv[2])) {
				printf("Time expired! exiting\n");
				exit(0);
			}
		}
	}

	printf("Reached end of main\n");
}