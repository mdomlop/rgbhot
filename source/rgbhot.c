#include <stdio.h>
#include <stdlib.h> /* For exit() function */
#include <unistd.h> /* For sleep() function */
#include <signal.h>
#include <string.h>

#define PROGRAM     "RGBHot"
#define EXECUTABLE  "rgbhot"
#define DESCRIPTION "Change RGB colors according to temperature."
#define VERSION     "0.3"
#define URL         "https://github.com/mdomlop/rgbhot"
#define LICENSE     "GPLv3+"
#define AUTHOR      "Manuel Domínguez López"
#define NICK        "mdomlop"
#define MAIL        "zqbzybc@tznvy.pbz"

unsigned int sleep(unsigned int seconds);

#define FALSE 0
#define TRUE  1

#define SLEEP 2  /* Sleeping time in seconds */

#define BLACK  "00000000 00000000 00000000" /* off */
#define WHITE  "FFFFFFFF FFFFFFFF FFFFFFFF" /* >20º */
#define CYAN   "00000000 FFFFFFFF FFFFFFFF" /* >30º */
#define BLUE   "00000000 00000000 FFFFFFFF" /* >40º */
#define GREEN  "00000000 FFFFFFFF 00000000" /* >50º */
#define YELLOW "FFFFFFFF FFFFFFFF 00000000" /* >60º */
#define ORANGE "FFFFFFFF 33333333 00000000" /* >70º */
#define RED    "FFFFFFFF 00000000 00000000" /* >80º */
#define PINK   "FFFFFFFF 00000000 FFFFFFFF" /* >90º */

#define CMD "msi-rgb --base-port 4e --pulse "

/* #define DEF_PATH "/sys/class/hwmon/hwmon1/temp3_input" */

int running = TRUE;
int tstatus = 0;  /* Store values from 0-9
					   for not repeat system() commands */

void set_color(const char *color, int tstat)
{
	if (tstatus != tstat)
	{
		char cmd[58]; /* Command has a fixed size. 57 chars + 1*/
		strcpy(cmd, CMD);
		strcat(cmd, color);

		if (system(cmd)) running = FALSE;

		tstatus = tstat;
	}


}

void check_color(int celsius)
{
	/* printf("Los grados son: %i\n", celsius); */
	if      (celsius > 90000) { set_color(PINK,   9); }
	else if (celsius > 80000) { set_color(RED,    8); }
	else if (celsius > 70000) { set_color(ORANGE, 7); }
	else if (celsius > 60000) { set_color(YELLOW, 6); }
	else if (celsius > 50000) { set_color(GREEN,  5); }
	else if (celsius > 40000) { set_color(BLUE,   4); }
	else if (celsius > 30000) { set_color(CYAN,   3); }
	else if (celsius > 20000) { set_color(WHITE,  2); }
	else					  { set_color(BLACK,  1); }

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

	const char* env = getenv("RGBHOT");

	FILE *fptr;

	switch (argc)
	{
		case 1:
			if (env != NULL) strcpy(fileaddr, env);
			else
			{
				fprintf(stderr, "RGBHOT environment variable is empty.\n"
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
		fprintf(stderr, "ERROR: Opening file %s\n", fileaddr);
		/* Program exits if file pointer returns NULL. */
		exit(1);
	}

	/* Reads text until newline is encountered */
	fscanf_ok = fscanf(fptr, "%[^\n]", c);
	fclose(fptr);

	if (!fscanf_ok) { return 0; }

	i = atoi(c);
	check_color(i);
	sleep(SLEEP);
	}

	/* system("msi-rgb --disable --base-port 4e " WHITE);  */
	cleanexit = system("msi-rgb --base-port 4e " BLACK);  /* No pulsing */

	if (cleanexit) { return 1; }

	return 0;
}
