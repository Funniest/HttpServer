#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_SIZE 4096
//default file path
#define FILE_PATH "/home/user/study/socket/html"
#define PORT 9998
#define HOST "www.test.web"

int create_sock();
int accpet_sock();
int request_cut(char* request, char* path);
char* read_file(int file_fd);
void ErrorMessage(char *str);

int main(int argc, char *argv[]){
	char *header = (char *)malloc(MAX_SIZE * sizeof(char));
	char *request = (char *)malloc(MAX_SIZE * sizeof(char));
	char *path = (char *)malloc(MAX_SIZE * sizeof(char));
	char *new_path = (char *)malloc(MAX_SIZE * sizeof(char));
	char *file_buffer = NULL;

	char *http_not_found = "HTTP/1.1 404 Not Found\n";
	char *http_ok = "HTTP/1.1 200 OK\n";

	time_t time_now;
	struct tm* time_info;
	time(&time_now);
	time_info = localtime(&time_now);

	int server_fd, client_fd, file_fd;
	//test
	struct sockaddr_in sock_addr;
	int sock_len = sizeof(sock_addr);

	//create server socket
	server_fd = create_sock();

	while(1){
		printf("== I'm ready! ==\n");
		//accept client socket
		client_fd = accept(server_fd, (struct sockaddr*)&sock_addr, &sock_len);
		if(client_fd == -1)
			continue;
		printf("Accept!\n");
		//get client request packet
		recv(client_fd, request, MAX_SIZE, 0);
		//GET /XXX HTTP/1.1\r\n cut

		if(request_cut(request, path)){
			printf("deffrent http header!\n");
			close(client_fd);
			continue;
		}
		
		if(!strcmp(path,"/exit")){
			printf("Exit!\n");
			close(client_fd);
			break;
		}

		//system path + request path
		strcat(new_path, FILE_PATH);
		strcat(new_path, path);
		printf("request path : %s\n", new_path);
		//HTTP NOT FOUND Exception
		if((file_fd = fopen(new_path, "r")) == NULL){
			printf("File Not Foun!\n");
			send(client_fd, http_not_found, strlen(http_not_found), 0);
			printf("send ok\n");
		}
		//HTTP OK!
		else{
			printf("File Found!\n");
			//sned request packet
			file_buffer = read_file(file_fd);
			printf("%s\n", file_buffer);
			int file_len = strlen(file_buffer);
			printf("len : %d\n", file_len);
			//make packet
	                sprintf(header, "%sContent-Type: text/html; charset=\"utf-8\"\nContent-Length: %d\nServer: %s\nDate: %s\n",http_ok ,file_len,  HOST, asctime(time_info));
			write(client_fd, header, strlen(header));
			write(client_fd, file_buffer, file_len);
		}

		memset(header, 0, strlen(header));
		memset(path, 0, strlen(path));
		memset(new_path, 0, strlen(new_path));
		memset(request, 0, strlen(request));
		if(file_buffer != NULL){
			printf("Free!\n");
			memset(file_buffer, 0, strlen(file_buffer));
			free(file_buffer);
			file_buffer = NULL;
		}

		close(client_fd);
		printf("close\n");
	}

	free(header);
	free(request);
	free(path);
	free(new_path);
	close(server_fd);
	return 0;
}

void ErrorMessage(char *str){
	perror(str);
	exit(1);
}

//create server socket
int create_sock(){
	int sock_fd;
	struct sockaddr_in sock_addr;

	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		ErrorMessage("socket error");

	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = INADDR_ANY;
	sock_addr.sin_port = htons(PORT);

	if(bind(sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1)
		ErrorMessage("bind error");

	if(listen(sock_fd, 5) == -1)
		ErrorMessage("listen error");

	return sock_fd;
}

//accept client socket
int accept_sock(int server_fd){
	int sock_fd;
	struct sockaddr_in sock_addr;
	struct hostnet *host;
	struct in_addr addr;
	char host_ip[32] = {0,};

	int sock_len = sizeof(sock_addr);
	if((sock_fd = accept(server_fd, (struct sockaddr*)&sock_addr, &sock_len)) == -1)
		ErrorMessage("accept error");
	//get client ip
	inet_ntop(AF_INET, &(sock_addr.sin_addr), host_ip, &addr);
	inet_pton(AF_INET, host_ip, &addr);
	host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

	printf("Connect client IP[%s]\n", host_ip);

	return sock_fd;
}

//GET /XXX HTTP/1.1 cut
int request_cut(char* request, char* path){
	char get[4];
	char http[9];
	int check = 0;

	printf("== request check... ==\n");
	sscanf(request, "%s %s %8s", get, path, http);
	printf("GET : %s, HTTP : %s\n", get, http);
	if(strcmp(get, "GET") || strcmp(http, "HTTP/1.1"))
		return 1;

	printf("header check ok...\n");
	printf("path : %s\n", path);
	return 0;
}

//read file buffer
char* read_file(int file_fd){
	char* file_buffer = NULL;
	int file_size = 0;

	fseek(file_fd, 0L, SEEK_END);
	file_size = ftell(file_fd);

	rewind(file_fd);

	//get file size
	file_buffer = (char *)malloc(file_size * sizeof(char));

	file_size = 0;
	//get file data
	while(!feof(file_fd)){
		fread(file_buffer + file_size, sizeof(file_buffer), 1, file_fd);
		file_size = strlen(file_buffer);
	}

	close(file_fd);
	return file_buffer;
}
