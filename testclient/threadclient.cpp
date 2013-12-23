#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/syscall.h>  
#define gettid() syscall(__NR_gettid)  

typedef struct thr_info {
	pthread_t thr_id;
	int thr_num;
	int sockfd;
}thr_info;

typedef struct tagData 
{
	int iSize;
	char pData[0];
}tagData;
const short g_hdDataSize = sizeof(tagData);

int i = 0;
unsigned int tTime = time(NULL);

void *thr_fn(void *arg) {
	printf("%d pid: %d tid: %lu num: %d\n", *(int *)arg, getpid(), pthread_self(), ((thr_info *)arg)->thr_num);
	int ret = 0;
	unsigned char buf[10241] = {0};

	tagData *p = (tagData *)buf;
	int n = (rand_r(&tTime) % 10) * 10 + (rand_r(&tTime) % 10);

	for (int j = 0; j < n; j++) {
		strcat((char *)p->pData, "哈");
	}

	p->iSize = g_hdDataSize + n * strlen("哈");

	ret = send(((thr_info *)arg)->sockfd, buf, p->iSize, 0);
	if (-1 == ret)
	{
		printf("send error:%s", strerror(errno));
		return NULL;
	}
	else
	{
		i++;
		char *pBuf[1024] = {0};
		memcpy(pBuf, p->pData, p->iSize - g_hdDataSize);
		printf("%d: send %d, %s\n", i, ret, pBuf);
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	int sockfd;
	int ret;
	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_port = htons(6666);
	address.sin_addr.s_addr = inet_addr("192.168.1.222");

	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) {
		printf("Socket error, %s", strerror(errno));
		return -1;
	}

	ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
	if (ret == -1) {
		printf("Connect error, %s", strerror(errno));
		return -1;
	}

	int thr_num, opt;

	while ((opt = getopt(argc, argv, "t:")) != -1) {
		switch (opt) {
			case 't':
				thr_num = strtoul(optarg, NULL, 0);
				break;

			default:
				fprintf(stderr, "Usage: %s [-t thr_num] arg...\n", 
						argv);
				exit(EXIT_FAILURE);
		}
	}

	void *res;
	pthread_t thread[thr_num];
	struct timespec req;
	req.tv_sec = 0;
	req.tv_nsec = 300;
	thr_info *a_thr_info = (thr_info *)calloc(thr_num, sizeof(thr_info));
	if (a_thr_info == NULL)
		return 0;

	for (int i = 0; i < thr_num; i++) {
		a_thr_info[i].thr_num = thr_num;
		a_thr_info[i].thr_id = i + 1;
		a_thr_info[i].sockfd = sockfd;
		ret = pthread_create(&thread[i], NULL, thr_fn, &a_thr_info[i]);
		nanosleep(&req, NULL);
	}

	for (int i = 0; i < thr_num; i++) {
		ret = pthread_join(thread[i], &res);
		//			printf("%s\n", (char *)res);
		free(res);
	}

	free(a_thr_info);
	//	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	sockfd = -1;
	return 0;
}

