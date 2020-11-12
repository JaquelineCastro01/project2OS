/* Project 2 - Multithreaded calculator */
// Name: Jaqueline Castro
#include "calc.h"

pthread_t adderThread;
pthread_t degrouperThread;
pthread_t multiplierThread;
pthread_t readerThread;
pthread_t sentinelThread;

char buffer[BUF_SIZE];
int num_ops;

/* Step 3: add mutual exclusion */
int len;
/* Step 6: add condition flag varaibles */
int total_prog = 1;
int addflag = 1;
int multiflag = 1;
int groupflag = 1;
/* Step 7: use a semaphore */
static pthread_mutex_t buffer_lock;

/* Utiltity functions provided for your convenience */
/* int2string converts an integer into a string and writes it in the
passed char array s, which should be of reasonable size (e.g., 20
characters).  */

char * int2string(int i, char * s) {
	sprintf(s, "%d", i);
	return s;
}
/* string2int just calls atoi() */
int string2int(const char * s) {
	return atoi(s);
}
/* isNumeric just calls isdigit() */
int isNumeric(char c) {
	return isdigit(c);
}
/* End utility functions */

void printErrorAndExit(char * msg) {
	msg = msg ? msg : "An unspecified error occured!";
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}
int timeToFinish() {
	/* be careful: timeToFinish() also accesses buffer */
	return buffer[0] == '.';
}
/* Looks for an addition symbol "+" surrounded by two numbers, e.g. "5+6"
and, if found, adds the two numbers and replaces the addition subexpression
with the result ("(5+6)*8" becomes "(11)*8")--remember, you don't have
to worry about associativity! */
void * adder(void * arg) {
	int bufferlen;
	int startOffset, remainderOffset;
	int i;
	int value1, value2;
	int total; // sum of value1, value2
	char string[50];
	while (1) {
		/* Step 3: add mutual exclusion */
		addflag = 1;
		startOffset = remainderOffset = value1 = value2 = -1;
		pthread_mutex_lock(&buffer_lock);
		if (timeToFinish()) {
			pthread_mutex_unlock(&buffer_lock);
			return NULL;
		}

		/* storing this prevents having to recalculate it in the loop */
		bufferlen = (int)strlen(buffer);
		/* Step 2: implement adder */
		 // do we have value1 already?  If not, is this a "naked" number?
		// if we do, is the next character after it a '+'?
		// if so, is the next one a "naked" number?

		// once we have value1, value2 and start and end offsets of the
		// expression in buffer, replace it with v1+v2
		for (i = 0; i < bufferlen; i++) {
			len = bufferlen;
			if (buffer[i] == ';') {
				break;
			}

			// use digit
			if (isdigit(buffer[i])) {

				if (buffer[i] == '+' && buffer[i + 1] == '(') {
					i = i + 2;
				}
				startOffset = i;
				// parse 1st operand
				value1 = string2int(buffer + i);
				// increment index past 1st operand
				while (isdigit(buffer[i])) {
					i++;
				}
				// check if current expression is a + expression
				if (buffer[i] != '+' || !isdigit(buffer[i + 1])) {
					// move onto next iteration if not a + expression
					continue;
				}
				// parse 2nd operand
				value2 = string2int(buffer + i + 1);
				// add operands
				total = value1 + value2;
				// increment index past 2nd operand
				do {
					i++;
				} while (isdigit(buffer[i]));
				// set the remainder of the buffer to index
				remainderOffset = i;
				// result toString
				int2string(total, string);
				// rearrange buffer
				strcpy(buffer + startOffset, string);
				strcpy((buffer + startOffset + strlen(string)), (buffer + remainderOffset));
				// set buffer length and position
				bufferlen = strlen(buffer);
				i = startOffset + (strlen(string)) - 1;
				// increment number of operations
				num_ops++;
				// if(buffer[0] != '\0')
				//   printf("Add %s\n", buffer);
			}
		}
		
		if (strlen(string) == 0 && bufferlen > 0) {
			addflag = 0;
			// printf("No progress can be made\n" );
			// exit(EXIT_FAILURE);
		}
		pthread_mutex_unlock(&buffer_lock);
		sched_yield();
	}
}
/* Looks for a multiplication symbol "*" surrounded by two numbers, e.g.
"5*6" and, if found, multiplies the two numbers and replaces the
mulitplication subexpression with the result ("1+(5*6)+8" becomes
"1+(30)+8"). */
void * multiplier(void * arg) {
	int bufferlen;
	int startOffset, remainderOffset;
	int i;
	int value1, value2;
	int total; // sum of value1, value2
	char string[50];
	while (1) {
		/* Step 3: add mutual exclusion */
		multiflag = 1;
		startOffset = remainderOffset = value1 = value2 = -1;
		pthread_mutex_lock(&buffer_lock);
		if (timeToFinish()) {
			pthread_mutex_unlock(&buffer_lock);
			return NULL;
		}
		/* storing this prevents having to recalculate it in the loop */
		bufferlen = (int)strlen(buffer);
		for (i = 0; i < bufferlen; i++) {
			len = bufferlen;
			if (buffer[i] == ';') {
				break;
			}

			// check if is a digit
			if (isdigit(buffer[i])) {
				if (buffer[i] == '*' && buffer[i + 1] == '(') {
					i = i + 2;
				}
				startOffset = i;
				// parse 1st operand
				value1 = atoi(buffer + i);
				// increment index past 1st operand
				while (isdigit(buffer[i])) {
					i++;
				}
				// check if current expression is a + expression
				if (buffer[i] != '*' || !isdigit(buffer[i + 1])) {
					// move onto next iteration if not a + expression
					continue;
				}
				// parse 2nd operand
				value2 = atoi(buffer + i + 1);
				// multiply operands
				total = value1 * value2;
				// increment index past 2nd operand
				do {
					i++;
				} while (isdigit(buffer[i]));
				// set the remainder of the buffer to index
				remainderOffset = i;
				// result string
				sprintf(string, "%d", total);
				// rearrange buffer
				strcpy(buffer + startOffset, string);
				strcpy((buffer + startOffset + strlen(string)), (buffer + remainderOffset));
				// set buffer length and position
				bufferlen = (int)strlen(buffer);
				i = startOffset + ((int)strlen(string)) - 1;
				// indicate that current thread has updated the buffer
				// increment number of operations
				num_ops++;
				
			}
		}
		if (strlen(string) == 0 && bufferlen > 0) {
			multiflag = 0;
		}
		pthread_mutex_unlock(&buffer_lock);
		sched_yield();
	}
}
/* Looks for a number immediately surrounded by parentheses [e.g.
"(56)"] in the buffer and, if found, removes the parentheses leaving
only the surrounded number. */
void * degrouper(void * arg) {
	int bufferlen;
	int startOffset = 0;
	int i;
	while (1) {
		groupflag = 1;
		pthread_mutex_lock(&buffer_lock);
		if (timeToFinish()) {
			pthread_mutex_unlock(&buffer_lock);
			return NULL;
		}
		/* storing this prevents having to recalculate it in the loop */
		bufferlen = (int)strlen(buffer);
		int naked = 1;
		// check for '(' followed by a naked number followed by ')'
	   // remove ')' by shifting the tail end of the expression
	   // remove '(' by shifting the beginning of the expression
		
		for (i = 0; i < bufferlen; i++) {
			
			len = bufferlen;
			if (naked == 0) {
				break;
			}
			// check for ';' to indicate finished processing expression
			if (buffer[i] == ';') {
				break;
			}
			int j = i;
			if (bufferlen > 0)
				while (j < bufferlen) {
					if (buffer[j] == '(') {
						i = j;
					}
					j++;
				}
			
			// check for '(' followed by a naked number followed by ')'
			if (buffer[i] == '(' && isdigit(buffer[i + 1])) {

				startOffset = i;
				//increment index past all digits
				while (buffer[i] != ')') {
					i++;
					if (buffer[i] == '+' || buffer[i] == '*') {
						naked = 0;
						break;
					}
				}
				
				if (naked == 0)
					continue;
				
				// remove ')' by shifting the tail end of the expression
				strcpy((buffer + i), (buffer + i + 1));

				// remove '(' by shifting the beginning of the expression
				strcpy((buffer + startOffset), (buffer + startOffset + 1));
				// set buffer length and position
                                 num_ops--;
				bufferlen -= 2;
				i = startOffset;
				
				
			}
			
		}
		if (len == bufferlen && bufferlen > 0) {
			groupflag = 0;
		}
		pthread_mutex_unlock(&buffer_lock);
		sched_yield();
	}
}
/* sentinel waits for a number followed by a ; (e.g. "453;") to appear
at the beginning of the buffer, indicating that the current
expression has been fully reduced by the other threads and can now be
output.  It then "dequeues" that expression (and trailing ;) so work can
proceed on the next (if available). */
void * sentinel(void * arg) {
	char numberBuffer[20];
	int bufferlen;
	int i;
	// return NULL; /* remove this line */
	while (1) {
		pthread_mutex_lock(&buffer_lock);
		if (timeToFinish()) {
			pthread_mutex_unlock(&buffer_lock);
			return NULL;
		}

		if (addflag == 0 && multiflag == 0 && groupflag == 0) {
			printf("No progress can be made\n");
			exit(EXIT_FAILURE);
		}

		/* storing this prevents having to recalculate it in the loop */
		bufferlen = strlen(buffer);
		// if(bufferlen > 0)
		//   printf("%d %d %d\n", addprogress, multiprogress, degprogress);
		for (i = 0; i < bufferlen; i++) {
			if (buffer[i] == ';') {
				if (i == 0) {
					printErrorAndExit("Sentinel found empty expression!");
				}
				else {
					/* null terminate the string */
					numberBuffer[i] = '\0';
					/* print out the number we've found */
					fprintf(stdout, "%s\n", numberBuffer);
					/* shift the remainder of the string to the left */
					strcpy(buffer, &buffer[i + 1]);
					break;
				}
			}
			else if (!isNumeric(buffer[i])) {
				break;
			}
			else {
				numberBuffer[i] = buffer[i];
			}
		}
		// something missing?
		/* Step 6: check for progress */
		pthread_mutex_unlock(&buffer_lock);
		/* Step 5: let others play, too */
		sched_yield();
	}
}
/* reader reads in lines of input from stdin and writes them to the
buffer */
void * reader(void * arg) {
	while (1) {
		char tBuffer[100];
		int currentlen;
		int newlen;
		int free;
		fgets(tBuffer, sizeof(tBuffer), stdin);
		/* Sychronization bugs in remainder of function need to be fixed */
		newlen = strlen(tBuffer);
		currentlen = strlen(buffer);
		/* if tBuffer comes back with a newline from fgets, remove it */
		if (tBuffer[newlen - 1] == '\n') {
			/* shift null terminator left */
			tBuffer[newlen - 1] = tBuffer[newlen];
			newlen--;
		}
		/* -1 for null terminator, -1 for ; separator */
		free = sizeof(buffer) - currentlen - 2;
		while (free < newlen) {
			// spinwaiting TO DO
			pthread_mutex_lock(&buffer_lock);

			currentlen = strlen(buffer);
			free = sizeof(buffer) - currentlen - 2;

			pthread_mutex_unlock(&buffer_lock);
			sched_yield();
		}
		/* Step 3: add mutual exclusion */
		/* we can add another expression now */
		pthread_mutex_lock(&buffer_lock);
		strcat(buffer, tBuffer);
		strcat(buffer, ";");
		pthread_mutex_unlock(&buffer_lock);
		sched_yield();
		/* Stop when user enters '.' */
		if (tBuffer[0] == '.') {
			return NULL;
		}

	}
}
/* Where it all begins */
int smp3_main(int argc, char ** argv) {
	void * arg = 0; /* dummy value */
	/* Step 7: use a semaphore */
	if (pthread_mutex_init(&buffer_lock, NULL) != 0) {
		printErrorAndExit("Failed to create mutex");
	}
	/* let's create our threads */
	if (pthread_create(&degrouperThread, NULL, degrouper, arg) ||
		pthread_create(&adderThread, NULL, adder, arg) ||
		pthread_create(&multiplierThread, NULL, multiplier, arg) ||
		pthread_create(&sentinelThread, NULL, sentinel, arg) ||
		pthread_create(&readerThread, NULL, reader, arg)) {
		printErrorAndExit("Failed trying to create threads");
	}
	/* you need to join one of these threads... but which one? */
	pthread_join(sentinelThread, NULL);
	pthread_detach(degrouperThread);
	pthread_detach(multiplierThread);
	pthread_detach(adderThread);
	pthread_detach(sentinelThread);
	pthread_detach(readerThread);
	/* Step 1: we have to join on the sentinel thread. */
	/* everything is finished, print out the number of operations performed */
	fprintf(stdout, "Performed a total of %d operations\n", num_ops);
	// TODO destroy semaphores and mutex
	pthread_mutex_destroy(&buffer_lock);
	return EXIT_SUCCESS;
}
