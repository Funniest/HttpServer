# Http Server
### Http Server
Http server는 웹 클라이언트가 웹 페이지를 요청하면 Http Server는 요청에 따라HTML문서를 전달합니다.

또는 클라이언트로 부터 파일 등을 전달 받습니다.

Http server를 간단히 그림으로 표현해보았습니다.

아래 그림처럼 Client가 Server에게 요청하면, 서버는 Request로 요청한 문서를 보내 줍니다.

![Alt text](https://github.com/Funniest/HttpServer/blob/master/img/HTTP_SERVER.PNG)

### Http Protocol
위 그림에서 간략하게 설명되었지만, 조금 더 자세하게 알아봅시다.

Http패킷은 위의 그림처럼 요청과 요청에 따른 응답이 있습니다.

이에 대한 패킷 구조는 Http header, Http content를포함합니다.

Http header의 특징은 \r\n으로 줄이 구분된단는 것이 특징입니다.

위 조그마한 글자를 보면, 요청을 하는 패킷이 나와있는데, 대부분 아래와 같은 느낌입니다.

```
GET / HTTP/1.1
Host: www.naver.com
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Accept-Language: ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4
Cookie: test
Connection: close 
```
요청을 하는 패킷은 아래 처럼 생겼습니다.

```
HTTP/1.1 200 OK
Content-Length: 3612
Content-Type: text/xml; charset="utf-8"
Server: Microsoft-Windows-NT/5.1 UPnP/1.0 UPnP-Device-Host/1.0 Microsoft-HTTPAPI/2.0
Date: Thu, 09 Mar 2017 10:30:02 GMT
Connection: close

(HTML Content...)
```

이제 위 패킷을 조금 분석해서 Simple HTTP Server를 제작하여 보겠습니다.

### 소스코드

Server는 Ubuntu 32bit nano에디터로 작성되었습니다.

#### Server
```
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
	printf("test1\n");
	if((sock_fd = accept(server_fd, (struct sockaddr*)&sock_addr, &sock_len)) == -1)
		ErrorMessage("accept error");
	printf("test2\n");
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
```
