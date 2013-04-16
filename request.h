#ifndef REQUEST_H
#define REQUEST_H

// request type 0: bat dau ket noi tu cli
//         type 1: cli gui tra loi cau hoi
//         type 9: serv gui cau hoi
//         type 10: main player tra loi truoc 99 ng cung choi
//         type 11: nguoi cung choi tra loi sai
//         type 12: main player chien thang
//         type 13: main player that bai
typedef struct Request_t
{
  int type;
  char mess[100];
  int num;
} Request;

int sendRequest(int sockfd, int type, char *mess, int num);
Request *recvRequest(int sockfd);

#endif
