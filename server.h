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
	const char *route;
	const char *file_path;
} Route;

typedef struct{
	int port;
	socket_t server_fd;
	int addrlen;
	struct sockaddr_in address;
	Route routes[MAX_ROUTES];
	int route_count;
} Server;


void init_server(Server *server, int port);
void start_server(Server *server);
void handle_client(Server *server, socket_t client_socket);
void add_route(Server *server, const char *route, const char *file_path);
void handle_route(Server *server, socket_t client_socket, const char *route);
void handle_route_not_found(socket_t client_socket);
void serve_file(socket_t client_socket, const char *file_path);
const char* get_mime_type(const char *file_path);
void logger(const char *method, const char *path);
void shutdown_server(Server *server);

#endif
