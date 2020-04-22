#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define PORT 8080

int 	main(void)
{
	int					sock, read_res, x, len; 
	struct sockaddr_in	serv_addr;
	char				buf[1024];

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		exit(EXIT_FAILURE); 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	   
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)  
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		exit(EXIT_FAILURE); 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		perror("connect failed"); 
		exit(EXIT_FAILURE); 
	}

	while ((read_res = read(sock, buf, 1024)) == -1)
		if (errno != EAGAIN)
		{
			perror("read");
			exit(EXIT_FAILURE);
		}
	x = atoi(buf);
	x += 27;

	len = sprintf(buf, "%d", x);
	if (send(sock, buf, len, 0) == -1)
	{
		perror("send");
		exit(EXIT_FAILURE);
	}
	return (0);
}