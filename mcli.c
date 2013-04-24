#include "unp.h"
#include "request.h"
#include "stdio.h"
#include "util.h"

int CAN_FIRST_HELP  = TRUE;
int CAN_SECOND_HELP = TRUE;
int CAN_THIRD_HELP  = TRUE;

/*main process*/
void cliProcess(FILE *fp, int sockfd);

/*main player choice*/
void printHelp();

/*validate function*/
int isValidedInputForQuestion(char *input, int len);
int isValidedInputForNavigation(char *input, int len);
int isValidInputForHelp(char *input, int len);

/*print server message*/
int printRecvMessage(Request *req, int sockfd);

int main(int argc, char **argv){
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

void cliProcess(FILE *fp, int sockfd) {
  char  sendline[MAXLINE], recvline[MAXLINE];
  int recv_bytes = 0;
  Request *req;
  int reqType = 8;
  // int received_question = 0;
  printf("+----------------------------------------------------------------+\n");
  printf("|                        DAU TRUONG 100                          |\n");
  printf("+----------------------------------------------------------------+\n");
  printf("Waiting for other players connect to game...\n");
  sendRequest(sockfd, 0, "m", 0);

  while ((req = recvRequest(sockfd)) != NULL) {
    if (printRecvMessage(req, sockfd) == 0) break;
    
    Fgets(sendline, MAXLINE, fp);

    //TODO consider server messages    
    if (req->type == 9 || req->type == 10 
      || req->type == TYPE_SERV_HELP_ANS) {
      while (isValidedInputForQuestion(sendline, MAXLINE) == 0
            && isValidInputForHelp(sendline, MAXLINE) == 0) {
        printf("\nInvalid input, reselect please\nYour choice: ");
        Fgets(sendline, MAXLINE, fp);
      }
      // consider request type !!!
      switch(sendline[0]) {
        case '1': 
          printf("\nYou chose 1st help\n"); 
          reqType = TYPE_CLI_HELP;
          //TODO get answer from main player
          char buffer = sendline[0];
          do{
            printf("Your guess answer: ");
            Fgets(sendline, MAXLINE, fp);
          }while(isValidedInputForQuestion(sendline, MAXLINE) == FALSE);
          sendline[1] = sendline[0];
          sendline[0] = buffer;
          break;
        case '2': 
          printf("\nYou chose 2nd help\n"); 
          reqType = TYPE_CLI_HELP;
          break;
        case '3': 
          printf("\nYou chose 3rd help\n"); 
          reqType = TYPE_CLI_HELP;
          break;
        default:
          printf("\nYour last answer is %c\n", sendline[0]);
          reqType = TYPE_CLI_ANS;
      }
    } else if (req->type == 14) {
      while (isValidedInputForNavigation(sendline, MAXLINE) == 0) {
        printf("Invalid input, reselect please\nYour choice: ");
        Fgets(sendline, MAXLINE, fp);
      }
    } 
    // error    
    else {
      printf("Cannot reach here. Something wrongs\n");
    }
    
    // send feed-back to server
    sendRequest(sockfd, reqType, sendline, 0);
  }
  if (req == NULL)
    err_quit("str_cli: server terminated prematurely");
}

int isValidedInputForNavigation(char *input, int len) {
  int input_len = strlen(input);

  if ( input_len != 2) return FALSE;
  if (input[0] == 'y' || input[0] == 'n') return TRUE;
  
  return FALSE;
}

int isValidedInputForQuestion(char *input, int len) {
  int input_len = strlen(input);

  if ( input_len != 2) return FALSE;
  if ((int)input[0] >= (int)'a' && (int)input[0] <= (int)'c') return TRUE;

  return FALSE;
}

int isValidInputForHelp(char *input, int len){
  int input_len = strlen(input);

  if ( input_len != 2) return FALSE;
  if ((int)input[0] == '1' && CAN_FIRST_HELP)  return TRUE;
  if ((int)input[0] == '2' && CAN_SECOND_HELP) return TRUE;
  if ((int)input[0] == '3' && CAN_THIRD_HELP)  return TRUE;

  return FALSE;
}

void printHelp(){
	if(CAN_FIRST_HELP||CAN_SECOND_HELP||CAN_THIRD_HELP) 
    printf("\n+----------------------- HELP HELP HELP ! -----------------------+");
  if(CAN_FIRST_HELP) {  
    printf("\n| 1. Poll the mob                                                |");
    printf("\n|    Player choose an answer, show number of mob choose the same |");
  }
  if(CAN_SECOND_HELP) { 
    printf("\n| 2. Ask the mob                                                 |");
    printf("\n|    Show 1 incorrect, 1 correct answer.                         |"); //  If no one correct, not shown this option
  }
  if(CAN_THIRD_HELP) {
    printf("\n| 3. Trust the mob                                               |");
    printf("\n|    Show the most popular answer                                |");
  }
  if(CAN_FIRST_HELP||CAN_SECOND_HELP||CAN_THIRD_HELP) 
    printf("\n+----------------------------------------------------------------+");
}

int printRecvMessage(Request *req, int sockfd) {
  switch (req->type) {
    case TYPE_SERV_HELP_ANS: 
      if (req->mess[0] == '1') {
        // consider help 1
        printf("Number mob chose %c is %d\n", req->res[0], req->res[1]);
        CAN_FIRST_HELP  = FALSE;
        printHelp();
        printf("\n\nYour choice: ");
        break;
      } else if (req->mess[0] == '2') {
        //consider help 2
        if(req->res[0] == 0) printf("All player choose correct or incorrect answer.");
        else                 printf("2 answer: %c - %c\n", req->res[0], req->res[1]);
        CAN_SECOND_HELP = FALSE; 
        printHelp();
        printf("\n\nYour choice: ");
        break;
      } else if (req->mess[0] == '3') {
        //consider help 3
        printf("The most popular answer is %c with %d\n", req->res[0], req->res[1]);
        CAN_THIRD_HELP  = FALSE; 
        printHelp();
        printf("\n\nYour choice: "); /*choice*/
        break;
      }
      break;
    case 9: 
      // serv gui cau hoi
      printf("\nThere are %d remaining players\n", req->num);
      printf("You are having %d$ in reward\n", calScore(100-req->num));
      printf("%s\n", req->mess);
      printHelp();
      printf("\nWaiting for answers of others ...\n");
      if ((req = recvRequest(sockfd)) == NULL) {
        printf("server terminated\n");
        return FALSE;
      }
      printf("\nYour choice: "); /*choice*/
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
      return FALSE;
    case 13:
      // main player that bai
      printf("Right answer is %c. ", (char)req->num);
      printf("You failed in this question. %s\n", req->mess);
      return FALSE;
    case 14:
      // tiep tuc hay dung choi
      printf("%s\n\n", req->mess);
      printf("Your choice: ");
      break;
    case 15:
      // tiep tuc hay dung choi
      printf("You chose to stop. Your reward: %d$\n", req->num);
      printf("%s\n", req->mess);
      return FALSE;
  }

  return TRUE;
}
