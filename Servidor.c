#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>

int main(int argc, char *argv[])
{
	
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
	//Inicio
	//Abrir el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Error creant socket");
	}
	// Hacemos el bind al puerto
	memset(&serv_adr, 0, sizeof(serv_adr));
	// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	//Establecemos el puerto de escucha en el 50
	serv_adr.sin_port = htons(9050);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
	{
		printf ("Error al bind");
	}
	if (listen(sock_listen, 3) < 0)
	{
		printf("Error en el Listen");
	}
	int i;
	//Vector de sockets para conectar con varios clientes
	int sockets[100];
	//Thread para atender multiples clientes
	pthread_t thread;
	i=0;
	// Bucle para atender a 5 clientes
	for (;;)
	{
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		
		sockets[i] =sock_conn;
		//sock_conn es el socket que usaremos para este cliente
		
		// Crear thead y decirle lo que tiene que hacer
		
		pthread_create (&thread, NULL, AtenderCliente,&sockets[i]);
		i=i+1;	
	}		
}

void *AtenderCliente (void *socket)
{
	int sock_conn;
	int *s;
	s= (int *) socket;
	sock_conn= *s;
	char peticion[512];
	char respuesta[512];
	int ret;
	
	int terminar =0;
	// Entramos en un bucle para atender todas las peticiones de este cliente
	//Hasta que se desconecte
	while (terminar ==0)
	{
		// Ahora recibimos la peticion
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibido\n");
		
		// Tenemos que agregar la marca de fin de string 
		// para que no escriba lo que hay despues en el buffer
		peticion[ret]='\0';
		
		printf ("Peticion: %s\n",peticion);
		
		// Vemos la peticion del cliente
		char *p = strtok( peticion, "/");
		int codigo =  atoi (p);
		// Ya tenemos el codigo de la peticion
		char nombre[20];
		
		if (codigo !=0)
		{
			p = strtok( NULL, "/");
			
			strcpy (nombre, p);
			// Ya tenemos el nombre
			printf ("Codigo: %d, Nombre: %s\n", codigo, nombre);
		}
		//Petcion de desconexion
		if (codigo ==0) 
		{
			terminar=1;
		}
		else 
		{
			//Si pide la longitud del nombre
			if (codigo == 1)
			{
				sprintf (respuesta,"%d",strlen (nombre));
			}
		    else 
			{
				//Si piden saber si el nombre es bonito
				if (codigo == 2)
				{
					if((nombre[0]=='M') || (nombre[0]=='S'))
					{
					strcpy (respuesta,"SI");
					}
					else
					{
						strcpy (respuesta,"NO");
					}
				}
				//Por descarte piden saber si es alto
				else
				{
					p = strtok( NULL, "/");
					float altura =  atof (p);
					if (altura > 1.70)
					{
						sprintf (respuesta, "%s: eres alto",nombre);
					}
					else
					{
						sprintf (respuesta, "%s: eresbajo",nombre);
					}
				}
			}	
		}

		if (codigo !=0)
		{
			printf ("Respuesta: %s\n", respuesta);
			// Enviamos respuesta
			write (sock_conn,respuesta, strlen(respuesta));
		}
	}
	// Se acabo el servicio para este cliente
	close(sock_conn);
}
