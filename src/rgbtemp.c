#include <stdio.h>
#include <stdlib.h> /* For exit() function */
#include <unistd.h> /* For sleep() function */
#include <signal.h>
#include <string.h>

unsigned int sleep(unsigned int seconds);

#define FALSE 0
#define TRUE  1

#define SLEEP 0.1  /* Sleeping time in seconds */

#define BLACK  "00000000 00000000 00000000" /* off */
#define WHITE  "FFFFFFFF FFFFFFFF FFFFFFFF" /* >20º */
#define CYAN   "00000000 FFFFFFFF FFFFFFFF" /* >30º */
#define BLUE   "00000000 00000000 FFFFFFFF" /* >40º */
#define GREEN  "00000000 FFFFFFFF 00000000" /* >50º */
#define YELLOW "FFFFFFFF FFFFFFFF 00000000" /* >60º */
#define ORANGE "FFFFFFFF 33333333 00000000" /* >70º */
#define RED    "FFFFFFFF 00000000 00000000" /* >80º */
#define PINK   "FFFFFFFF 00000000 FFFFFFFF" /* >90º */

/* #define DEF_PATH "/sys/class/hwmon/hwmon1/temp3_input" */

int running = TRUE;

void set_color(int celsius) {
	/* printf("Los grados son: %i\n", celsius); */
	int exitstatus = 0;
    char s[50];
	if	  (celsius >= 90000)   { snprintf(s, 50, "openrgb -c FF00%X", (celsius-90000)*256/10000); exitstatus = system(s); } // PINK
	else if (celsius >= 80000) { snprintf(s, 50, "openrgb -c FF%X00", (90000-celsius)*64/10000); exitstatus = system(s); } // RED
	else if (celsius >= 70000) { snprintf(s, 50, "openrgb -c FF%X00", (80000-celsius)*192/10000); exitstatus = system(s); } // ORANGE
	else if (celsius >= 60000) { snprintf(s, 50, "openrgb -c %XFF00", (celsius-60000)*256/10000); exitstatus = system(s); } // YELLOW
	else if (celsius >= 50000) { snprintf(s, 50, "openrgb -c 00%X%X", (celsius-50000)*256/10000, (60000-celsius)*256/10000); exitstatus = system(s); } // GREEN
	else if (celsius >= 40000) { snprintf(s, 50, "openrgb -c 00%XFF", (50000-celsius)*256/10000); exitstatus = system(s); } // BLUE
	else if (celsius >= 30000) { snprintf(s, 50, "openrgb -c %XFFFF", (40000-celsius)*256/10000); exitstatus = system(s); } // CYAN
	else if (celsius >= 20000) { exitstatus = system("openrgb -c FFFFFF"); }
	else					   { exitstatus = system("openrgb -c 000000"); }

	if (exitstatus) running = FALSE;
}

void sig_handler(int signo)
{
	if (signo == SIGUSR1) {
		fprintf(stderr, "\nINFO: Received SIGUSR1\n");
		running = FALSE;
	}
	else if (signo == SIGTERM) {
		fprintf(stderr, "\nINFO: Received SIGTERM\n");
		running = FALSE;
	}
	else if (signo == SIGINT) {
		fprintf(stderr, "\nINFO: Received SIGINT\n");
		running = FALSE;
	}
	else if (signo == SIGKILL)
		fprintf(stderr, "\nINFO: Received SIGKILL\n");
	else if (signo == SIGSTOP)
		fprintf(stderr, "\nINFO: Received SIGSTOP\n");
	else
		fprintf(stderr, "\nINFO: Received: %d", signo);
}

int main(int argc, char *argv[]) {
	int i;
	int cleanexit;
	int fscanf_ok;
	char c[8];
	char fileaddr[255];

	const char* env = getenv("RGBTEMP");

	FILE *fptr;

	switch (argc)
	{
		case 1:
			if (env != NULL) strcpy(fileaddr, env);
			else
			{
				fprintf(stderr, "RGBTEMP environment variable is empty.\n"
				"Please, supply a file in such variable or by command line.\n"
				"Exiting.\n");
				exit(2);
			}
			break;
		case 2:
			strcpy(fileaddr, argv[1]);
			break;
		default:
			fprintf(stderr, "\nWarning: incorrect number of args: %d.", argc);
			break;
	}

	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
		fprintf(stderr, "WARNING: Can't catch SIGUSR1\n");
	if (signal(SIGTERM, sig_handler) == SIG_ERR)
		fprintf(stderr, "WARNING: Can't catch SIGTERM\n");
	if (signal(SIGINT, sig_handler) == SIG_ERR)
		fprintf(stderr, "WARNING: Can't catch SIGINT\n");
	/*if (signal(SIGKILL, sig_handler) == SIG_ERR)
		fprintf(stderr, "%s", "WARNING: Can't catch SIGKILL\n");
	if (signal(SIGSTOP, sig_handler) == SIG_ERR)
		fprintf(stderr, "%s", "WARNING: Can't catch SIGSTOP\n"); */

	while (running) {
	if ((fptr = fopen(fileaddr, "r")) == NULL) {
		fprintf(stderr, "ERROR: Opening file %s", fileaddr);
		/* Program exits if file pointer returns NULL. */
		exit(1);
	}

	/* Reads text until newline is encountered */
	fscanf_ok = fscanf(fptr, "%[^\n]", c);
	fclose(fptr);
	
	system("openrgb -m Static");
	
	if (!fscanf_ok) { return 0; }

	i = atoi(c);
	set_color(i);
	sleep(SLEEP);
	}

	/* system("msi-rgb --disable --base-port 4e " WHITE);  */
	cleanexit = system("openrgb -m Off");  /* No pulsing */

	if (cleanexit) { return 1; }

	return 0;
}
