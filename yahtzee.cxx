#include "Dice.hxx"
#include <iostream>
#include <cctype>
#include <string>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <ncurses.h>
#include <csignal>


using namespace std;

int players;
char *names[4];
vector<Dice> dice(5);
//int values[5] = { 0 };
//bool hold[5] = {};
int totals[4][14];
bool taken[4][14];
int topHalfTotal[4] = { 0 };
static const char *bottomHalfStrings[8] = {"3 of a kind", "4 of a kind", "Full house",
"Short Straight", "Long Straight", "Yahtzee", "Chance", "Yahtzee Bonus"};
bool bonus = 0;
WINDOW *win;
WINDOW *scoresWin;
WINDOW *categoriesWins[4];

bool checkYahtzee();

void resizeHandle(int signum){
  int h, w;
  getmaxyx(stdscr, h, w);
  resizeterm(h, w);
  refresh();
  wresize(win, h, w / 2);
  wrefresh(win);

  wresize(scoresWin, players * 3 + 1, w - 58);
  wrefresh(scoresWin);
  int categoriesWinHeight, categoriesWinWidth;
  switch(players){
    case 1: categoriesWinHeight = h - 4;
            categoriesWinWidth = w - 58;
            break;
    case 2: categoriesWinHeight = h - 7;
            categoriesWinWidth = (w - 58) / 2;
            break;
    case 3: categoriesWinHeight = (h - 10) / 2;
            categoriesWinWidth = (w - 58) / 2;
            break;
    case 4: categoriesWinHeight = (h - 13) / 2;
            categoriesWinWidth = (w - 58) / 2;
  }
  for(int i = 0; i < players; i++){
    wresize(categoriesWins[i], categoriesWinHeight, categoriesWinWidth);
    wrefresh(categoriesWins[i]);
  }
}

void sigIntHandle(int signum){
  for(int i = 0; i < players; i++){
    free(names[i]);
    delwin(categoriesWins[i]);
  }
  delwin(win);
  endwin();
  //_nc_free_and_exit(signum);
  exit(signum);
}

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

//void roll(){
//  srand(time(0));
//  for(int i = 0; i < 5; i++){
//    if(!hold[i]){
//      values[i] = (rand() % 6 + 1);
//    }
//  }
//}

void printOptions(int player){
  wmove(categoriesWins[player], 0, 0);
  wprintw(categoriesWins[player], "\n");
  wprintw(categoriesWins[player], "Remaining categories for %s\n", names[player]);
  for(int i = 0; i < 14; i++){
    if(!taken[player][i] && i != 13){
      if(i < 6){
        wprintw(categoriesWins[player], "%d: %ds\n", i + 1, i + 1);
      }
      else{
        wprintw(categoriesWins[player], "%d: %s\n", i + 1, bottomHalfStrings[i - 6]);
      }
    }
    if(i == 13 && taken[player][11] && totals[player][11] == 50 &&
        checkYahtzee() && !taken[player][13]){
        wprintw(categoriesWins[player], "%d: %s\n", i + 1, bottomHalfStrings[i - 6]);
    }
  }
  wclrtobot(categoriesWins[player]);
  wrefresh(categoriesWins[player]);
}

int count(int diceValue){
  int total = 0;
  for(int i = 0; i < 5; i++){
    if(dice[i] == diceValue){
      total += diceValue;
    }
  }
  return total;
}

int count(){
  int total = 0;
  for(int i = 0; i < 5; i++){
    total += dice[i].getValue();
  }
  return total;
}

bool checkThree(){
  int num[6] = { 0 };
  for(int i = 0; i < 5; i++){
    if(++num[dice[i].getValue() - 1] == 3){
      return true;
    }
  }
  return false;
}

