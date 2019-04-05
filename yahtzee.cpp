#include <iostream>
#include <cctype>
#include <string>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <ncurses.h>

using namespace std;
char terminal_clearline[4];
char terminal_moveup[4];

int players;
char *names[4];
int values[5] = { 0 };
bool hold[5] = {};
int totals[4][14];
bool taken[4][14];
int topHalfTotal[4] = { 0 };
const char *bottomHalfStrings[8] = {"3 of a kind", "4 of a kind", "Full house",
"Short Straight", "Long Straight", "Yahtzee", "Chance", "Yahtzee Bonus"};
bool bonus = 0;
WINDOW *win;

bool checkYahtzee();

bool isYes(char *input){
  string inputStr(input);
  for(int i = 0; i < (int)inputStr.length(); i++){
    inputStr.replace(i, 1, string(1, tolower(inputStr[i])));
  }
  if(!inputStr.compare("yes") || inputStr[0] == 'y' ||
     !inputStr.compare("correct"))
    return true;
  return false;
}

bool isNo(char *input){
  string inputStr(input);
  for(int i = 0; i < inputStr.length(); i++){
    inputStr.replace(i, 1, string(1, tolower(inputStr[i])));
  }
  if(!inputStr.compare("no") || inputStr[0] == 'n' ||
     !inputStr.compare("incorrect"))
    return true;
  return false;
}

void roll(){
  srand(time(0));
  for(int i = 0; i < 5; i++){
    if(!hold[i]){
      values[i] = (rand() % 6 + 1);
    }
  }
}

void printOptions(int player){
  //printf("%s", terminal_clearline);
  //cout << endl;
  wprintw(win, "\n");
  //cout << "Remaining categories for " << names[player] << endl;
  wprintw(win, "Remaining categories for %s\n", names[player]);
  for(int i = 0; i < 14; i++){
    if(!taken[player][i] && i != 13){
      if(i < 6){
        //cout << i << ": " << i + 1 << "s" << endl;
        wprintw(win, "%d: %ds\n", i, i + 1);
      }
      else{
        //cout << i << ": " << bottomHalfStrings[i - 6] << endl;
        wprintw(win, "%d: %s\n", i, bottomHalfStrings[i - 6]);
      }
    }
    if(i == 13 && taken[player][11] && totals[player][11] == 50 &&
        checkYahtzee() && !taken[player][13]){
        wprintw(win, "%d: %s\n", i, bottomHalfStrings[i - 6]);
        //cout << i << ": " << bottomHalfStrings[i - 6] << endl;
    }
  }
  wrefresh(win);
}

int count(int diceValue){
  int total = 0;
  for(int i = 0; i < 5; i++){
    if(values[i] == diceValue){
      total += diceValue;
    }
  }
  return total;
}

int count(){
  int total = 0;
  for(int i = 0; i < 5; i++){
    total += values[i];
  }
  return total;
}

bool checkThree(){
  int num[6] = { 0 };
  for(int i = 0; i < 5; i++){
    if(++num[values[i] - 1] == 3){
      return true;
    }
  }
  return false;
}

bool checkFour(){
  int num[6] = { 0 };
  for(int i = 0; i < 5; i++){
    if(++num[values[i] - 1] == 4){
      return true;
    }
  }
  return false;
}

bool checkFull(){
  int num[6] = { 0 };
  bool two, three;
  two = three = false;
  for(int i = 0; i < 5; i++){
    num[values[i] - 1]++;
  }
  for(int i = 0; i < 6; i++){
    if(num[i] == 2){
      two = true;
    }
    if(num[i] == 3){
      three = true;
    }
    if(two && three){
      return true;
    }
  }
  return false;
}

bool checkStraight(int target){
  int num[6] = { 0 };
  int inARow = 0;
  for(int i = 0; i < 5; i++){
    num[values[i] - 1]++;
  }
  for(int i = 0; i < 6; i++){
    if(num[i] > 2){
      return false;
    }
    if(num[i] > 0){
      if(++inARow == target){
        return true;
      }
    }
    else{
      inARow = 0;
    }
  }
  return false;
}

