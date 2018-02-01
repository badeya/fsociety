#include "socket.h"

int creer_serveur(int port) {

    // Création Socket 
    int socket_serveur;
    socket_serveur = socket(AF_INET , SOCK_STREAM, 0);
    
    if (socket_serveur == -1)
    {
        perror("socket_serveur");
        return 1;
    }

    // Mise en place bind
    struct sockaddr_in saddr ;
    saddr.sin_family = AF_INET ;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = INADDR_ANY;

    int bind_socket = bind(socket_serveur, (struct sockaddr *)&saddr, sizeof(saddr));

    if (bind_socket == -1)
    {
        perror("bind socker_serveur");
        return 1;
    }

    // Mise en place listen
    int listen_socket = listen(socket_serveur, 10);

    if (listen_socket == -1)
    {
        perror ("listen socket_serveur");
        return 1;
    }

    return socket_serveur;
}