#include "unp.h"
#include "request.h"
#include "util.h"

#define TOTAL_PLAYERS 2
#define OFF 0
#define ON 1

// TODO
// kiem tra sendRequest, bao loi hoac gui lai
// phan tro giup, cuu ho

// send mess
void sendQuestionToAll(int *client, char *message, int len, int number_of_remaining_players);
void sendWrongAnswerMessage(int *remaining_players, char *player_answers, char right_answer, int len);
void sendMainPlayerFailedMessage(int *remaining_players, int len, char right_answer);
void sendMainPlayerStopMessage(int *remaining_players, int len, int score);

// print serv log
void printAllClient(int *client, int len);
void printAllPlayerAnswer(char *player_answers, int *remaining_players, int len);

// calculate current state
int numberOfanswers(char *player_answers, int len); 
int numberOfRemainingPlayers(int *remaining_players, int len);

// process player array
void processPlayerAnswer(int *remaining_players, char *player_answers, char right_answer, int len);
void resetPlayerAnswers(char *player_answers, int len);

int isMainPlayer(int sockfd, int main_player_sockfd);

int main(int argc, char **argv)
{
  int     i, maxi, maxfd, listenfd, connfd, sockfd;
  int     nready, client[FD_SETSIZE];
  // ssize_t n;
  fd_set  rset, allset;
  // char    buf[MAXLINE];
  struct sockaddr_in cliaddr, servaddr;
  socklen_t  clilen = sizeof(cliaddr);

  int score = 0;
  int request_counter = 0;
  int main_player_sockfd = 0;
  // int game_state = ON;
  int question_counter = 0;
  /*
  char questions[3][100], correct_answers[3];
  initQuestion(questions, correct_answers);
  */
  char questions[3][100];
  char correct_answers[3];

  strcpy(questions[0], "cau hoi 1: 1+1=?\na.2\nb.3\nc.4"); correct_answers[0] = 'a';
  strcpy(questions[1], "cau hoi 2: 2+1=?\na.2\nb.3\nc.4"); correct_answers[1] = 'b';
  strcpy(questions[2], "cau hoi 3: 3+1=?\na.2\nb.3\nc.4"); correct_answers[2] = 'c';
  //char correct_answers[3] = {'a', 'b', 'c'};

  char player_answers[FD_SETSIZE] = {0};
  int remaining_players[FD_SETSIZE] = {0};
  Request *req;

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
        if ( (req = recvRequest(sockfd)) == NULL) { /* connection closed by client */
          Close(sockfd);
          FD_CLR(sockfd, &allset);
          client[i] = -1;
        } else {
          // processing request
          printf("req mess: %s\n", req->mess);
          // if (gameState == OFF || remaining_players[sockfd] == OFF) {
          //   Writen(sockfd, mp_not_continue, strlen(mp_not_continue)+1);
          // }
          switch (req->type) {
            default:
			        printf("Error: Type invalid.\n");
			        break;
            //TODO help implement 1
            case TYPE_1_HELP_REQ: sendRequest(sockfd, TYPE_1_HELP_ANS, "1", 1);break;

            //TODO help implement 2
            case TYPE_2_HELP_REQ: sendRequest(sockfd, TYPE_2_HELP_ANS, "2", 2);break;

            //TODO help implement 3
            case TYPE_3_HELP_REQ: sendRequest(sockfd, TYPE_1_HELP_ANS, "3", 3);break;

            case 0:
              // cli gui ket noi, san sang choi
              if ((req->mess[0] == 'm') && (main_player_sockfd == 0)) {
                main_player_sockfd = sockfd;
                remaining_players[sockfd] = ON;
                player_answers[sockfd] = '0';
                printf("Main player connected! Socket: %d\n", main_player_sockfd);
                printAllClient(client, FD_SETSIZE);
                request_counter++;
                if (TOTAL_PLAYERS == request_counter) {
                  sendQuestionToAll(client, questions[question_counter], FD_SETSIZE, numberOfRemainingPlayers(remaining_players, FD_SETSIZE));
                } 
                break;
              }
              if (req->mess[0] == 'y') {
                request_counter++;
                remaining_players[sockfd] = ON;
                player_anwsers[sockfd] = '0';
                if (TOTAL_PLAYERS < request_counter) {
                  remaining_players[sockfd] = OFF;
                  player_anwsers[sockfd] = 0;
                  request_counter--;
                  sendRequest(sockfd, 16, "No more players can play. Sorry!", 0);
                  break;
                }
                if (TOTAL_PLAYERS == request_counter && main_player_sockfd == 0) {
                  remaining_players[sockfd] = OFF;
                  player_anwsers[sockfd] = 0;
                  request_counter--;
                  sendRequest(sockfd, 16, "No more players can play. Sorry!", 0);  
                  break;
                }
                if (TOTAL_PLAYERS == request_counter) {
                  sendQuestionToAll(client, questions[question_counter], FD_SETSIZE, numberOfRemainingPlayers(remaining_players, FD_SETSIZE));
                }
                printf("request_counter: %d sockfd: %d\n", request_counter, sockfd);
                printAllClient(client, FD_SETSIZE);
                break;
              }
              break;

            case TYPE_CLI_ANS:
              // cli gui tra loi
              if (isMainPlayer(sockfd, main_player_sockfd)) {
                if (numberOfRemainingPlayers(remaining_players, FD_SETSIZE) - numberOfanswers(player_answers, FD_SETSIZE) > 1) {
                  sendRequest(sockfd, 10, "You must wait for others finish their answer", 0);
                  break;
                }
                player_answers[sockfd] = req->mess[0];
                if (req->mess[0] == correct_answers[question_counter]) {
                  printf("number of answers: %d\n", numberOfanswers(player_answers, FD_SETSIZE));
                  printAllPlayerAnswer(player_answers, remaining_players, FD_SETSIZE);
                  processPlayerAnswer(remaining_players, player_answers, correct_answers[question_counter], FD_SETSIZE);
                  sendWrongAnswerMessage(remaining_players, player_answers, correct_answers[question_counter], FD_SETSIZE);

                  if (numberOfRemainingPlayers(remaining_players, FD_SETSIZE) == 1) {
                    // gameState = OFF;
                    score = 50000000;
                    sendRequest(sockfd, 12, "Congratulation! You win", score);
                    break;
                  }

                  sendRequest(sockfd, 14, "Your answer is right. Now, you can decide to go further or stop. (y/n) ", 0);
                  free(req);
                  req = recvRequest(sockfd);
                  if (req == NULL) {
                    err_quit("str_cli: server terminated prematurely");
                  }
                  if (req->mess[0] == 'n') {
                    // gameState = OFF;
                    score = calScore(100- numberOfRemainingPlayers(remaining_players, FD_SETSIZE));
                    sendMainPlayerStopMessage(remaining_players, FD_SETSIZE, score);
                    // exit(0);
                    printf("Game closed!\n");
                    break;
                  }
                  question_counter++;
                  request_counter = 0;
                  resetPlayerAnswers(player_answers, FD_SETSIZE);
                  sendQuestionToAll(client, questions[question_counter], FD_SETSIZE, numberOfRemainingPlayers(remaining_players, FD_SETSIZE));
                } else {
                  // gameState = OFF;
                  sendMainPlayerFailedMessage(remaining_players, FD_SETSIZE, correct_answers[question_counter]);                  
                  // exit(0);
                }
              } else {
                player_answers[sockfd] = req->mess[0];
              }
              printf("number of answers: %d\n", numberOfanswers(player_answers, FD_SETSIZE));
              break;
            }
            free(req);
            printAllPlayerAnswer(player_answers, remaining_players, FD_SETSIZE);    
          }
            
          if (--nready <= 0)
            break;         /* no more readable descriptors */
      }
    }
  }
}

