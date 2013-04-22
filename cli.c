#include "unp.h"
#include "request.h"

void cliProcess(FILE *fp, int sockfd);
int isValidedInput(char *input, int len);
int printRecvMessage(Request *req);

int main(int argc, char **argv)
{
  int sockfd;
  struct sockaddr_in  servaddr;
  
  // check IP
  if (argc != 2) err_quit("usage: tcpcli <IPaddress>");

  // get socket IPv4, data stream
  sockfd = Socket(AF_INET, SOCK_STREAM, 0);

  // bind and connect
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
  
  // client side
  cliProcess(stdin, sockfd);

  // exit(0);
  return 0;
}

void cliProcess(FILE *fp, int sockfd)
{
  char  sendline[MAXLINE], recvline[MAXLINE];
  Request *req;
  // int received_question = 0;

  printf("+------------------------------------+\n");
  printf("|            DAU TRUONG 100          |\n");
  printf("+------------------------------------+\n");
  printf("Waiting for other players connect to game...\n");
  sendRequest(sockfd, 0, "y", 0);

  while ((req = recvRequest(sockfd)) != NULL) {
    if (printRecvMessage(req) == 0) {
      break;
    }
    
    Fgets(sendline, MAXLINE, fp);
    while (isValidedInput(sendline, MAXLINE) == 0) {
      printf("Invalid input, reselect please\nYour choice: ");
      Fgets(sendline, MAXLINE, fp);
    }
    printf("Submiting... Waiting for others...\n");
    sendRequest(sockfd, TYPE_CLI_ANS, sendline, 0);
  }
  if (req == NULL)
    err_quit("str_cli: server terminated prematurely");
}

int isValidedInput(char *input, int len) {
  int input_len = strlen(input);

  if ( input_len != 2) return FALSE;
  if ((int)input[0] < (int)'a' || (int)input[0] > (int)'c') return FALSE;

  return TRUE;
}

int printRecvMessage(Request *req) {
  switch (req->type) {
    case 9: 
      // serv gui cau hoi
      printf("\nThere are %d remaining players\n", req->num);
      printf("%s\n", req->mess);
      printf("Your choice: ");
      break;
    case 10:
      // main player tra loi cau hoi truoc khi 99 nguoi cung choi tra loi
      printf("%s\n", req->mess);
      printf("Your choice: ");
      break;
    case 11:
      // 1 hay nhieu nguoi cung choi tra loi sai
      printf("Right answer is %c. ", (char)req->num);
      printf("%s\n", req->mess);
      return 0;
    case 13:
      // main player that bai
      printf("Right answer is %c. ", (char)req->num);
      printf("Main player failed in this question. %s\n", req->mess);
      return 0;
    case 15:
      // tiep tuc hay dung choi
      printf("Main player chosen to stop and take his %d$ reward\n", req->num);
      printf("%s\n", req->mess);
      return 0;
    case 16:
      // nguoi choi bi tu choi vi da du so nguoi choi
      printf("%s\n", req->mess);
      return 0;
  }

  return 1;
}
