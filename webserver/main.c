#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

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

		if (fork() == 0) {
			close(socket_serveur); //car fils pas besoin socket : il ya encore la reférence dans son père
			
			write(socket_client, message_bienvenue, strlen(message_bienvenue));
			
			char buffer[BUFFER_SIZE];
			while (1) {
				bzero(buffer,BUFFER_SIZE);
								
				if(read(socket_client, buffer, BUFFER_SIZE) <= 0){
					//buffer[150] = '\0';
					break;
				}
				
				printf("Message: %s", buffer);
				
				if(write(socket_client, buffer, strlen(buffer)) <= 0){
					break;
				}
			}
			printf("DEBUG: client deconnecté \n");
			return -1;
		}
		close(socket_client);
	}
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