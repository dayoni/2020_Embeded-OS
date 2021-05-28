/* 헤더 파일 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

/* 전처리문*/
#define SERV_IP		"220.149.128.103"
#define SERV_PORT	4568
#define BACKLOG		10
#define INIT_MSG	"=======================================\nHello! Da-Yeon & Sang-Chu Project #2...\nPlease, LOG-IN!\n---------------------------------------\n"
#define USER_INF	"===================================\nTRYING TO LOG-IN USER INFORMATION\n"

#define OK_LOG1		"Log-in success!! [user1] *^^*\n=======================================\n"
#define OK_LOG2		"Log-in success!! [user2] *^^*\n=======================================\n"
#define OK_LOG3     "Log-in success!! [user3] *^^*\n=======================================\n"

#define NK_ID		"Log-in fail: Incorrect ID... *o.o*\nAccess denied...\n=======================================\n"
#define NK_PW		"Log-in fail: Incorrect PW... *T.T*\nAccess denied...\n=======================================\n"
#define NK_IDPW     "Log-in fail: No data... *._.*\nAccess denied...\n=======================================\n"

#define USER1_ID	"user1"
#define USER2_ID	"user2"
#define USER3_ID	"user3"

#define USER1_PW	"passwd1"
#define USER2_PW	"passwd2"
#define USER3_PW	"passwd3"

#define BUF_SIZE	512
#define MAX_CLNT	100

/* 전역변수 */
char buf[BUF_SIZE];
char msg[BUF_SIZE];

int clnt_socks[MAX_CLNT];
int clnt_cnt = 0;

pthread_mutex_t mutx;

/* 함수 선언 */
void *clnt_sockets(void* arg);
void *clnt_login(void* arg);
void send_msg(char *msg, int num);

/* 메인함수 */
int main(int argc, char **argv) {
	// 소켓 생성 시 반환되는 파일 디스크럽터 이름
	int serv_fd, new_fd;
	int val = 1;

	// 주소를 저장할 구조체 변수, 클라이언트 주소 크기 저장할 변수
	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_addr_size;

	pthread_t clnt_logging;
	pthread_mutex_init(&mutx, NULL);


	// 소켓의 프로토콜을 설정하는 부분이므로 IPv4 프로토콜인 PF_INET으로 변경
	serv_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_fd == -1) {
		perror("Server-socket() error lol!");
		exit(1);
	}
	else printf("Server-socket() serv_fd is OK...\n");

	// serv_addr를 0으로 초기화 
	memset(&serv_addr, 0, sizeof(serv_addr));

	// IPv4 주소 형식으로 설정, 서버 PORT와 IP로 초기화
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 생성되는 소켓에 PORT와 IP 연결
	if (bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		perror("Server-bind() error lol!");
		exit(1);
	}
	else printf("Server-bind() is OK...\n");

	// 소켓 생성 후 연결요청 대기
	if (listen(serv_fd, BACKLOG == -1)) {
		perror("listen() error lol!");
		exit(1);
	}
	else printf("listen() is OK...\n");

	while (1) {
		clnt_addr_size = sizeof(clnt_addr);
		// 연결 요청 클라이언트 승인
		new_fd = accept(serv_fd, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

		// 클라이언트 연결소켓 생성 에러 시, 프로세스 종료
		if (new_fd == -1) {
			perror("accept() error lol!");
			exit(1);
		}
		else
			printf("accept() is OK...\n\n");

		// clnt_logging 쓰레드 생성, 핸들링 함수 : clnt_login, 받을 인자 : new_fd
		pthread_create(&clnt_logging, NULL, clnt_login, (void *)&new_fd);
		pthread_detach(clnt_logging);
	}
	close(serv_fd);
	return 0;
}

