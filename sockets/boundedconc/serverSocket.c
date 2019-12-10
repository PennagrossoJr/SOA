 #include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAXB 10
int hijos = 0;

doServiceFork(int fd) {
    
	int i = fork();
	hijos = hijos + 1;
	if(i == 0) {
	    //printf("el hijo cominza  %d\n" , i);
		doService(fd);
		//printf("el hijo termina  %d\n" , i);
		//return 0;
        exit(0);
	}
	//printf("pid %d\n" , i);

}

doService(int fd) {

int i = 0;
char buff[80];
char buff2[80];
int ret;
int socket_fd = (int) fd;

	ret = read(socket_fd, buff, sizeof(buff));
	while(ret > 0) {
		buff[ret]='\0';
		sprintf(buff2, "Server [%d] received: %s\n", getpid(), buff);
		write(1, buff2, strlen(buff2));
		ret = write(fd, "caracola ", 8);
		if (ret < 0) {
			perror ("Error writing to socket");
			exit(1);
		}
		ret = read(socket_fd, buff, sizeof(buff));
	}
	if (ret < 0) {
			perror ("Error reading from socket");

	}
	sprintf(buff2, "Server [%d] ends service\n", getpid());
	write(1, buff2, strlen(buff2));

}


main (int argc, char *argv[])
{
  int socketFD;
  int connectionFD;
  char buffer[80];
  int ret;
  int port;


  if (argc != 2)
    {
      strcpy (buffer, "Usage: ServerSocket PortNumber\n");
      write (2, buffer, strlen (buffer));
      exit (1);
    }

  port = atoi(argv[1]);
  socketFD = createServerSocket (port);
  
  if (socketFD < 0)
    {
      perror ("Error creating socket\n");
      exit (1);
    }

  while (1) {
      //printf ("Recibo conexion \n");
	  if (hijos >= MAXB){
        //printf ("Hijos ejecutandose: %d \n", hijos);
        wait(NULL);
        hijos = hijos - 1;
        //printf ("Hijos ejecutandose despues del wait: %d \n", hijos);
      }
	
	  connectionFD = acceptNewConnections (socketFD);
	  if (connectionFD < 0)
	  {
		  perror ("Error establishing connection \n");
		  deleteSocket(socketFD);
		  exit (1);
	  }
	  //printf ("Esta disponible \n");
	  doServiceFork(connectionFD);
	  //printf ("Ocupados: %d\n", hijos);
  }

}
