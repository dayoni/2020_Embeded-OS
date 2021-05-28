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
#define SERV_PORT		4560
#define BUF_SIZE		512
#define MY_PORT			"4562"
#define MY_IP			"220.149.128.102"

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
	scanf("%c", &clean);	// 버퍼 초기화

	printf("PW : ");
	scanf("%s", pw);
	scanf("%c", &clean);	// 버퍼 초기화
	printf("---------------------------------\n");

	// 서버로 입력한 로그인 정보 전송
	send(serv_fd, id, strlen(id) + 1, 0);
	send(serv_fd, pw, strlen(pw) + 1, 0);
	sleep(1);
	send(serv_fd, MY_IP, strlen(MY_IP) + 1, 0);
	send(serv_fd, MY_PORT, strlen(MY_PORT) + 1, 0);

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
		if (rcv_byte < 0) return NULL;

		// 수신받은 메세지 출력
		printf("%s", msg);

		// 수신받은 메세지가 "[FILE]" -> P2P 파일 전송 모드 시작
		if (strstr(msg, "[FILE]")) {
			rcv_byte = recv(serv_fd, msg, sizeof(msg), 0);
			char server_ip[100];
			sprintf(server_ip, "%s", msg);
			rcv_byte = recv(serv_fd, msg, sizeof(msg), 0);

			printf("\n=============== Welcome P2P file Transfer Mode ===============\n");
			char file_name[100][BUF_SIZE];
			char file_count[100];
			char file_selec[20];
			char file_pos[100];
			char file_text[100];
			char selec_file[100];

			int file_num = 0;
			int file_sel = 0;
			int port = atoi(msg);
			int new_serv_fd;

			struct dirent *direc;
			DIR *file;

			struct sockaddr_in new_serv_addr;
			//pthread_t snd_thread, rcv_thread;

			// 소켓의 프로토콜을 설정하는 부분이므로 IPv4 프로토콜인 PF_INET으로 변경
			new_serv_fd = socket(PF_INET, SOCK_STREAM, 0);

			if (new_serv_fd == -1) {
				perror("Client-sockek() error lol!");
				exit(1);
			}
			else printf("Client-socket() serv_fd is OK...\n");

			// serv_addr를 0으로 초기화 
			memset(&new_serv_addr, 0, sizeof(new_serv_addr));

			// IPv4 주소 형식으로 설정, 서버 PORT와 IP로 초기화
			new_serv_addr.sin_family = AF_INET;
			new_serv_addr.sin_port = htons(port);
			new_serv_addr.sin_addr.s_addr = inet_addr(server_ip);

			// 초기화된 정보를 기반으로 연결요청, 에러 발생 시 프로세스 종료
			if (connect(new_serv_fd, (struct sockaddr *)&new_serv_addr, sizeof(new_serv_addr)) == -1) {
				perror("Client-connect() error lol!");
				exit(1);
			}
			else printf("Client-connect() is OK...\n\n");

			// 파일을 열어서 자신이 가지고있는 파일 리스트 확인 후 클라이언트-서버로 전송하는 부분
			file = opendir("/home/st2018146015/project3");

			// 하나씩 확인 후 배열에 저장
			while ((direc = readdir(file)) != NULL) {
				if ((strcmp(direc->d_name, ".") != 0) && (strcmp(direc->d_name, "..") != 0)) {
					printf("%d. %s\n", file_num + 1, direc->d_name);
					sprintf(file_name[file_num], "%s", direc->d_name);
					file_num++;
				}
			}
			closedir(file);

			// 파일 리스트 갯수 전송
			sprintf(file_count, "%d", file_num);
			send(new_serv_fd, file_count, strlen(file_count) + 1, 0);

			for (int i = 0; i < file_num; i++) {
				sleep(1);
				send(new_serv_fd, file_name[i], strlen(file_name[i]) + 1, 0);
			}

			// 클라이언트-서버에서 선택한 번호 받아오는 부분
			rcv_byte = recv(new_serv_fd, file_selec, sizeof(file_selec), 0);
			file_sel = atoi(file_selec);

			// 문자열로 수신되어지기 때문에 Int 형으로 변환
			printf("\nuser request the file\n");

			// 파일 여는부분 --> 파일 경로/파일이름 파일이름은 받아온 숫자에 있는 배열값으로!
			sprintf(file_pos, "/home/st2018146015/project3/%s", file_name[file_sel]);
			FILE* myfile;

			// 파일 열어서 읽는부분 "r"
			myfile = fopen(file_pos, "r");

			// 파일의 내용 저장 및 출력
			fgets(file_text, sizeof(file_text), myfile);
			printf("\n========== text ==========\n");
			printf("%s", file_text);
			printf("==========================\n");
			printf("<<  sending complete  >>\n");
			send(new_serv_fd, file_text, strlen(file_text) + 1, 0);

			// 저장된 내용 전송!
			fclose(myfile);
			printf("\n=============== Restart Group Chatting Mode ===============\n");
		}
	}
	return NULL;
}