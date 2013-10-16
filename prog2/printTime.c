#include <sys/time.h>
#include <stdio.h>

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

		if ((currentTime.tv_usec - startTime.tv_usec) % 1000 == 0) {
			printf("Current time diff: %lu seconds, %lu microseconds\n", currentTime.tv_sec - startTime.tv_sec, currentTime.tv_usec - startTime.tv_usec);
		}
	}
}