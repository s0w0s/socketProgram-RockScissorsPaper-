#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXLINE 1024
char win[MAXLINE] = "당신이 이겼습니다.\n";
char lose[MAXLINE] ="안타깝게도 졌네요 \n";
char draw[MAXLINE] = "비겼네요\n";
char check[MAXLINE] ="입력값을 다시 확인해주세요\n";
	
int who_winer(int, int);
void send_result(int, int);

int main(int argc, char **argv)
{
	int server_sockfd, client_sockfd;
	int client_num, server_num, winner;
	int state, client_len;
	pid_t pid;
	
	FILE *fp;
	struct sockaddr_un clientaddr, serveraddr;
	
	char buf[MAXLINE];
	srand(time(NULL));
	
	if(argc != 2) //입력 값이 2개인지 확인
	{
		printf("Usage : %s [file name]\n", argv[0]);
		printf("예 : %s /tmp/mysocket\n", argv[0]);
		exit(0);
	}
	
	if(access(argv[1], F_OK) == 0)
	{
		unlink(argv[1]);
	}
		
	// 주소 사이즈 
	client_len = sizeof(clientaddr);
	if ((server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) <0) //서버 소켓 생성
	{
		perror("socket error : ");
		exit(0);
	}	
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sun_family = AF_UNIX; // ipv4 인터넷 프로토콜
	strcpy(serveraddr.sun_path, argv[1]);
	
	//ex) bind(ip,80) 클라이언트가 서버의 위치를 알기 위한 ip와 port등록 함수
	state = bind(server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(state == -1)
	{
		perror("bind error: ");
		exit(0);
	}
	
	printf("waiting for client connection...\n");
	// listen()함수를 통해 클라이언트의 connect() 함수의 접속 요청을 '확인'하도록 요청한다. 확인되어진 요청에 대한 처리 accept()함수에게 넘김
	state = listen(server_sockfd, 5);
	if(state ==-1)
	{
			perror("listen error : ");
			exit(0);
	}
	
	while(1)
	{
		// 클라이언트 요청 처리 소켓생성
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr, &client_len);
		printf("connected complet \n client%d hello ~~ \n" ,client_sockfd);
		pid = fork(); // 프로세스 복사본 생성 
		if(pid ==0)
		{
			if(client_sockfd == -1)
			{
				perror("Accept error : ");
				exit(0);
			}
			
			while(1)
			{
				server_num = (rand() % 3) +1; //random 1~3
				
				memset(buf, 0x00, MAXLINE);
				if(read(client_sockfd, buf, MAXLINE) <= 0) // 클라이언트로부터 입력정보 받음
				{
					close(client_sockfd); 
					exit(0);
				}
				
				printf("client%d :%s vs server: %d \n" ,client_sockfd, buf, server_num);
				
				client_num = atoi(buf);
				if(client_num == 1 || client_num == 2 || client_num == 3)  {
					winner = who_winer(server_num, client_num); //서버 입력값, 클라인언트 입력값
					send_result(winner, client_sockfd);					
					
				} else {
					write(client_sockfd, check, strlen(check));
				}				
						
			}
		}
	}
	close(client_sockfd);	
}

// 서버와 클라이언트 가위바위보 승자 확인 함수
int who_winer(int server, int client){ 
	if(server==client) return 0;
	else if(server%3 == (client+1) %3) return 1;
	else return -1;
	
} 

// 결과 값과 소켓을 입력받아 클라이언트에게 결과 전송
void send_result(int result, int client_sockfd){
	if(result==1){
		write(client_sockfd, win, strlen(win));
	}else if(result == -1){
		write(client_sockfd, lose, strlen(lose));
	}else {
		write(client_sockfd, draw, strlen(draw));
	}
	
}
