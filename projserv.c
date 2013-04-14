#include     "unp.h"

#define TOTAL_PLAYERS 3
#define OFF 0
#define ON 1

int number_of_anwsers(char *player_anwsers, int len); 
int number_of_remaining_players(int *remaining_players, int len);
void reset_player_answers(char *player_anwsers, int len);

int main(int argc, char **argv)
{
  int     i, maxi, maxfd, listenfd, connfd, sockfd;
  int     nready, client[FD_SETSIZE];
  ssize_t n;
  fd_set  rset, allset;
  char    buf[MAXLINE];
  socklen_t  clilen;
  struct sockaddr_in cliaddr, servaddr;

  int gameState = ON; // 1 la choi, 0 la nghi
  int counter = 0;
  int question_counter = 0;
  char questions[3][100];
  strcpy(questions[0], "cau hoi 1: 1+1=?\na.2\nb.3\nc.4\nd.5");
  strcpy(questions[1], "cau hoi 2: 2+1=?\na.2\nb.3\nc.4\nd.5");
  strcpy(questions[2], "cau hoi 3: 3+1=?\na.2\nb.3\nc.4\nd.5");
  char correct_anwsers[3] = {'a', 'b', 'c'};
  char player_anwsers[TOTAL_PLAYERS + 4] = {'0', '0', '0', '0', '0', '0', '0'};
  int remaining_players[TOTAL_PLAYERS + 4] = {0,0,0,0,ON,ON,ON};
  char *after_anwsering_message = "Waiting for other players. Feel free to change your mind\nSau khi ng choi chinh chon ket qua, an 'y' de tiep tuc\n";
  char *m_after_anwsering_right_message = "Your answer is right. Next? (y/n) ";
  char *m_not_continue = "q";
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  
  Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
  
  Listen(listenfd, LISTENQ);
  maxfd = listenfd;            /* initialize */
  maxi = -1;                   /* index into client[] array */
  
  for (i = 0; i < FD_SETSIZE;  i++)
    client[i] = -1;          /* -1 indicates available entry */
  
  FD_ZERO(&allset);
  FD_SET(listenfd, &allset);

  for ( ; ; ) {
    rset = allset;          /* structure assignment */
    nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

    if (FD_ISSET(listenfd, &rset)) {       /* new client connection */
      connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

      for (i = 0; i < FD_SETSIZE; i++)
        if (client[i] < 0) {
          client[i] = connfd; /* save descriptor */
          break;
        }
        
      if (i == FD_SETSIZE)
        err_quit("too many clients");
        
      FD_SET(connfd, &allset);       /* add new descriptor to set */
      if (connfd > maxfd)
        maxfd = connfd; /* for select */
      if (i > maxi)
        maxi = i;          /* max index in client[] array */
      if (--nready <= 0)
        continue;          /* no more readable descriptors */
    }

    for (i = 0; i <= maxi; i++) {       /* check all clients for data */
      if ( (sockfd = client[i]) < 0)
        continue;
      if (FD_ISSET(sockfd, &rset)) {

        if ( (n = Read(sockfd, buf, MAXLINE)) == 0) { /* connection closed by client */
          Close(sockfd);
          FD_CLR(sockfd, &allset);
          client[i] = -1;
        } else {
          if (buf[0] == 'y') {
            // bat dau choi
            Writen(sockfd, questions[question_counter], strlen(questions[question_counter])+1);
          } else {
            player_anwsers[sockfd] = buf[0];
            if (number_of_remaining_players(remaining_players, TOTAL_PLAYERS+4) > number_of_anwsers(player_anwsers, TOTAL_PLAYERS+4))
            {
              // thay dieu kien if thanh !is_main_player())
              Writen(sockfd, after_anwsering_message, strlen(after_anwsering_message)+1);
            } else {
              // kiem tra so cau tra loi < so ng choi con lai
              // neu co ng cung choi chua tra loi, auto tra loi sai, loai
              if (buf[0] == correct_anwsers[question_counter]) {
                //  chia diem, cong diem
                //  xu li mang, thong bao 99 nguoi cung choi tra loi dung sai
                Writen(sockfd, m_after_anwsering_right_message, strlen(m_after_anwsering_right_message)+1);
                if (recv(sockfd, buf, MAXLINE, 0) == 0) {
                  err_quit("str_cli: server terminated prematurely");
                }
                if (buf[0] == 'n') {
                  gameState = 0;
                  // gui den tat ca client q de ngat ket noi
                  Writen(sockfd, m_not_continue, strlen(m_not_continue)+1);
                }
                question_counter++;
                reset_player_answers(player_anwsers, TOTAL_PLAYERS+4);
              } else {
              //  thong bao sai, dung choi, ngat ket noi
              }              
              Writen(sockfd, questions[question_counter], strlen(questions[question_counter])+1);
            }
          }

          for (counter = 0; counter < TOTAL_PLAYERS + 4; counter++)
          {
            printf("Player %d tra loi %c\n", counter, player_anwsers[counter]);
          }
          printf("\n");
        }
        
        if (--nready <= 0)
          break;         /* no more readable descriptors */
      }
    }
  }
}

int number_of_anwsers(char *player_anwsers, int len) {
  int c, number = 0;
  for (c = 4; c < len; c++) {
    if (player_anwsers[c] != '0') {
      number++;
    }
  }
  printf("number of anwsers: %d\n", number);
  return number;
}

int number_of_remaining_players(int *remaining_players, int len) {
  int c, number = 0;
  for (c = 4; c < len; c++) {
    if (remaining_players[c] == ON) {
      number++;
    }
  }
  printf("number of remaining player: %d\n", number);
  return number;
}

void reset_player_answers(char *player_anwsers, int len) {
  int c;
  for (c = 4; c < len; c++) 
    player_anwsers[c] = '0';
}