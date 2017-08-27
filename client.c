#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>


struct _stData {
	char buf[50];
	int a;
	int b;
	int c;
};

int main()
{
	struct sockaddr_in server;
	int sock;
//	char buf[32];
	struct _stData dat;
	int n;

	/* ソケットの作成 */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		printf("err no = %d\n", errno);
		return 1;
	} else {
		/* 接続先指定用構造体の準備 */
		server.sin_family = AF_INET;
		server.sin_port = htons(49152);
		server.sin_addr.s_addr = inet_addr("192.168.12.35");

		/* サーバに接続 */
		connect(sock, (struct sockaddr *)&server, sizeof(server));

		if(send(sock, "I am send process", 17, 0) < 0) {
			perror("send");
			return 1;
		}

		while(1) {

			/* サーバからデータを受信 */
			memset(dat.buf, 0, sizeof(dat.buf));
			n = read(sock, &dat, sizeof(dat));

			if(strncmp(dat.buf, "END", strlen(dat.buf)) == 0) {
		                if(send(sock, "END", 3, 0) < 0) {
		               	         perror("send2");
                       			 return 1;
               			}

				break;
			}

			printf("%d, %s\n", n, dat.buf);
		}

		/* socketの終了 */
		close(sock);
	}

	return 0;
}