void sendQuestionToAll(int *client, char *question, int len, int number_of_remaining_players) {
  int i;
  for (i = 0; i < len; ++i)
  {
    if ( client[i] < 0)
      continue;
    sendRequest(client[i], 9, question, number_of_remaining_players-1);
  }
}

void printAllClient(int *client, int len) {
  int i;
  for (i = 0; i < len; ++i)
  {
    if ( client[i] < 0)
      continue;
    printf("Client %d connected\n", client[i]);
  }
}

void printAllPlayerAnswer(char *player_answers, int *remaining_players, int len) {
  int i;
  for (i = 0; i < len; ++i)
  {
    if (remaining_players[i] == ON)
      printf("Client %d answer %c\n", i, player_answers[i]);
  }
}

int numberOfanswers(char *player_answers, int len) {
  int c, number = 0;
  for (c = 4; c < len; c++) {
    if (player_answers[c] >= 'a' && player_answers[c] <= 'c') {
      number++;
    }
  }
  return number;
}

int numberOfRemainingPlayers(int *remaining_players, int len) {
  int c, number = 0;
  for (c = 4; c < len; c++) {
    if (remaining_players[c] == ON) {
      number++;
    }
  }
  return number;
}

void resetPlayerAnswers(char *player_answers, int len) {
  int c;
  for (c = 4; c < len; c++) 
    player_answers[c] = '0';
}

int isMainPlayer(int sockfd, int main_player_sockfd) {
  if (sockfd == main_player_sockfd)
  {
    return 1;
  }
  return 0;
}

void processPlayerAnswer(int *remaining_players, char *player_answers, char right_answer, int len) {
  int i;
  for (i = 4; i < len; i++) {
    if ((remaining_players[i] == ON) && (player_answers[i] != right_answer)) {
      remaining_players[i] = OFF;
    }
  }
}

void sendWrongAnswerMessage(int *remaining_players, char *player_answers, char right_answer, int len) {
  int i;

  for (i = 4; i < len; i++) {
    if ((remaining_players[i] == OFF) && (player_answers[i] != right_answer)) {
      sendRequest(i, 11, "Thank you! Goodbye.", (int)right_answer);
    }
  }
}

void sendMainPlayerFailedMessage(int *remaining_players, int len, char right_answer) {
  int i;

  for (i = 4; i < len; i++) {
    if (remaining_players[i] == ON) {
      sendRequest(i, 13, "Thank you! Goodbye.", (int)right_answer);
    }
  }
}

void sendMainPlayerStopMessage(int *remaining_players, int len, int score) {
  int i;

  for (i = 4; i < len; i++) {
    if (remaining_players[i] == ON) {
      sendRequest(i, 15, "Thank you! Goodbye.", score);
    }
  }
}
