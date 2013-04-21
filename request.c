#include "request.h"
#include <sys/socket.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int sendRequest(int sockfd, int type, char *mess, int num) {
  Request *req = (Request *)malloc(sizeof(Request));
  int sendbytes;

  req->type = type;
  strcpy(req->mess, mess);
  req->num = num;
  sendbytes = send(sockfd, (char *)req, sizeof(*req), 0);
  // printf("%d %d\n", sizeof(char), sizeof(Request));
  free(req);
  // printf("sendbytes: %d\n", sendbytes);
  return sendbytes;
}

Request *recvRequest(int sockfd) {
  Request *req = (Request *)malloc(sizeof(Request));
  int recvbytes;
  char recvline[4096];
  
  recvbytes = recv(sockfd, recvline, 4096, 0);
  // printf("recvbytes: %d\n", recvbytes);
  if (recvbytes == 0)
    return NULL;
  
  memcpy((char *)req, recvline, sizeof(Request));
  // printf("type: %d  mess: %s  num: %d\n", req->type, req->mess, req->num);
  return req;
}
