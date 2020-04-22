#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

static int 	connect_to_proccess(int port, char *proccess)
{
	int					socket_fd, opt, new_socket;
	struct sockaddr_in	address;
	int					addrlen = sizeof(address);

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{ 
		perror("socket failed");
		exit(EXIT_FAILURE); 
	}

	opt = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	}

	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(port); 

	if (bind(socket_fd, (struct sockaddr *)&address,  sizeof(address)) < 0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	}

	size_t	len = strlen(proccess);
	char	tmp[5 + len];
	tmp[len + 4] = '\0';
	strcpy(tmp + 2, proccess);
	tmp[len + 3] = '&';
	tmp[len + 2] = ' ';
	tmp[0] = '.';
	tmp[1] = '/';
	system(tmp);

	if (listen(socket_fd, 1) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	}
	if ((new_socket = accept(socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	}
	return new_socket;
}

static void print_warning(time_t *counter, time_t *start_time)
{
	int					choice;
	printf("Execution takes to long\nDo you want:\n1)Continue;\n2)Continue with no future warnings;\n3)Stop?\n");
	while (scanf("%d", &choice) != 1 && (choice >= 1 && choice <= 3))
		printf("Incorrect value, choose 1,2 or 3\n");
	if (choice == 1)
		*start_time = time(NULL);
	else if (choice == 2)
		*start_time = 2147483647;
	else
	{
		printf("Process terminated due to too long calculation\n");
		exit(EXIT_FAILURE);
	}
}

int			main(int argc, char **argv)
{
	int					x, f_fd, g_fd, flags, read_res_f, read_res_g, res_f, res_g, len, counter;
	char				buf[1024];
	time_t				cur_time;
	time_t				start_time;

	if (argc != 3)
	{
		printf("Usage: ./laba <f_function> <g_function>\n");
		exit(EXIT_FAILURE);
	}

	printf("Enter x\n");
	while (scanf("%d", &x) != 1)
		printf("Incorrect value for x, try again\n");
	
	f_fd = connect_to_proccess(8080, argv[1]);
	g_fd = connect_to_proccess(8081, argv[2]);

	flags = fcntl(f_fd, F_GETFL, 0);
	if (fcntl(f_fd, F_SETFL, flags | O_NONBLOCK))
	{
		perror("non block");
		exit(EXIT_FAILURE);
	}
	flags = fcntl(g_fd, F_GETFL, 0);
	if (fcntl(g_fd, F_SETFL, flags | O_NONBLOCK))
	{
		perror("non block");
		exit(EXIT_FAILURE);
	}

	len = sprintf(buf, "%d", x);
	if (send(f_fd, buf, len, 0) == -1 || send(g_fd, buf, len, 0) == -1)
	{
		perror("send");
		exit(EXIT_FAILURE);
	}
	
	read_res_f = -1;
	read_res_g = -1;
	counter = 0;
	start_time = time(NULL);
	cur_time = time(NULL);
	//bzero(buf, 1024);
	while ((read_res_f = recv(f_fd, buf, 512, 0)) ||
			(read_res_g = recv(g_fd, buf + 512, 512, 0)))
		if (errno != EAGAIN)
		{
			perror("read");
			exit(EXIT_FAILURE);
		}
		else
		{
			cur_time = time(NULL);
			if (cur_time - start_time >= 10)
				print_warning(&cur_time, &start_time);
		}
	res_f = atoi(buf);
	res_g = atoi(buf + 512);
	printf("Result %d\n", res_f * res_g);
	return (0);
}