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

/* 전역 변수 */
#define SERV_IP		"220.149.128.103"
#define SERV_PORT	4568
#define BUF_SIZE	512

#define NK_ID		"Log-in fail: Incorrect ID... *o.o*\nAccess denied...\n=======================================\n"
#define NK_PW		"Log-in fail: Incorrect PW... *T.T*\nAccess denied...\n=======================================\n"
#define NK_IDPW     "Log-in fail: No data... *._.*\nAccess denied...\n=======================================\n"
#define CAHT	   "< #### Chatting Start #### >\n"

char msg[BUF_SIZE];
char buf[BUF_SIZE];

char id[20];
char pw[20];
int rcv_byte;

/* 함수 선언 */
void *send_msg(void *arg);
void *recv_msg(void *arg);

/* 메인 함수 */
int main(int argc, char *agrv[])
{
	int serv_fd;
	char clean;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;

	// 소켓의 프로토콜을 설정하는 부분이므로 IPv4 프로토콜인 PF_INET으로 변경
	serv_fd = socket(PF_INET, SOCK_STREAM, 0);

	if (serv_fd == -1) {
		perror("Client-sockek() error lol!");
		exit(1);
	}
	else printf("Client-socket() serv_fd is OK...\n");

	// serv_addr를 0으로 초기화 
	memset(&serv_addr, 0, sizeof(serv_addr));

	// IPv4 주소 형식으로 설정, 서버 PORT와 IP로 초기화
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);

	// 초기화된 정보를 기반으로 연결요청, 에러 발생 시 프로세스 종료
	if (connect(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		perror("Client-connect() error lol!");
		exit(1);
	}
	else printf("Client-connect() is OK...\n\n");

	// serv_fd(연결된 소켓)로 부터 로그인 요청 메세지 수신
	rcv_byte = recv(serv_fd, buf, sizeof(buf), 0);
	printf("%s\n", buf);

	// 로그인 정보 입력
	printf("ID : ");
	scanf("%s", id);
	scanf("%c", &clean);	// 버퍼 초기화

	printf("PW : ");
	scanf("%s", pw);
	scanf("%c", &clean);	// 버퍼 초기화
	printf("---------------------------------\n");

	// 서버로 입력한 로그인 정보 전송
	send(serv_fd, id, strlen(id) + 1, 0);
	send(serv_fd, pw, strlen(pw) + 1, 0);

	// 로그인 결과 수신
	rcv_byte = recv(serv_fd, buf, sizeof(buf), 0);
	printf("%s\n\n", buf);

	// 로그인 정상 => 채팅 진행
	if (!((strcmp(buf, NK_ID) == 0) || (strcmp(buf, NK_PW) == 0) || (strcmp(buf, NK_IDPW) == 0))) {

		// 채팅시작 시, 화면 clear
		sleep(1);
		system("clear");
		printf("%s", CAHT);

		/* 메세지 송·수신 쓰레드 생성 및 실행 */

		// snd_thread 쓰레드 생성, 핸들링 함수 : send_msg, 받을 인자 : serv_fd
		pthread_create(&snd_thread, NULL, send_msg, (void *)&serv_fd);
		// rcv_thread 쓰레드 생성, 핸들링 함수 : recv_msg, 받을 인자 : serv_fd
		pthread_create(&rcv_thread, NULL, recv_msg, (void *)&serv_fd);

		// 메세지 송·수신 쓰레드 종료까지 대기
		pthread_join(snd_thread, NULL);
		pthread_join(rcv_thread, NULL);
	}
	// 로그인 비정상 => 접근 거부 출력 후 연결된 소켓 닫은 후 프로세스 종료
	else {
		printf("Access denied...");
		close(serv_fd);
		exit(1);
	}
	close(serv_fd);
	return 0;
}

/* 쓰레드 핸들링 함수(1) : 메세지 송신 */
void *send_msg(void *arg) {
	int serv_fd = *((int *)arg);
	char msg[BUF_SIZE];
	char send_buf[BUF_SIZE];

	while (1) {
		// 콘솔로부터 문자열 입력
		msg[0] = '\0';
		fgets(msg, BUF_SIZE, stdin);

		// [EXIT] 또는 /exit 입력 시 연결된 소켓 닫은 후 프로세스 종료
		if (!strcmp(msg, "[EXIT]\n") || !strcmp(msg, "/exit\n")) {
			close(serv_fd);
			exit(1);
		}
		else {
			// send_buf <= "[USERn] : msg", [USERn] Color : yellow 
			sprintf(send_buf, "\x1b[33m[%s]\x1b[0m : %s", id, msg);
			send(serv_fd, send_buf, strlen(send_buf) + 1, 0);
		}
	}
	return NULL;
}

/* 쓰레드 핸들링 함수(2) : 메세지 수신 */
void *recv_msg(void *arg) {
	int serv_fd = *((int *)arg);
	int rcv_byte;
	char msg[BUF_SIZE];

	while (1) {
		// 연결된 소켓으로 부터 메세지 수신
		rcv_byte = recv(serv_fd, msg, sizeof(msg), 0);

		// 수신 에러 발생 시 쓰레드 종료
		if(rcv_byte < 0) return NULL;

		// 수신받은 메세지 출력
		printf("%s", msg);
	}
	return NULL;
}