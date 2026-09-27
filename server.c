#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

Route routes[MAX_ROUTES];
int route_count = 0;
time_t raw;
struct tm * timeinfo;

void init_server(Server *server, int port){
	if(socketinit() != 0){
		perror("Socket initialization failed");
		exit(1);
	}

	server->port = port;
	server->addrlen = sizeof(server->address);
	
	if((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	if(setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0){
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}
	
	server->address.sin_family = AF_INET;
	server->address.sin_addr.s_addr = INADDR_ANY;
	server->address.sin_port = htons(port);
	
	if(bind(server->server_fd, (struct sockaddr*)&server->address, server->addrlen) < 0){
		perror("binding failed");
		exit(EXIT_FAILURE);
	}
}

void start_server(Server *server){
	if(listen(server->server_fd, 5) < 0){
		perror("listening failed");
		exit(EXIT_FAILURE);
	}
	
	printf("Server is listening on Port %d\n", server->port);
	
	socket_t new_socket;
	while((new_socket = accept(server->server_fd, (struct sockaddr*)&server->address, (socklen_t*)&server->addrlen)) != INVALID_SOCKET){
		handle_client(new_socket);
	}
	
	if(new_socket == INVALID_SOCKET){
		perror("accept failed");
		exit(EXIT_FAILURE);
	}
	
}

void handle_client(socket_t client_socket){
	char buffer[BUFFER_SIZE] = {0};
	int valread = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
	if(valread < 0){
		perror("recv failed");
		exit(EXIT_FAILURE);
	}
	
	char method[METHOD_BUFFER_SIZE] = {0}, path[PATH_BUFFER_SIZE] = {0};
	sscanf(buffer, "%s %s", method, path);
	logger(method, path);
	
	if(strcmp(method, "GET") == 0){
		handle_route(client_socket, path);
	}else{
		handle_route_not_found(client_socket);
	}
	
	closesocket(client_socket);
}

void add_route(char *route, char *file_path){
	if(route_count < MAX_ROUTES){
		routes[route_count].route = route;
		routes[route_count].file_path = file_path;
		route_count++;
	}
}

void handle_route(socket_t client_socket, char *route){
	for(int i=0; i<route_count; i++){
		if(strcmp(route, routes[i].route) == 0){
			serve_file(client_socket, routes[i].file_path);
			return;
		}
	}
	handle_route_not_found(client_socket);
}

void handle_route_not_found(socket_t client_socket){
	char *resp = "HTTP/1.1 404 Not Found\r\n"
				  "Content-Length: 13\r\n\r\n"
				  "404 Not Found";
	send(client_socket, resp, strlen(resp), 0);
}

void serve_file(socket_t client_socket, char *file_path){
	int file = open(file_path, O_RDONLY | O_BINARY);
	
	if(file < 0){
		handle_route_not_found(client_socket);
		return;
	}
	
	struct stat file_stat;
	fstat(file, &file_stat);
	
	char response_header[BUFFER_SIZE];
	snprintf(response_header, BUFFER_SIZE, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n", file_stat.st_size, get_mime_type(file_path));
	send(client_socket, response_header, strlen(response_header), 0);
	
	char file_buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	
	while((bytes_read = read(file, file_buffer, BUFFER_SIZE)) > 0){
		send(client_socket, file_buffer, bytes_read, 0);
	}
	
	close(file);
}

char* get_mime_type(char *file_path){
	char *ext = strrchr(file_path, '.');
	if(!ext) return "text/plain";
	if(strcmp(ext, ".html") == 0) return "text/html";
	if(strcmp(ext, ".css") == 0) return "text/css";
	if(strcmp(ext, ".js") == 0) return "application/javascript";
	if(strcmp(ext, ".json") == 0) return "application/json";
	if(strcmp(ext, ".png") == 0) return "image/png";
	if(strcmp(ext, ".jpg") == 0) return "image/jpeg";
	if(strcmp(ext, ".gif") == 0) return "image/gif";
	return "text/plain";
}

void logger(char *method, char *path){
	char buffer[TIME_BUFFER_SIZE];
	time(&raw);
	timeinfo = localtime(&raw);
	
	strftime(buffer, TIME_BUFFER_SIZE, "[%a %b %d %H:%M:%S %Y]", timeinfo);
	printf("%s %s %s\n", buffer, method, path);
}

void shutdown_server(Server *server){
	closesocket(server->server_fd);
	server->server_fd = INVALID_SOCKET;
	socketcleanup();
}
