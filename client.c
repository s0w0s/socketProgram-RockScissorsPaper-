#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 1024

void main(int argc, char **argv)
{
	int client_len;
	int client_sockfd;
	int server_sockfd;
	
	FILE *fp_in;
	char buf_in[MAXLINE];
	char buf_get[MAXLINE];
	
	struct sockaddr_un clientaddr;
	if(argc != 2)
	{
		printf("Usage : %s [file name]\n", argv[0]);
		printf("yes : %s /tmp/mysocket\n", argv[0]);
		exit(0);
	}
	
	// af_unix  호스트에 존재하는 프로세스 사이의 통신을 지원 (소켓을 생성)
	client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(client_sockfd == -1) 
	{
		perror("error : ");
		exit(0);
	}
	
	bzero(&clientaddr, sizeof(clientaddr)); // 메모리 공간을 size 바이트만큼 0 으로 채운다
	clientaddr.sun_family = AF_UNIX;
	strcpy(clientaddr.sun_path, argv[1]);
	client_len = sizeof(clientaddr);
	// 서버와 연결시도
	if (connect(client_sockfd, (struct sockaddr *)&clientaddr, client_len) < 0)
	{
		perror("Connect erro:");
		exit(0);
	}
	while(1)
	{
		memset(buf_in, 0x00, MAXLINE); //메모리 buf_in을 size 만큼 value 값으로 채우고 buf_in 주소를 반환한다.
		memset(buf_get, 0x00, MAXLINE);
		printf("pick number 1~3 [Rock:1 Scissors:2 Paper:3] ->");
		scanf("%s", buf_in);
		write(client_sockfd, buf_in, strlen(buf_in)); // 입력 받은값 서버에 전송
		
		read(client_sockfd, buf_get, MAXLINE); // 서버로 부터 결과값 읽음
		printf("%s\n", buf_get);
	}
	close(client_sockfd); //close
	exit(0);
}
