#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_IP		"220.149.128.103"
#define SERV_PORT	4568

int main(int argc, char *agrv[])
{
	int serv_fd;
	struct sockaddr_in dest_addr;

	int rcv_byte;
	char buf[512];

	char id[20];
	char pw[20];

	serv_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (serv_fd == -1)
	{
		perror("Client-sockek() error lol!");
		exit(1);
	}
	else printf("Client-socket() serv_fd is OK...\n");

	/* host byte order */
	dest_addr.sin_family = AF_INET;

	/* short, network byte order */
	dest_addr.sin_port = htons(SERV_PORT);
	dest_addr.sin_addr.s_addr = inet_addr(SERV_IP);

	/* zero the rest of the struct */
	memset(&(dest_addr.sin_zero), 0, 8);

	/* connect */
	if (connect(serv_fd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Client-connect() error lol!");
		exit(1);
	}
	else printf("Client-connect() is OK...\n\n");

	/* receive INIT_MSG */
	rcv_byte = recv(serv_fd, buf, sizeof(buf), 0);
	printf("%s\n", buf);

	// ID, PW 입력
	printf("ID : ");
	scanf("%s", id);

	printf("PW : ");
	scanf("%s", pw);
	printf("---------------------------------\n");

	// server로 ID, PW 전송
	send(serv_fd, id, strlen(id) + 1, 0);
	send(serv_fd, pw, strlen(pw) + 1, 0);

	// 로그인 결과 받기
	rcv_byte = recv(serv_fd, buf, sizeof(buf), 0);
	printf("%s\n\n", buf);

	close(serv_fd);

	return 0;
}
