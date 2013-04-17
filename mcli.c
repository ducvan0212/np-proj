#include "unp.h"
#include "request.h"

void cliProcess(FILE *fp, int sockfd);
int isValidedInputForQuestion(char *input, int len);
int isValidedInputForNavigation(char *input, int len);
int printRecvMessage(Request *req);
int calScore(int number_of_defeated_players);

int main(int argc, char **argv)
{
  int sockfd;
  struct sockaddr_in  servaddr;
  
  if (argc != 2)
    err_quit("usage: tcpcli <IPaddress>");

  sockfd = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
  
  cliProcess(stdin, sockfd);    /* do it all */

  exit(0);
}

void cliProcess(FILE *fp, int sockfd)
{
  char  sendline[MAXLINE], recvline[MAXLINE];
  int recv_bytes = 0;
  Request *req;
  // int received_question = 0;

  printf("DAU TRUONG 100\n");
  printf("Waiting for other players connect to game...\n");
  sendRequest(sockfd, 0, "m", 0);

  while ((req = recvRequest(sockfd)) != NULL) {
    if (printRecvMessage(req) == 0) {
      break;
    }
    
    Fgets(sendline, MAXLINE, fp);

    if (req->type == 9 || req->type == 10) {
      while (isValidedInputForQuestion(sendline, MAXLINE) == 0) {
        printf("Invalid input, reselect please\nYour choice: ");
        Fgets(sendline, MAXLINE, fp);
      }
    } else if (req->type == 14) {
      while (isValidedInputForNavigation(sendline, MAXLINE) == 0) {
        printf("Invalid input, reselect please\nYour choice: ");
        Fgets(sendline, MAXLINE, fp);
      }
    } else {
      printf("Cannot reach here. Something wrongs\n");
    }

    printf("Submiting... Waiting for others...\n");
    sendRequest(sockfd, 1, sendline, 0);
  }
  if (req == NULL)
    err_quit("str_cli: server terminated prematurely");
}

// 0 is invalid
int isValidedInputForNavigation(char *input, int len) {
  int input_len = strlen(input);
  if ( input_len != 2) {
    return 0;
  }
  if (input[0] == 'y' || input[0] == 'n') {
    return 1;
  }
  return 0;
}

// 0 is invalid
int isValidedInputForQuestion(char *input, int len) {
  int input_len = strlen(input);
  if ( input_len != 2) {
    return 0;
  }
  if ((int)input[0] < (int)'a' || (int)input[0] > (int)'c') {
    return 0;
  }
  return 1;
}

int printRecvMessage(Request *req) {
  switch (req->type) {
    case 9: 
      // serv gui cau hoi
      printf("\nThere are %d remaining players\n", req->num);
      printf("You are having %d$ in reward\n", calScore(100-req->num));
      printf("%s\n", req->mess);
      printf("Your choice: ");
      break;
    case 10:
      // main player tra loi cau hoi truoc khi 99 nguoi cung choi tra loi
      printf("%s\n", req->mess);
      printf("Your choice: ");
      break;
    case 12:
      // main player chien thang
      printf("\n%s\n", req->mess);
      printf("All other players are defeated.\n");
      printf("Your reward: %d$\n", req->num);
      return 0;
    case 13:
      // main player that bai
      printf("Right answer is %c. ", (char)req->num);
      printf("You failed in this question. %s\n", req->mess);
      return 0;
    case 14:
      // tiep tuc hay dung choi
      printf("%s\n", req->mess);
      printf("Your choice: ");
      break;
    case 15:
      // tiep tuc hay dung choi
      printf("You chosen to stop. Your reward: %d$\n", req->num);
      printf("%s\n", req->mess);
      return 0;
  }

  return 1;
}

int calScore(int number_of_defeated_players) {
  int n = number_of_defeated_players % 10;
  switch (n) {
    case 0:
      return 100;
      break;
    case 1:
      return 200;
      break;
    case 2:
      return 400;
      break;
    case 3:
      return 800;
      break;
    case 4:
      return 1000;
      break;
    case 5:
      return 2000;
      break;
    case 6:
      return 4000;
      break;
    case 7:
      return 8000;
      break;
    case 8:
      return 10000;
      break;
    case 9:
      return 20000;
      break;
  }
  return 0;
}