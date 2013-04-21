#include "util.h"
#include "string.h"

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
    default:
      return 0;
      break;
  }
}

void initQuestion(char **questions, char *answers){
  strcpy(questions[0], "cau hoi 1: 1+1=?\na.2\nb.3\nc.4"); answers[0] = 'a';
  strcpy(questions[1], "cau hoi 2: 2+1=?\na.2\nb.3\nc.4"); answers[1] = 'b';
  strcpy(questions[2], "cau hoi 3: 3+1=?\na.2\nb.3\nc.4"); answers[2] = 'c';
}
