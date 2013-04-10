#include  "unp.h"

void str_cli_cus(FILE *fp, int sockfd);

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
  
  str_cli_cus(stdin, sockfd[0]);    /* do it all */

  exit(0);
}

void str_cli_cus(FILE *fp, int sockfd)
{
  char  sendline[MAXLINE], recvline[MAXLINE];

  printf("Are you ready? (y/n) ");
  while (Fgets(sendline, MAXLINE, fp) != NULL) {
    send(sockfd, sendline, strlen(sendline), 0);

    if (recv(sockfd, recvline, MAXLINE, 0) == 0) {
      err_quit("str_cli: server terminated prematurely");
    }
    
    printf("%s\nYour anwser: ", recvline);
    // Fputs(recvline, stdout);
  }
}
