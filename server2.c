#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

// Thread
#include <pthread.h>

struct _stData {
	char buf[50];
	int a;
	int b;
	int c;
};

int	status;

void* thread_recv(void* pParam)
{
	//char buf[50];
	struct _stData dat;
	int *sock;

	sock = (int *)pParam;
	while(1) {
		memset(dat.buf, 0, sizeof(dat.buf));
		// パケット受信、パケットが到着するまでブロック
		if(recv(*sock, &dat, sizeof(dat), 0) < 0) {
			perror("recv");
			status = 3;
			pthread_exit(0);
		}

		printf("%s\n", dat.buf);

		// "END" を受信したらスレッドから抜ける
		if(strncmp(dat.buf, "END", strlen(dat.buf)) == 0) {
			break;
		}
	}
}

void* thread_write(void* pParam)
{
	//char buf[50];
	struct _stData dat;
	int *sock;

	sock = (int *)pParam;
	while(1) {
		printf("Input next word!! : ");
		if(status == 3 ){
			printf("exit thread_write\n");
			pthread_exit(0);
		}

		sleep(2);
		sprintf(dat.buf, "12345");

		//scanf("%s", &dat.buf);

		/* 5文字送信 */
		write(*sock, &dat, sizeof(dat));

		// "END" を入力したら終了
		if(strncmp(dat.buf, "END", strlen(dat.buf)) == 0) {
			break;
		}
	}
}

int main()
{
	int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;
	int sock;
	char buf[50];
	char inputBuf[50];
	pthread_t t_recv, t_write;


	signal(SIGPIPE, SIG_IGN);	// SIGPIPEシグナルを無視する
	status = 0;

	while(1) {

	/* ソケットの作成 */
	sock0 = socket(AF_INET, SOCK_STREAM, 0);
	if(sock0 < 0) {
		perror("socket");
		printf("err no = %d¥n", errno);
		return 1;
	} else {
		/* ソケットの設定 */
		addr.sin_family = AF_INET;
		addr.sin_port = htons(49152);
		addr.sin_addr.s_addr = INADDR_ANY;

		// バインドする	
		if(bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("bind");
			return 1;
		}

		/* TCPクライアントからの接続要求を待てる状態にする */
		if(listen(sock0, 5) < 0) {
			perror("listen");
			return 1;
		}

		/* TCPクライアントからの接続要求を受け付ける */
		len = sizeof(client);
		if((sock = accept(sock0, (struct sockaddr *)&client, &len)) < 0) {
			perror("accept");
			return 1;
		}

		// 受信ループスレッド作成
		if(0 == pthread_create(&t_recv, NULL, thread_recv, (void *)& sock)) {
			if(0 == pthread_create(&t_write, NULL, thread_write, (void *)& sock)) {
				status = 1;

				// スレッド終了待ち
				pthread_join(t_recv, NULL);
				pthread_join(t_write, NULL);
			}
		}

		/* TCPセッションの終了 */
		close(sock);

		/* listen するsocketの終了 */
		close(sock0);
	}

	}

	return 0;
}
