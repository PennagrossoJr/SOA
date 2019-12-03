#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


// Create a socket and initialize it to be able to accept 
// connections.
// It returns the virtual device associated to the socket that should be used 
// in the accept system call, to get the virtual device associated to 
// the connection
//

int
createServerSocket (int port)
{
	//write(1,"dd",2);
	//int fd = clientConnection("0.0.0.0",port);
    
    // SocketCreate
    int fd = socket (AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        write(1,"Error al crear el socket!!!",50);
        return fd;
    }
    
    // BindCreatedSocket
    struct sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    
    /* Internet address family */
    serv_addr.sin_family = AF_INET;
    
    /* Any incoming interface */
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    
    int iRetval = bind(fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
    
    if (iRetval < 0) {
        write(1,"Error al crear el bind!!!",50);
        return fd;
    }
    
    //Listen
    listen(fd, 5); //numero de conexiones que se permite
    //write(1,"Escuchando...",10);
    printf("Escuchando...\n");
    return fd;
}


// Returns the file descriptor associated to the connection.
// accept system call will fill the socketAddr parameter
// with the address of the socket for the client which is requesting the
// connection, and the addrSize parameter with the size of that address.

int
acceptNewConnections (int socket_fd)
{
	//return accept( socket_fd, (struct sockaddr *) AF_INET, sizeof(AF_INET));
    
     //accept connection from an incoming client
     int clientLen = sizeof(struct sockaddr_in);
     struct sockaddr_in client;
     int sock = accept(socket_fd,&client,&clientLen);
     if (sock < 0) {
        //write(1,"no se acepta conexiones",50);
        printf("no se acepta conexiones\n");
        return 1;
    }
    else {
       //write(1,"se acepta conexiones",50);
       printf("se acepta conexiones\n");
       return sock; 
    }
}

// Returns the socket virtual device that the client should use to access 
// the socket, if the connection is successfully established 
// and -1 in case of error
//
// Connect system call requires the address of the 
// server socket to request the connection and the size of that address.
//

int
clientConnection (char *host_name, int port)
{
  printf("value of sc : %s \n", host_name);
  struct sockaddr_in serv_addr;
  struct hostent * hent;
  int socket_fd;
  int ret;

  //creates the virtual device for accessing the socket
  socket_fd = socket (AF_INET, SOCK_STREAM, 0);
  
  if (socket_fd < 0)
    return socket_fd;

  memset((char *) &serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  hent = gethostbyname(host_name);
  if (hent == NULL) {
        close (socket_fd);
	return -1;

  }
  memcpy((char *)&serv_addr.sin_addr.s_addr, (char *) hent->h_addr, hent->h_length);
  serv_addr.sin_port = htons(port);
  serv_addr.sin_family = PF_INET; 
  serv_addr.sin_addr.s_addr = INADDR_ANY; 
  ret = connect (socket_fd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));
  if (ret < 0)
  {

	  close (socket_fd);
	  return (ret);
  } 

  return socket_fd;

}


int deleteSocket (int socket_fd)
{
  close (socket_fd);
}

