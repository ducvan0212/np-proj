#include  "unp.h"

void cli_process(FILE *fp, int sockfd);
int is_valided_input(char *input, int len);
void print_recv_message(int sockfd, char *recvline, int len);

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
  // int received_question = 0;

  printf("Are you ready? (y/n) ");
  while (Fgets(sendline, MAXLINE, fp) != NULL) {
    if (!is_valided_input(sendline, MAXLINE)) {
      printf("Invalid input, choice again\nYour choice: ");;
      continue;
    }

    // if ((received_question == 0) && ((int)sendline[0] >= (int)'a') && ((int)sendline[0] <= (int)'d')) {
    //   printf("Get ready by press y\nYour choice: ");;
    //   continue;        
    // }

    send(sockfd, sendline, strlen(sendline), 0);

    recv_bytes = recv(sockfd, recvline, MAXLINE, 0);
    if (recv_bytes == 0) {
      err_quit("str_cli: server terminated prematurely");
    }
    
    print_recv_message(sockfd, recvline, recv_bytes);

    // else if (recvline[0] == 'c') {
    //   received_question = 1;
    // }

    printf("Your choice: ");
  }
}

// 0 is invalid
int is_valided_input(char *input, int len) {
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

void print_recv_message(int sockfd, char *recvline, int len) {
  char *after_anwsering_message = "Waiting for other players. Feel free to change your mind\nSau khi ng choi chinh chon ket qua, an 'y' de tiep tuc";
  char *get_ready_message = "Get ready by press y";
  char *main_player_answer_fast_forward = "You must wait for others finish their answer";
  char *mp_after_anwsering_right_message = "Your answer is right. Next? (y/n) ";
  char *mp_after_anwsering_wrong_message = "Your answer is wrong. Good bye";
  char *mp_not_continue = "Good bye";

  if (strcmp(recvline, "aam") == 0) {
    printf("%s\n", after_anwsering_message);
  } else if (strcmp(recvline, "grm") == 0) {
    printf("%s\n", get_ready_message);
  } else if (strcmp(recvline, "mpaff") == 0) {
    printf("%s\n", main_player_answer_fast_forward);
  } else if (strcmp(recvline, "maarm") == 0) {
    printf("%s\n", mp_after_anwsering_right_message);
  } else if (strcmp(recvline, "maawm") == 0) {
    err_quit(mp_after_anwsering_wrong_message);
  } else if (strcmp(recvline, "mnc") == 0) {
    err_quit(mp_not_continue);
  } else {
    printf("%s\n", recvline);
  }
}