bool checkYahtzee(){
  int num[6] = { 0 };
  for(int i = 0; i < 5; i++){
    if(++num[values[i] - 1] == 5){
      return true;
    }
  }
  return false;
}

bool checkBonus(int player){
  if(taken[player][11] && totals[player][11] == 50 && checkYahtzee()){
    return true;
  }
  return false;
}

void updateScores(int category, int player){
  while(category > 13 || taken[player][category]){
    wprintw(win, "That category isn't valid, try again\n");
    wrefresh(win);
    printOptions(player);
    category = -1;
    do{
      char *categoryInput = (char *) malloc(sizeof(char) * 5);
      wgetstr(win, categoryInput);
      category = strtol(categoryInput, NULL, 10);
      free(categoryInput);
    }while(category == -1);
  }
  switch(category){
    case 0: totals[player][category] = count(1);
            taken[player][category] = true;
            topHalfTotal[player] += totals[player][category];
            break;
    case 1: totals[player][category] = count(2);
            taken[player][category] = true;
            topHalfTotal[player] += totals[player][category];
            break;
    case 2: totals[player][category] = count(3);
            taken[player][category] = true;
            topHalfTotal[player] += totals[player][category];
            break;
    case 3: totals[player][category] = count(4);
            taken[player][category] = true;
            topHalfTotal[player] += totals[player][category];
            break;
    case 4: totals[player][category] = count(5);
            taken[player][category] = true;
            topHalfTotal[player] += totals[player][category];
            break;
    case 5: totals[player][category] = count(6);
            taken[player][category] = true;
            topHalfTotal[player] += totals[player][category];
            break;
    case 6: if(checkThree()){
              totals[player][category] = count();
            }
            taken[player][category] = true;
            break;
    case 7: if(checkFour()){
              totals[player][category] = count();
            }
            taken[player][category] = true;
            break;
    case 8: if(checkFull()){
              totals[player][category] = 25;
            }
            taken[player][category] = true;
            break;
    case 9: if(checkStraight(4)){
              totals[player][category] = 30;
            }
            taken[player][category] = true;
            break;
    case 10: if(checkStraight(5)){
               totals[player][category] = 40;
             }
             taken[player][category] = true;
            break;
    case 11: if(checkYahtzee()){
               totals[player][category] = 50;
             }
             taken[player][category] = true;
            break;
    case 12: totals[player][category] = count();
             taken[player][category] = true;
             break;
    case 13: if(checkBonus(player)){
               totals[player][category] = 100;
             }
             taken[player][category] = true;
             break;
  }
  //cout << "Score recorded = " << totals[player][category] << endl;
  wprintw(win, "Score recorded = %d\n", totals[player][category]);
  wrefresh(win);
}

void endGame(){
  for(int player = 0; player < players; player++){
    int grandTotal = 0;
    for(int i = 0; i < 14; i++){
      grandTotal += totals[player][i];
    }
    if(topHalfTotal[player] >= 63){
      grandTotal += 35;
    }
    wprintw(win, "%s scored %d\n", grandTotal);
    wrefresh(win);
  }
}

void scores(){
  //cout << "Scores on the doors:" << endl;
  wprintw(win, "Scores on the doors:\n");
  for(int player = 0; player < players; player++){
    //cout << endl;
    wprintw(win, "\n");
    int grandTotal = 0;
    for(int i = 0; i < 14; i++){
      grandTotal += totals[player][i];
    }
    //cout << names[player] << " has " << grandTotal << endl;
    //cout << "And has " << topHalfTotal[player] << "/63 for their bonus" << endl;
    wprintw(win, "%s has %d\n", names[player], grandTotal);
    wprintw(win, "And has %d/63 for their bonus\n", topHalfTotal[player]);
  }
  wprintw(win, "\n");
  wrefresh(win);
}