bool checkFour(){
  int num[6] = { 0 };
  for(int i = 0; i < 5; i++){
    if(++num[dice[i].getValue() - 1] == 4){
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
    num[dice[i].getValue() - 1]++;
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
    num[dice[i].getValue() - 1]++;
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
  if(dice[0].getValue() == 0){
    return false;
  }
  int num[6] = { 0 };
  for(int i = 0; i < 5; i++){
    if(++num[dice[i].getValue() - 1] == 5){
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
  while(category > 14 || taken[player][category - 1]){
    wprintw(win, "That category isn't valid, try again\n");
    wrefresh(win);
    printOptions(player);
    category = -1;
    do{
      char *categoryInput = (char *) malloc(sizeof(char) * 5);
      wgetstr(win, categoryInput);
      category = strtol(categoryInput, NULL, 10);
      free(categoryInput);
    }while(category == -1 || category > 14 || category < 1);
  }
  category--;
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
    wprintw(win, "%s scored %d\n", names[player], grandTotal);
    wrefresh(win);
  }
}

void scores(){
  wclear(scoresWin);
  wprintw(scoresWin, "Scores on the doors:\n");
  for(int player = 0; player < players; player++){
    wprintw(scoresWin, "\n");
    int grandTotal = 0;
    for(int i = 0; i < 14; i++){
      grandTotal += totals[player][i];
    }
    wprintw(scoresWin, "%s has %d\n", names[player], grandTotal);
    wprintw(scoresWin, "And has %d/63 for their bonus\n", topHalfTotal[player]);
  }
  wprintw(scoresWin, "\n");
  wrefresh(scoresWin);
}

void takeTurn(int player){
  int nextTurn;
  int holdIndex;
  int category;
  wclear(win);
  wrefresh(win);
  wprintw(win, "%s's turn\n", names[player]);
  wrefresh(win);
  for(int i = 0; i < 5; i++){
    if(dice[i].getHold()){
      dice[i].toggleHold();
    }
  }
  wprintw(win, "Dice values:\n");
  wrefresh(win);
  int y, x;
  getyx(win, y, x);
  WINDOW *diceWin = newwin(7, 50, y + 1, x);
  for(int rolls = 0; rolls < 2; rolls++){
    wmove(diceWin, 0, 0);
    wrefresh(diceWin);
    wprintw(diceWin, "Roll %d\n", rolls + 1);
    for(int i = 0; i < 5; i++){
      dice[i].roll();
    }
    for(int i = 0; i < 5; i++){
      wprintw(diceWin, "%d: %d", i + 1, dice[i].getValue());
      if(dice[i].getHold()){
        wprintw(diceWin, ", Hold");
      }
      wprintw(diceWin, "\n");
    }
    wrefresh(diceWin);
    do{
      wprintw(diceWin, "Enter a number to (un)hold or enter 6 to continue");
      wrefresh(diceWin);
      holdIndex = -1;
      do{
        holdIndex = getch();
      }while(holdIndex == -1 || holdIndex < 49 || holdIndex > 58);
      holdIndex -= 49;
      if(holdIndex >= 5){
        break;
      }
      dice[holdIndex].toggleHold();
      wmove(diceWin, 1, 0);
      wrefresh(diceWin);
      for(int i = 0; i < 5; i++){
        wprintw(diceWin, "%d: %d", i + 1, dice[i].getValue());
        if(dice[i].getHold()){
          wprintw(diceWin, ", Hold");
        }
        wprintw(diceWin, "\n");
      }
      wrefresh(diceWin);
    }while(holdIndex < 6);
  }
  for(int i = 0; i < 5; i++){
    dice[i].roll();
  }
  wmove(diceWin, 0, 0);
  wrefresh(diceWin);
  wprintw(diceWin, "Roll 3\n");
  for(int i = 0; i < 5; i++){
    wprintw(diceWin, "%d: %d", i + 1, dice[i].getValue());
    if(dice[i].getHold()){
      wprintw(diceWin, ", Hold");
    }
    wprintw(diceWin, "\n");
  }
  wrefresh(diceWin);
  delwin(diceWin);
  wmove(win, y + 7, 0);
  nocbreak();
  echo();
  printOptions(player);
  wprintw(win, "\nSelect a category from your list to the right\n");
  category = -1;
  do{
    char *categoryInput = (char *) malloc(sizeof(char) * 5);
    wgetstr(win, categoryInput);
    category = strtol(categoryInput, NULL, 10);
    free(categoryInput);
  }while(category == -1 || category > 14 || category < 1);
  updateScores(category, player);
  scores();
  for(int i = 0; i < 5; i++){
    dice[i].setValue(0);
  }
  printOptions(player);
  if(players > 1){
    wprintw(win, "Pass to %s\n", names[(player + 1) % players]);
  }
  wprintw(win, "Press Enter to continue\n");
  wrefresh(win);
  noecho();
  cbreak();
  getch();
}

void startGame(){
  for(int i = 0; i < 5; i++){
    dice[i].roll();
  }
  char *input = NULL;
  bool continueGame[4];
  bool loop = false;
  do{
    free(input);
    fill(continueGame, continueGame + 4, true);
    scores();
    for(int i = 0; i < players; i++){
      printOptions(i);
    }
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
    wprintw(win, "Would you like to play again? (y/n)\n");
    wrefresh(win);
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
  signal(SIGINT, sigIntHandle);
  signal(SIGWINCH, resizeHandle);
  initscr();
  cbreak();
  noecho();
  clear();
  refresh();
  int h, w;
  getmaxyx(stdscr, h, w);
  win = newwin(h, 55, 0, 0);
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

  scoresWin = newwin(players * 3 + 1, w - 55, 0, 55);
  int categoriesWinHeight, categoriesWinWidth;
  switch(players){
    case 1: categoriesWinHeight = h - 4;
            categoriesWinWidth = w - 55;
            break;
    case 2: categoriesWinHeight = h - 7;
            categoriesWinWidth = (w - 55) / 2;
            break;
    case 3: categoriesWinHeight = (h - 10) / 2;
            categoriesWinWidth = (w - 55) / 2;
            break;
    case 4: categoriesWinHeight = (h - 13) / 2;
            categoriesWinWidth = (w - 55) / 2;
  }
  for(int i = 0; i < players; i++){
    switch(i){
      case 0: categoriesWins[i] = newwin(categoriesWinHeight,
                  categoriesWinWidth, players * 3 + 1, 55);
              break;
      case 1: categoriesWins[i] = newwin(categoriesWinHeight,
                  categoriesWinWidth, players * 3 + 1,
                  55 + categoriesWinWidth);
              break;
      case 2: categoriesWins[i] = newwin(categoriesWinHeight,
                  categoriesWinWidth, (players * 3 + 1) + categoriesWinHeight,
                  55);
              break;
      case 3: categoriesWins[i] = newwin(categoriesWinHeight,
                  categoriesWinWidth, (players * 3 + 1) + categoriesWinHeight,
                  55 + categoriesWinWidth);
              break;
    }
    wprintw(win, "Enter name of player %d\n", i + 1);
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
  for(int i = 0; i < players; i++){
    free(names[i]);
    delwin(categoriesWins[i]);
  }
  endwin();
}
