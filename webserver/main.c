#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <regex.h>

#include "main.h"
#include "socket.h"

#define BUFFER_SIZE 150

int main(void) {

	const char * message_bienvenue = "Bonjour, bienvenue sur mon serveur \n";

	initialiser_signaux();

	int socket_serveur = creer_serveur(8080);
	if (socket_serveur == -1)
	{
		perror("creer_serveur");
		exit(1);
	}

	int socket_client ;
	while(1) {

		socket_client = accept(socket_serveur, NULL ,NULL);
		
		if ( socket_client == -1 )
		{
			perror("accept");
			exit(1);
		}

		/*
		 * char *c token = strtok(chaine, " ");  <= iterateur
		 * while(token != NULL)
		 * 	token = strtok(NULL, " ");
		 */

		if (fork() == 0) {
			close(socket_serveur); //car fils pas besoin socket : il ya encore la reférence dans son père
			
			char buffer[BUFFER_SIZE];

			FILE *client = fdopen(socket_client, "w+");

			fprintf(client, message_bienvenue, strlen(message_bienvenue));
			fflush(client);

			int init_status = init_connection(client, buffer);

			while (init_status == 1) {
				bzero(buffer,BUFFER_SIZE);

				if(fgets(buffer, BUFFER_SIZE, client) == NULL) {
					break;
				}

				// Skiping header
				/*
				while(fgets(buffer, BUFFER_SIZE, client) != NULL) {			
					if (strcmp(buffer, "\r\n") == 0)
						break;
				} */
				
				printf("Message: %s", buffer);
				
				// concat <fsociety> + buffer
				/*
				char *message = malloc(strlen(buffer) + 11);
				strcpy(message, "<fsociety> ");
				strcat(message, buffer);

				if(fprintf(client, message, strlen(message)) <= 0) {
					break;
				} */ 
			}

			printf("DEBUG: client deconnecté \n");
			return -1;
		}
		close(socket_client);
	}
}

int init_connection(FILE *client, char *buffer) {
	bzero(buffer,BUFFER_SIZE);

	if(fgets(buffer, BUFFER_SIZE, client) == NULL)
		return -1;

	printf("buffer : [%s]\n", buffer);

	/* REGULAR EXPRESSION VERIFICATION */

	regex_t regex;
	int status_code;

	/* COMPILE */
	status_code = regcomp(&regex, "^GET / HTTP/1.[0-1]\r\n$", 0);
	printf("Status code : %d\n", status_code);
	if (status_code)
		return -1;

	/* EXEC */
	status_code = regexec(&regex, buffer, 0, NULL, 0);
	printf("Status code : %d %d\n", status_code, REG_NOMATCH);
	if (status_code || status_code == REG_NOMATCH)
		return -1;

	/* FREE */
	regfree(&regex);

	while(strcmp(buffer, "\r\n") != 0 && strcmp(buffer, "\n") != 0) {
		printf("Buf : %s", buffer);
		if(fgets(buffer, BUFFER_SIZE, client) == NULL)
			return -1;
	}

	printf("test");
	return 1;
}

void initialiser_signaux() {

	struct sigaction sa;

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		perror("signal");

	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	
	if(sigaction(SIGCHLD , &sa , NULL) == -1)
		perror("sigaction  SIGCHLD)");

}

void traitement_signal(int sig){
	printf("Signal %d reçu \n", sig);
	while(waitpid(-1,NULL,WNOHANG) > 0);
}