void takeTurn(int player){
  int nextTurn;
  int holdIndex;
  int category;
  wclear(win);
  wrefresh(win);
  scores();
  printOptions(player);
  wprintw(win, "\n%s's turn\n", names[player]);
  wrefresh(win);
  for(int i = 0; i < 5; i++){
    values[i] = 0;
    hold[i] = false;
  }
  wprintw(win, "Dice values:\n");
  wrefresh(win);
  int y, x;
  getyx(win, y, x);
  WINDOW *diceWin = newwin(6, 80, y + 1, x);
  for(int rolls = 0; rolls < 2; rolls++){
    wclear(diceWin);
    wrefresh(diceWin);
    roll();
    for(int i = 0; i < 5; i++){
      wprintw(diceWin, "%d: %d", i, values[i]);
      if(hold[i]){
        wprintw(diceWin, ", Hold");
      }
      wprintw(diceWin, "\n");
    }
    wrefresh(diceWin);
    do{
      wprintw(diceWin, "Enter a number to (un)hold or enter 5 to continue\n");
      wrefresh(diceWin);
      holdIndex = -1;
      do{
        holdIndex = getch();
      }while(holdIndex == -1);
      holdIndex -= 48;
      if(holdIndex >= 5){
        break;
      }
      hold[holdIndex] = !hold[holdIndex];
      wclear(diceWin);
      wrefresh(diceWin);
      for(int i = 0; i < 5; i++){
        wprintw(diceWin, "%d: %d", i, values[i]);
        if(hold[i]){
          wprintw(diceWin, ", Hold");
        }
        wprintw(diceWin, "\n");
      }
      wrefresh(diceWin);
    }while(holdIndex < 5);
  }
  roll();
  wclear(diceWin);
  wrefresh(diceWin);
  for(int i = 0; i < 5; i++){
    wprintw(diceWin, "%d: %d", i, values[i]);
    if(hold[i]){
      wprintw(diceWin, ", Hold");
    }
    wprintw(diceWin, "\n");
  }
  wrefresh(diceWin);
  delwin(diceWin);
  wmove(win, y + 6, x);
  printOptions(player);
  nocbreak();
  echo();
  category = -1;
  do{
    char *categoryInput = (char *) malloc(sizeof(char) * 5);
    wgetstr(win, categoryInput);
    category = strtol(categoryInput, NULL, 10);
    free(categoryInput);
  }while(category == -1);
  updateScores(category, player);
  wprintw(win, "Pass to %s\n", names[(player + 1) % players]);
  wprintw(win, "Press Enter to continue\n");
  wrefresh(win);
  noecho();
  cbreak();
  getch();
}

void startGame(){
  char *input = NULL;
  bool continueGame[4];
  bool loop = false;
  do{
    free(input);
    fill(continueGame, continueGame + 4, true);
    do{
      for(int player = 0; player < players; player++){
        if(continueGame[player]){
          takeTurn(player);
          continueGame[player] = false;
          for(int i = 0; i < 13; i++){
            if(!taken[player][i]){
              continueGame[player] = true;
            }
          }
        }
      }
      loop = false;
      for(int player = 0; player < players; player++){
        loop = loop || continueGame[player];
      }
    }while(loop);
    endGame();
    for(int player = 0; player < 4; player++){
      for(int i = 0; i < 14; i++){
        totals[player][i] = 0;
        taken[player][i] = false;
      }
      topHalfTotal[player] = 0;
    }
    nocbreak();
    echo();
    input = (char *) malloc(80 * sizeof(char));
    wgetstr(win, input);
    noecho();
    cbreak();
  }while(isYes(input));
  free(input);
}

int main(int argc, char **argv){
  initscr();
  cbreak();
  noecho();
  clear();
  refresh();
  int h, w;
  getmaxyx(stdscr, h, w);
  win = newwin(h, w, 0, 0);
  players = 0;
  do{
    wclear(win);
    wrefresh(win);
    waddstr(win, "Welcome to my Yahtzee program\n");
    waddstr(win, "Enter number of players, 4 max\n");
    wrefresh(win);
    players = getch();
  }while(players < 49 || players > 52);
  players -= 48;
  for(int i = 0; i < players; i++){
    wprintw(win, "Enter name of player %d\n", i);
    wrefresh(win);
    nocbreak();
    echo();
    names[i] = (char *) malloc(sizeof(char) * 80);
    wgetstr(win, names[i]);
  }
  noecho();
  cbreak();
  startGame();
  delwin(win);
  endwin();
  for(int i = 0; i < players; i++){
    free(names[i]);
  }
}
