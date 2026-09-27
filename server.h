#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>

	typedef SOCKET socket_t;
	static inline int socketinit(void) {
		WSADATA wsa;
		return WSAStartup(MAKEWORD(2, 2), &wsa);
	}
	#define socketcleanup() WSACleanup()

#else
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>

	typedef int socket_t;	
	#define INVALID_SOCKET (-1)
	#define socketinit() 0
	#define socketcleanup() ((void)0)
	#define closesocket close
	#define O_BINARY 0

#endif

#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)

#define METHOD_LENGTH 19
#define METHOD_BUFFER_SIZE (METHOD_LENGTH + 1)

#define PATH_LENGTH 99
#define PATH_BUFFER_SIZE (PATH_LENGTH + 1)

#define BUFFER_SIZE  1024
#define MAX_ROUTES 20
#define TIME_BUFFER_SIZE 80

typedef struct{
	int port;
	socket_t server_fd;
	int addrlen;
	struct sockaddr_in address;
} Server;

typedef struct{
	char *route;
	char *file_path;
} Route;


void init_server(Server *server, int port);
void start_server(Server *server);
void handle_client(socket_t client_socket);
void add_route(char *route, char *file_path);
void handle_route(socket_t client_socket, char *route);
void handle_route_not_found(socket_t client_socket);
void serve_file(socket_t client_socket, char *file_path);
char* get_mime_type(char *file_path);
void logger(char *method, char *path);
void shutdown_server(Server *server);

#endif
