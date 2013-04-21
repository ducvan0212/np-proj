#include "util.h"
#include "string.h"

#define OFF 0
#define ON  1

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

int firstHelp(char *answers, char mainPlayerAnswer, int *remaining_players, int len){
  int i, j;
  j = 0;
  for (i = 0; i < len; ++i)
    if (remaining_players[i] == ON)
      if(answers[i] == mainPlayerAnswer)
        j++;
  return j;
}

int* secondHelp(char *answers, char correctAnswer, int *remaining_players, int len){
  int *j = (int*)malloc(3*sizeof(int));
  j[0] = 0;  // invalid or not 
  j[1] = 0;  // correct
  j[2] = 0;  // incorrect

  int i = 0;
  for (i = 0; i < len; ++i)
    if (remaining_players[i] == ON){
      if(answers[i] == correctAnswer) j[1] = answers[i]; 
      else if(answers[i] != correctAnswer) j[2] = answers[i];
    }
  if(j[1] != 0 && j[2] != 0) j[0] = 1;
  printf("Check2: %d %d\n", j[1], j[2]); // fucking buggy ?
  return j;
}

int* thirdHelp(char *answers, int *remaining_players, int len){
  int *j = (int*)malloc(2*sizeof(int));
  j[0] = 'a';
  j[1] = 0;

  int s[3] = {0,0,0};
  int i = 0;
  for (i = 0; i < len; ++i)
    if (remaining_players[i] == ON){
      s[answers[i] - 'a']++;
      if(s[answers[i] - 'a'] > j[1]){
        j[0] = answers[i];              
        j[1] = s[answers[i] - 'a']; 
      }
    }
  printf("Check3: %c %d\n", j[0], j[1]); // fucking buggy ?
  return j;
}

void initQuestion(char **questions, char *answers){
  strcpy(questions[0], "cau hoi 1: 1+1=?\na.2\nb.3\nc.4"); answers[0] = 'a';
  strcpy(questions[1], "cau hoi 2: 2+1=?\na.2\nb.3\nc.4"); answers[1] = 'b';
  strcpy(questions[2], "cau hoi 3: 3+1=?\na.2\nb.3\nc.4"); answers[2] = 'c';
}
