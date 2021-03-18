#include <unistd.h>
extern char *optarg;
extern int optind, opterr,optopt;
int main(int argc, char *argv[]) {
	int flags=0, opt, nsecs=0, tfnd=0;
	while ((opt = getopt(argc, argv, "nt:")) != -1) {
		switch (opt) {
			case 'n': flags = 1; break;
			case 't': tfnd = 1; nsecs = atoi(optarg); break;
			default: /* '?' */
				  fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv[0]);
				  exit(EXIT_FAILURE);
		}
	}
	printf("flags=%d; tfnd=%d; optind=%d\n", flags, tfnd, optind);
	if (optind >= argc) {
		fprintf(stderr, "Expected argument after options\n");
		exit(EXIT_FAILURE);
	}
	printf("name argument = %s\n", argv[optind]);
	/* Tähän sovelluksen varsinainen koodi */
	exit(EXIT_SUCCESS);
}
