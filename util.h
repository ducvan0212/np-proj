int calScore(int number_of_defeated_players);

int firstHelp(char *answers, char mainPlayerAnswer, int *remaining_players, int len);
int* secondHelp(char *answers, char correctAnswer, int *remaining_players, int len);
int* thirdHelp(char *answers, int *remaining_players, int len);

void initQuestion(char *questions[100], char *answers);
