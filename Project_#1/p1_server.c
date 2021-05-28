#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_IP "220.149.128.103"
#define SERV_PORT 4568
#define BACKLOG 10

#define INIT_MSG	"=================================\nHello! IM dayoni_Termproject1... \nPlease, LOG-IN!\n---------------------------------\n"
#define USER_INF	"=================================\nTRYING TO LOG-IN USER INFORMATION\n"
#define OK_LOG1		"Log-in success!! [user1] *^^*\n=================================\n"
#define OK_LOG2		"Log-in success!! [user2] *^^*\n=================================\n"
#define NK_PW		"Log-in fail: Incorrect password... *T.T*\n=================================\n"
#define NK_ID		"Log-in fail: Who... Are....Yo...u...? *o.o*\n=================================\n"
#define NK_IDPW		"Log-in fail: No data... *._.*\n=================================\n"
#define USER1_ID	"user1"
#define USER1_PW	"passwd1"
#define USER2_ID	"user2"
#define USER2_PW	"passwd2"

int main(void)
{
	/* listen on sock_fd, new connection on new_fd */
	int serv_fd, clnt_fd;

	/*my address information, address where I run this program */
	struct sockaddr_in my_addr;

	/* remote address information */
	struct sockaddr_in their_addr;
	unsigned int sin_size;

	/* buffer */
	int rcv_byte;
	char buf[512];

	char id[20];
	char pw[20];

	char msg[512];

	int val = 1;

	pid_t pid;

	/* socket */
	serv_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_fd == -1) {
		perror("Server-socket() error lol!");
		exit(1);
	}
	else printf("Server-socket() serv_fd is OK...\n");

	/* host byte order */
	my_addr.sin_family = AF_INET;

	/* short, network byte order */
	my_addr.sin_port = htons(SERV_PORT);

	/* my_addr.sin_addr.s_addr = inet_addr(SERV_IP); */
	my_addr.sin_addr.s_addr = INADDR_ANY;

	/* zero the rest of struct */
	memset(&(my_addr.sin_zero), 0, 8);

	/* to prevent 'Address already in use...' */
	if(setsockopt(serv_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val)) < 0){
		perror("setsockpot");
		close(serv_fd);
		return -1;
	}

	/* bind */
	if(bind(serv_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Server-bind() error lol!");
		exit(1);
	}
	else printf("Server-bind() is OK...\n");

	/* listen */
	if(listen(serv_fd, BACKLOG == -1))
	{
		perror("listen() error lol!");
		exit(1);
	}
	else printf("listen() is OK...\n");

	while(1) {
		/* ...other codes to read rhe received data... */
		sin_size = sizeof(struct sockaddr_in);
		clnt_fd = accept(serv_fd, (struct sockaddr *)&their_addr, &sin_size);

		if (clnt_fd == -1) {
			perror("accept() error lol!");
			exit(1);
		}
		else 
			printf("accept() is OK...\n\n");

		pid = fork();

		/* 자식 프로세스 */
		if (pid == 0) {
			send(clnt_fd, INIT_MSG, strlen(INIT_MSG) + 1, 0);
			rcv_byte = recv(clnt_fd, id, sizeof(id), 0);
			rcv_byte = recv(clnt_fd, pw, sizeof(pw), 0);

			printf("%s\n", USER_INF);
			printf("ID : %s, PW : %s\n", id, pw);

			// USER1 정상 로그인
			if ((strcmp(id,USER1_ID) == 0) && (strcmp(pw,USER1_PW) == 0)) {
				printf("%s\n", OK_LOG1);
				send(clnt_fd, OK_LOG1, strlen(OK_LOG1) + 1, 0);
			}
			// USER2 정상 로그인
			else if ((strcmp(id, USER2_ID) ==0) && (strcmp(pw, USER2_PW) == 0)) {
				printf("%s\n", OK_LOG2);
				send(clnt_fd, OK_LOG2, strlen(OK_LOG2) + 1, 0);
			}
			// 비밀번호 틀린 경우
			else if (((strcmp(id, USER1_ID) == 0) && (strcmp(pw, USER1_PW) != 0)) ||\
				((strcmp(id, USER2_ID) == 0) && (strcmp(pw, USER2_PW) != 0))) {
				printf("%s\n", NK_PW);
				send(clnt_fd, NK_PW, strlen(NK_PW) + 1, 0);
			}
			// 아이디가 틀린 경우
			else if (((strcmp(id, USER1_ID) != 0) && (strcmp(pw, USER1_PW) == 0)) ||\
				((strcmp(id, USER2_ID) != 0) && (strcmp(pw, USER2_PW) == 0))) {
				printf("%s\n", NK_ID);
				send(clnt_fd, NK_ID, strlen(NK_ID) + 1, 0);
			}
			// 아이디, 비밀번호 둘 다 틀린 경우
			else {
				printf("%s\n", NK_IDPW);
				send(clnt_fd, NK_IDPW, strlen(NK_IDPW) + 1, 0);
			}
			close(clnt_fd);
		}
		/* 부모 프로세스 */
		else if (pid > 0) {
			close(clnt_fd);
		}
		/* fork 에러 */
		else {
			perror("fork() error lol!");
			exit(1);
		}
		close(clnt_fd);
	}
	close(serv_fd);

	return 0;
}
