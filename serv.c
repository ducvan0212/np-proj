#include     "unp.h"

#define TOTAL_PLAYERS 3
#define OFF 0
#define ON 1

void initGame(char *player_anwsers, int *remaining_players, int len);
void send_message_to_all(int *client, char *message);
void print_all_client(int *client);
void print_all_player_answer(char *player_anwsers, int *remaining_players);
int number_of_anwsers(char *player_anwsers, int len); 
int number_of_remaining_players(int *remaining_players, int len);
void reset_player_answers(char *player_anwsers, int len);
int is_main_player(int sockfd, int main_player_sockfd);
void process_player_answer(int *remaining_players, char *player_anwsers, char right_answer);

int main(int argc, char **argv)
{
  int     i, maxi, maxfd, listenfd, connfd, sockfd;
  int     nready, client[FD_SETSIZE];
  ssize_t n;
  fd_set  rset, allset;
  char    buf[MAXLINE];
  socklen_t  clilen;
  struct sockaddr_in cliaddr, servaddr;

  int request_counter = 0;
  int main_player_sockfd = 0;
  int gameState = ON;
  int counter = 0;
  int question_counter = 0;
  char questions[3][100];
  strcpy(questions[0], "cau hoi 1: 1+1=?\na.2\nb.3\nc.4");
  strcpy(questions[1], "cau hoi 2: 2+1=?\na.2\nb.3\nc.4");
  strcpy(questions[2], "cau hoi 3: 3+1=?\na.2\nb.3\nc.4");
  char correct_anwsers[3] = {'a', 'b', 'c'};
  char player_anwsers[FD_SETSIZE];
  int remaining_players[FD_SETSIZE];
  char *get_ready_message = "grm";
  char *main_player_answer_fast_forward = "mpaff";
  char *mp_after_anwsering_right_message = "maarm";
  char *mp_after_anwsering_wrong_message = "maawm";
  char *mp_not_continue = "8";
  char *win = "w";

  initGame(player_anwsers, remaining_players, TOTAL_PLAYERS+4);

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
          // processing request
          printf("%s\n", buf);
          // if (gameState == OFF || remaining_players[sockfd] == OFF) {
          //   Writen(sockfd, mp_not_continue, strlen(mp_not_continue)+1);
          // }
          if ((main_player_sockfd == 0) && (buf[0] == 'm')) {
            main_player_sockfd = sockfd;
            printf("main player connected! Socket: %d\n", main_player_sockfd);
            print_all_client(client);
            request_counter++;
            if (number_of_remaining_players(remaining_players, FD_SETSIZE) == request_counter) {
              send_message_to_all(client, questions[question_counter]);
            } 
            continue;
          }
          if (buf[0] == 'y') {
            request_counter++;
            if (number_of_remaining_players(remaining_players, FD_SETSIZE) == request_counter) {
              send_message_to_all(client, questions[question_counter]);
            }
            printf("request_counter: %d sockfd: %d\n", request_counter, sockfd);
            print_all_client(client);
            continue;
          } else {
            if (is_main_player(sockfd, main_player_sockfd)) {
              if (number_of_remaining_players(remaining_players, FD_SETSIZE) - number_of_anwsers(player_anwsers, FD_SETSIZE) > 1) {
                Writen(sockfd, main_player_answer_fast_forward, strlen(main_player_answer_fast_forward)+1);
                continue;
              }
              player_anwsers[sockfd] = buf[0];
              if (buf[0] == correct_anwsers[question_counter]) {
                printf("number_of_anwsers: %d\n", number_of_anwsers(player_anwsers, FD_SETSIZE));
                print_all_player_answer(player_anwsers, remaining_players);    
                // in diem
                process_player_answer(remaining_players, player_anwsers, correct_anwsers[question_counter]);
                printf("number_of_remaining_players: %d\n", number_of_remaining_players(remaining_players, FD_SETSIZE));

                if (number_of_remaining_players(remaining_players, FD_SETSIZE) == 1) {
                  gameState = OFF;
                  // send score
                  send(main_player_sockfd, "7abc", strlen("7abc")+1, 0);
                  break;
                }

                Writen(sockfd, mp_after_anwsering_right_message, strlen(mp_after_anwsering_right_message)+1);
                if (recv(sockfd, buf, MAXLINE, 0) == 0) {
                  err_quit("str_cli: server terminated prematurely");
                }
                if (buf[0] == 'n') {
                  gameState = OFF;
                  send_message_to_all(client, mp_not_continue);
                  // exit(0);
                  break;
                }
                question_counter++;
                request_counter = 0;
                reset_player_answers(player_anwsers, FD_SETSIZE);
                send_message_to_all(client, questions[question_counter]);
              } else {
                gameState = OFF;
                send_message_to_all(client, "Main player failed to victory!");
                // exit(0);
              }
            } else {
              player_anwsers[sockfd] = buf[0];
            }
            printf("number_of_anwsers: %d\n", number_of_anwsers(player_anwsers, FD_SETSIZE));
          }

          print_all_player_answer(player_anwsers, remaining_players);    
          // for (i = 0; i < FD_SETSIZE; ++i)
          // {
          //   printf("%c", player_anwsers[i]);
          // }
        }
            
        if (--nready <= 0)
          break;         /* no more readable descriptors */
      }
    }
  }
}

void initGame(char *player_anwsers, int *remaining_players, int len) {
  int c;
  for (c = 4; c < len; c++) {
    remaining_players[c] = ON;
  }
  for (c = 0; c < FD_SETSIZE; c++) {
    player_anwsers[c] = '0';
  }
}

void send_message_to_all(int *client, char *message) {
  int i;
  for (i = 0; i < FD_SETSIZE; ++i)
  {
    if ( client[i] < 0)
      continue;
    Writen(client[i], message, strlen(message));
  }
}

void print_all_client(int *client) {
  int i;
  for (i = 0; i < FD_SETSIZE; ++i)
  {
    if ( client[i] < 0)
      continue;
    printf("Client %d\n", client[i]);
  }
}

void print_all_player_answer(char *player_anwsers, int *remaining_players) {
  int i;
  for (i = 0; i < FD_SETSIZE; ++i)
  {
    if (remaining_players[i] == OFF)
      continue;
    printf("Client %d answer %c\n", i, player_anwsers[i]);
  }
}

int number_of_anwsers(char *player_anwsers, int len) {
  int c, number = 0;
  for (c = 4; c < len; c++) {
    if (player_anwsers[c] != '0') {
      number++;
    }
  }
  return number;
}

int number_of_remaining_players(int *remaining_players, int len) {
  int c, number = 0;
  for (c = 4; c < len; c++) {
    if (remaining_players[c] == ON) {
      number++;
    }
  }
  return number;
}

void reset_player_answers(char *player_anwsers, int len) {
  int c;
  for (c = 4; c < len; c++) 
    player_anwsers[c] = '0';
}

int is_main_player(int sockfd, int main_player_sockfd) {
  if (sockfd == main_player_sockfd)
  {
    return 1;
  }
  return 0;
}

void process_player_answer(int *remaining_players, char *player_anwsers, char right_answer) {
  int i;
  char mess[100];
  char ranswer[2];
  ranswer[0] = right_answer;
  ranswer[1] = '\0';
  strcpy(mess, "9Right answer is ");
  strcat(mess, ranswer);
  for (i = 4; i < FD_SETSIZE; i++) {
    if ((remaining_players[i] == ON) && (player_anwsers[i] != right_answer)) {
      remaining_players[i] = OFF;
      send(i, mess, strlen(mess)+1, 0);
    }
  }
}