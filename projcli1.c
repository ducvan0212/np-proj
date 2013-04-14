#include  "unp.h"

void cli_process(FILE *fp, int sockfd);
int check_user_input(char *input, int len);

int main(int argc, char **argv)
{
  int         i, sockfd[5];
  struct sockaddr_in  servaddr;

  if (argc != 2)
    err_quit("usage: tcpcli <IPaddress>");

  sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  Connect(sockfd[i], (SA *) &servaddr, sizeof(servaddr));
  
  cli_process(stdin, sockfd[0]);    /* do it all */

  exit(0);
}

void cli_process(FILE *fp, int sockfd)
{
  char  sendline[MAXLINE], recvline[MAXLINE];
  int recv_bytes = 0;

  printf("Are you ready? (y/n) ");
  while (Fgets(sendline, MAXLINE, fp) != NULL) {
    if (!check_user_input(sendline, MAXLINE)) {
      printf("Invalid input, choice again\nYour choice: ");;
      continue;
    }

    send(sockfd, sendline, strlen(sendline), 0);

    recv_bytes = recv(sockfd, recvline, MAXLINE, 0);
    if (recv_bytes == 0) {
      err_quit("str_cli: server terminated prematurely");
    }
    
    if (is_close_signal(recvline, recv_bytes)) {
      err_quit("Good bye\n");
    }
    printf("%s\nYour anwser: ", recvline);
    // Fputs(recvline, stdout);
  }
}

// 0 is invalid
int check_user_input(char *input, int len) {
  int input_len = strlen(input);
  if ( input_len != 2) {
    return 0;
  }
  if ((int)input[0] == (int)'y' || (int)input[0] == (int)'n') {
    return 1;
  } else if ((int)input[0] < (int)'a' || (int)input[0] > (int)'d') {
    return 0;
  }
  return 1;
}

int is_close_signal(char *recv, int len) {
  if (recv[0] == 'q') {
    return 1;
  } 
  return 0;
}