/* 쓰레드 핸들링 함수(1) : 클라이언트 로그인 */
void *clnt_login(void *arg) {
	int new_fd = *((int *)arg);
	int clnt_fd;

	int rcv_byte;
	int users_num;

	char id[20];
	char pw[20];

	pthread_t clnt_handling;

	// 연결된 소켓으로 로그인 요청 후 로그인 정보 수신
	send(new_fd, INIT_MSG, strlen(INIT_MSG) + 1, 0);
	rcv_byte = recv(new_fd, id, sizeof(id), 0);
	rcv_byte = recv(new_fd, pw, sizeof(pw), 0);

	// 수신 에러 발생 시 쓰레드 종료
	if (rcv_byte < 0) return NULL;

	// 수신받은 로그인 정보 출력
	printf("%s\n", USER_INF);
	printf("ID : %s, PW : %s\n", id, pw);

	// USER1 정상 로그인
	if ((strcmp(id, USER1_ID) == 0) && (strcmp(pw, USER1_PW) == 0)) {
		printf("%s\n", OK_LOG1);
		send(new_fd, OK_LOG1, strlen(OK_LOG1) + 1, 0);
		users_num = 1;
	}
	// USER2 정상 로그인
	else if ((strcmp(id, USER2_ID) == 0) && (strcmp(pw, USER2_PW) == 0)) {
		printf("%s\n", OK_LOG2);
		send(new_fd, OK_LOG2, strlen(OK_LOG2) + 1, 0);
		users_num = 2;
	}
	// USER3 정상 로그인
	else if ((strcmp(id, USER3_ID) == 0) && (strcmp(pw, USER3_PW) == 0)) {
		printf("%s\n", OK_LOG3);
		send(new_fd, OK_LOG3, strlen(OK_LOG3) + 1, 0);
		users_num = 3;
	}
	// 로그인 비정상 ① 비밀번호 틀린 경우
	else if (((strcmp(id, USER1_ID) == 0) && (strcmp(pw, USER1_PW) != 0)) || \
		((strcmp(id, USER2_ID) == 0) && (strcmp(pw, USER2_PW) != 0)) || \
		((strcmp(id, USER3_ID) == 0) && (strcmp(pw, USER3_PW) != 0))) {
		printf("%s\n", NK_PW);
		send(new_fd, NK_PW, strlen(NK_PW) + 1, 0);
		users_num = -1;
	}
	// 로그인 비정상 ② 아이디 틀린 경우
	else if (((strcmp(id, USER1_ID) != 0) && (strcmp(pw, USER1_PW) == 0)) || \
		((strcmp(id, USER2_ID) != 0) && (strcmp(pw, USER2_PW) == 0)) || \
		((strcmp(id, USER3_ID) != 0) && (strcmp(pw, USER3_PW) == 0))) {
		printf("%s\n", NK_ID);
		send(new_fd, NK_ID, strlen(NK_ID) + 1, 0);
		users_num = -1;
	}
	// 로그인 비정상 ③ 아이디, 비밀번호 둘 다 틀린 경우
	else {
		printf("%s\n", NK_IDPW);
		send(new_fd, NK_IDPW, strlen(NK_IDPW) + 1, 0);
		users_num = -1;
	}

	// 로그인이 정상적으로 진행된 경우,
	if (users_num > 0) {
		// 임계영역 ① clnt_fd, clnt_socks[] 자원 액세스
		pthread_mutex_lock(&mutx);
		clnt_fd = new_fd;
		clnt_socks[clnt_cnt++] = clnt_fd;
		pthread_mutex_unlock(&mutx);
		// 임계영역 ① 종료

		// clnt_handling 쓰레드 생성, 핸들링 함수 : clnt_sockets, 받을 인자 : clnt_fd
		pthread_create(&clnt_handling, NULL, clnt_sockets, (void *)&clnt_fd);
		pthread_detach(clnt_handling);
	}
	// 로그인이 비정상적으로 진행된 경우 쓰레드 종료
	else return NULL;

	return NULL;
}

/* 쓰레드 핸들링 함수(2) : 클라이언트 소켓 관리 */
void *clnt_sockets(void *arg) {
	int clnt_fd = *((int *)arg);
	int rcv_byte = 0;
	int server_exit = 1;
	int clnt_num;
	char msg[BUF_SIZE];

	// 클라이언트가 EOT를 수신할 때까지 읽음
	while(rcv_byte = read(clnt_fd, msg, sizeof(msg))!= 0) {
		for (int i = 0; i < clnt_cnt; i++) {
			if (clnt_socks[i] == clnt_fd) {
				clnt_num = i;
			}
		}
		send_msg(msg, clnt_num);
	}

	// 임계영역 ② clnt_socks[] 자원 액세스
	pthread_mutex_lock(&mutx);
	for (int i = 0; i < clnt_cnt; i++) {
		if (clnt_fd == clnt_socks[i]) {
			while (++i < clnt_cnt) {
				clnt_socks[i - 1] = clnt_socks[i];
			}
			break;
		}
	}
	clnt_cnt--;

	// 확인용 ###########
	printf("#### Total Client : %d#### \n\n", clnt_cnt);

	if (clnt_cnt == 0) server_exit = 0;
	pthread_mutex_unlock(&mutx);
	// 임계영역 ② 종료

	// 채팅시스템에 접속 유저가 0명이 되면 
	if (!server_exit) {
		printf("No users have been connected!!!\n\n");
		close(clnt_fd);
		exit(1);
	}
	close(clnt_fd);

	return NULL;
}

/* 자신을 제외한 클라이언트 메세지 전송하는 함수 */
void send_msg(char *msg, int num) {
	pthread_mutex_lock(&mutx);
	// 수신된 메세지 출력
	printf("%s", msg);

	// 자신보다 늦게 생성된 클라이언트 소켓으로 전송
	for (int i = 0; i < num; i++) {
		send(clnt_socks[i], msg, strlen(msg) + 1, 0);
	}
	// 자신보다 먼저 생성된 클라이언트 소켓으로 전송
	for (int j = clnt_cnt; j > num; j--) {
		send(clnt_socks[j], msg, strlen(msg) + 1, 0);
	}
	pthread_mutex_unlock(&mutx);
}
