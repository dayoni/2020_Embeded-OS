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
#include <dirent.h>

/* 전처리문 */
#define SERV_IP			"220.149.128.103"
#define MY_IP			"220.149.128.101"
#define SERV_PORT		4560

#define MY_PORT			4561
#define MY_PORT_CHAR	"4561"
#define BUF_SIZE		512
#define BACKLOG			10

#define NK_ID			"Log-in fail: Incorrect ID... *o.o*\nAccess denied...\n=======================================\n"
#define NK_PW			"Log-in fail: Incorrect PW... *T.T*\nAccess denied...\n=======================================\n"
#define NK_IDPW			"Log-in fail: No data... *._.*\nAccess denied...\n=======================================\n"
#define CAHT			"< #### Chatting Start #### >\n"

/* 전역변수 */
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
	scanf("%c", &clean);   // 버퍼 초기화

	printf("PW : ");
	scanf("%s", pw);
	scanf("%c", &clean);   // 버퍼 초기화
	printf("---------------------------------\n");

	// 서버로 입력한 로그인 정보 전송
	send(serv_fd, id, strlen(id) + 1, 0);
	send(serv_fd, pw, strlen(pw) + 1, 0);

	sleep(1);

	send(serv_fd, MY_IP, strlen(MY_IP) + 1, 0);
	send(serv_fd, MY_PORT_CHAR, strlen(MY_PORT_CHAR) + 1, 0);
	
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

		// 입력한 메세지가 "[FILE]" -> P2P 파일 전송 모드 시작
		else if (strcmp(msg, "[FILE]\n") == 0)
		{
			// 서버에 [FILE]이라고 전송
			sprintf(send_buf, "\x1b[33m[%s]\x1b[0m : %s", id, msg);
			send(serv_fd, send_buf, strlen(send_buf) + 1, 0);

			// P2P 통신을 위한 소켓 생성 및 연결 시작
			printf("\n=============== Welcome P2P file Transfer Mode ===============\n");
			char file_name[100][100];
			char selec_buf[20];
			char file_pos[100];
			char file_text[100];
			char clean;

			int select = 0;
			int my_serv_fd, my_new_fd;
			struct sockaddr_in my_serv_addr, my_clnt_addr;

			socklen_t my_clnt_addr_size;

			my_serv_fd = socket(PF_INET, SOCK_STREAM, 0);

			memset(&my_serv_addr, 0, sizeof(my_serv_addr));

			my_serv_addr.sin_family = AF_INET;
			my_serv_addr.sin_port = htons(MY_PORT);
			my_serv_addr.sin_addr.s_addr = inet_addr(MY_IP);

			if (bind(my_serv_fd, (struct sockaddr *)&my_serv_addr, sizeof(my_serv_addr)) == -1) {
				perror("Server-bind() error lol!");
				exit(1);
			}
			else printf("Server-bind() is OK...\n");

			// 소켓 생성 후 연결요청 대기
			if (listen(my_serv_fd, BACKLOG == -1)) {
				perror("listen() error lol!");
				exit(1);
			}
			else printf("listen() is OK...\n");

			my_clnt_addr_size = sizeof(my_clnt_addr);
			// 연결 요청 클라이언트 승인
			my_new_fd = accept(my_serv_fd, (struct sockaddr *)&my_clnt_addr, &my_clnt_addr_size);

			// 클라이언트 연결소켓 생성 에러 시, 프로세스 종료
			if (my_new_fd == -1) {
				perror("accept() error lol!");
				exit(1);
			}
			else
				printf("accept() is OK...\n\n");

			// 파일이 몇개인지 받아오는부분 --> for문 돌릴때 파일의 수만큼 돌리기 위해서 값 저장
			rcv_byte = recv(my_new_fd, msg, sizeof(msg), 0);
			int file_num = atoi(msg);
			printf("<<  file list  >>\n");

			// 파일의 수만큼 파일이름 받아와서 배열에 저장 (파일 리스트 저장 및 출력)
			for (int i = 0; i < file_num; i++)
			{
				rcv_byte = recv(my_new_fd, file_name[i], sizeof(file_name[i]), 0);
				printf("%d. %s\n", i + 1, file_name[i]);
			}

			// 다운로드 받을 파일 번호 지정
			printf("\nselect the file number\n>> ");
			scanf("%d", &select);
			scanf("%c", &clean);

			// 번호 입력 오류 시 재입력 요청
			while (select<1 || select>file_num)
			{
				printf("re select please\n>> ");
				scanf("%d", &select);
				scanf("%c", &clean);
			}

			// int 형으로 수신된 메세지 문자열로 변환
			sprintf(selec_buf, "%d", select);
			send(my_new_fd, send_buf, strlen(send_buf) + 1, 0);

			// 파일 내용 받아와서 콘솔화면에 출력
			rcv_byte = recv(my_new_fd, file_text, sizeof(file_text), 0);
			printf("\n========== text ==========\n");
			printf("%s", file_text);
			printf("==========================\n");

			// 파일 저장
			sprintf(file_pos, "/home/st2018146015/project3/%s", file_name[select - 1]);
			FILE* savefile;
			savefile = fopen(file_pos, "w");

			// 위에서 저장한 리스트에서 선택한 파일이름으로 파일 만들고 "w" 쓰기모드로 파일 오픈
			fputs(file_text, savefile);

			// 받아온 파일 내용 저장
			printf("<<  save complete!!  >>\n");
			fclose(savefile);
			printf("\n=============== Restart Group Chatting Mode ===============\n");
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
	char file_name[BUF_SIZE];

	while (1) {
		// 연결된 소켓으로 부터 메세지 수신
		rcv_byte = recv(serv_fd, msg, sizeof(msg), 0);

		// 수신 에러 발생 시 쓰레드 종료
		if (rcv_byte < 0) return NULL;

		// 수신받은 메세지 출력
		printf("%s", msg);
	}
	return NULL;
}
