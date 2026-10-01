#include "cerver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


void cerver_init(Cerver *server, int port){
	if(socketinit() != 0){
		perror("Socket initialization failed");
		exit(1);
	}

	server->route_count = 0;
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

void cerver_start(Cerver *server){
	if(listen(server->server_fd, 5) < 0){
		perror("listening failed");
		exit(EXIT_FAILURE);
	}
	
	printf("Server is listening on Port %d\n", server->port);
	
	socket_t new_socket;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	while((new_socket = accept(server->server_fd, (struct sockaddr*)&client_addr, &client_len)) != INVALID_SOCKET){
		cerver_handle_client(server, new_socket);
		client_len = sizeof(client_addr);
	}
	
	perror("accept failed");
}

void cerver_handle_client(Cerver *server, socket_t client_socket){
	char buffer[BUFFER_SIZE] = {0};
	int valread = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
	if(valread < 0){
		perror("recv failed");
		closesocket(client_socket);
		return;
	}
	
	char method[METHOD_BUFFER_SIZE] = {0}, path[PATH_BUFFER_SIZE] = {0};
	if(sscanf(buffer, "%" STR(METHOD_LENGTH) "s %" STR(PATH_LENGTH) "s", method, path) != 2){
		cerver_handle_route_not_found(client_socket);
		closesocket(client_socket);
		return;
	}

	cerver_logger(method, path);
	
	if(strcmp(method, "GET") == 0){
		cerver_handle_route(server, client_socket, path);
	}else{
		cerver_handle_route_not_found(client_socket);
	}
	
	closesocket(client_socket);
}

void cerver_add_route(Cerver *server, const char *route, const char *file_path){
	if(server->route_count < MAX_ROUTES){
		server->routes[server->route_count].route = route;
		server->routes[server->route_count].file_path = file_path;
		server->route_count++;
	}
}

void cerver_handle_route(Cerver *server, socket_t client_socket, const char *route){
	for(int i=0; i<server->route_count; i++){
		if(strcmp(route, server->routes[i].route) == 0){
			cerver_serve_file(client_socket, server->routes[i].file_path);
			return;
		}
	}
	cerver_handle_route_not_found(client_socket);
}

void cerver_handle_route_not_found(socket_t client_socket){
	char *resp = "HTTP/1.1 404 Not Found\r\n"
				  "Content-Length: 13\r\n\r\n"
				  "404 Not Found";
	send(client_socket, resp, strlen(resp), 0);
}

void cerver_serve_file(socket_t client_socket, const char *file_path){
	int file = open(file_path, O_RDONLY | O_BINARY);
	
	if(file < 0){
		cerver_handle_route_not_found(client_socket);
		return;
	}
	
	struct stat file_stat;
	fstat(file, &file_stat);
	
	char response_header[BUFFER_SIZE];
	snprintf(response_header, BUFFER_SIZE, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n", file_stat.st_size, cerver_get_mime_type(file_path));
	send(client_socket, response_header, strlen(response_header), 0);
	
	char file_buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	
	while((bytes_read = read(file, file_buffer, BUFFER_SIZE)) > 0){
		send(client_socket, file_buffer, bytes_read, 0);
	}
	
	close(file);
}

const char* cerver_get_mime_type(const char *file_path){
	const char *ext = strrchr(file_path, '.');
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

void cerver_logger(const char *method, const char *path){
	time_t raw;
	time(&raw);
	struct tm * timeinfo = localtime(&raw);
	
	char buffer[TIME_BUFFER_SIZE];
	strftime(buffer, TIME_BUFFER_SIZE, "[%a %b %d %H:%M:%S %Y]", timeinfo);
	printf("%s %s %s\n", buffer, method, path);
}

void cerver_shutdown(Cerver *server){
	closesocket(server->server_fd);
	server->server_fd = INVALID_SOCKET;
	socketcleanup();
}
