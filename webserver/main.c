#include "socket.h"

int main(void) {

	printf("Test1");
	int socket_serveur = creer_serveur(8080);
	if (socket_serveur == -1)
	{
		perror("creer_serveur");
		exit(1);
	}

	int socket_client ;
	socket_client = accept(socket_serveur, NULL ,NULL);
	
	if ( socket_client == -1 )
	{
		perror("accept");
		exit(1);
	}

	const char * message_bienvenue = "Bonjour, bienvenue sur mon serveur \n";
	write(socket_client, message_bienvenue, strlen(message_bienvenue));
	printf("Test2");
	// char buffer[50];
	// int bytes;
	// while ((bytes = read(socket_client, buffer, 50)) > 0) {
	// 	printf("%s", buffer);
	// }
}