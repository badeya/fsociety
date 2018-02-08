#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <signal.h>

#include "main.h"
#include "socket.h"

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
			close(socket_serveur);
			write(socket_client, message_bienvenue, strlen(message_bienvenue));

			while (1) {
				char buffer[150];
				while(read(socket_client, buffer, 150) > 0) {
					//buffer[150] = '\0';
					printf("Message: %s", buffer);
				}
				printf("DEBUG: client deconnecté \n");
				return -1;
			}
		}

		close(socket_client);

	}
}

void initialiser_signaux() {
	
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		perror("signal");

}