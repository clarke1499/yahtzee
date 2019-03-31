#include <iostream>
#include <cctype>
#include <string>
#include <cstdlib>
#include <ctime>
#include <stdio.h>

using namespace std;
char terminal_clearline[4];
char terminal_moveup[4];

int players;
string names[4];
int values[5] = { 0 };
bool hold[5] = {};
int totals[4][14];
bool taken[4][14];
int topHalfTotal[4] = { 0 };
const string bottomHalfStrings[8] = {"3 of a kind", "4 of a kind", "Full house",
"Short Straight", "Long Straight", "Yahtzee", "Chance", "Yahtzee Bonus"};
bool bonus = 0;

bool checkYahtzee();

bool isYes(string input){
  for(int i = 0; i < (int)input.length(); i++){
    input.replace(i, 1, string(1, tolower(input[i])));
  }
  if(!input.compare("yes") || input[0] == 'y' ||
     !input.compare("correct"))
    return true;
  return false;
}

bool isNo(string input){
  for(int i = 0; i < input.length(); i++){
    input.replace(i, 1, string(1, tolower(input[i])));
  }
  if(!input.compare("no") || input[0] == 'n' ||
     !input.compare("incorrect"))
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
  printf("%s", terminal_clearline);
  cout << endl;
  cout << "Remaining categories for " << names[player] << endl;
  for(int i = 0; i < 14; i++){
    if(!taken[player][i] && i != 13){
      if(i < 6){
        cout << i << ": " << i + 1 << "s" << endl;
      }
      else{
        cout << i << ": " << bottomHalfStrings[i - 6] << endl;
      }
    }
    if(i == 13 && taken[player][11] && totals[player][11] == 50 &&
        checkYahtzee() && !taken[player][13]){
        cout << i << ": " << bottomHalfStrings[i - 6] << endl;
    }
  }
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
  int in;
  while(category > 13 || taken[player][category]){
    cout << "That category isn't valid, try again" << endl;
    printOptions(player);
    category = -1;
    do{
      cin >> category;
      cin.clear();
      cin.ignore();
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
  cout << "Score recorded = " << totals[player][category] << endl;
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
    cout << names[player] << " scored " << grandTotal << endl;
  }
}

void scores(){
  cout << "Scores on the doors:" << endl;
  for(int player = 0; player < players; player++){
    cout << endl;
    int grandTotal = 0;
    for(int i = 0; i < 14; i++){
      grandTotal += totals[player][i];
    }
    cout << names[player] << " has " << grandTotal << endl;
    cout << "And has " << topHalfTotal[player] << "/63 for their bonus" << endl;
  }
  cout << endl;
}

void startGame(){
  string input;
  int nextTurn;
  int holdIndex;
  int category;
  bool continueGame = true;
  do{
    do{
      for(int player = 0; player < players; player++){
        printf("\033[2J");
        printf("\033[%d;%dH", 0, 0);
        if(nextTurn == 1){
          scores();
        }
        printOptions(player);
        cout << endl;
        cout << names[player] << "'s turn" << endl;
        for(int i = 0; i < 5; i++){
          values[i] = 0;
          hold[i] = false;
        }
        cout << "Dice values: " << endl;
        for(int rolls = 0; rolls < 2; rolls++){
          if(rolls > 0){
            for (int i = 0; i < 7; i++){
              printf("%s", terminal_moveup);
            }
          }
          roll();
          for(int i = 0; i < 5; i++){
            cout << i << ": " << values[i] << (hold[i] ? ", Hold" : "") << endl;
          }
          do{
            cout << "Enter a number to (un)hold or enter 5 to continue" << endl;
            holdIndex = -1;
            do{
              cin >> holdIndex;
              cin.clear();
              cin.ignore();
            }while(holdIndex == -1);
            if(holdIndex >= 5){
              break;
            }
            for (int i = 0; i < 7; i++){
              printf("%s", terminal_moveup);
            }
            hold[holdIndex] = !hold[holdIndex];
            for(int i = 0; i < 5; i++){
              cout << i << ": " << values[i] << (hold[i] ? ", Hold" : "      ") << endl;
            }
          }while(holdIndex < 5);
        }
        //cout << "Dice values: " << endl;
        roll();
        for (int i = 0; i < 7; i++){
          printf("%s", terminal_moveup);
        }
        //cout << endl;
        for(int i = 0; i < 5; i++){
          cout << i << ": " << values[i] << (hold[i] ? ", Hold" : "") << endl;
        }
        printOptions(player);
        category = -1;
        do{
          cin >> category;
          cin.clear();
          cin.ignore();
        }while(category == -1);
        updateScores(category, player);
        cout << "Pass to player " << names[player] << endl;
        cout << "Enter 1 to see the scores and continue or any";
        cout << " other (non-zero) number to just continue" << endl;
        nextTurn = 0;
        do{
          cin >> nextTurn;
          cin.clear();
          cin.ignore();
        }while(nextTurn == 0);
      }
      continueGame = false;
      for(int player = 0; player < players; player++){
        for(int i = 0; i < 13; i++){
          if(!taken[player][i]){
            continueGame = true;
          }
        }
        if(taken[player][11] && totals[player][11] == 50 &&
            !taken[player][13]){
          continueGame = true;
        }
      }
    }while(continueGame);
    endGame();
    cout << "Would you like to play again?" << endl;
    cin >> input;
  }while(isYes(input));
}

int main(int argc, char **argv){
  sprintf(terminal_clearline, "%c[2K", 0x1B);
  sprintf(terminal_moveup, "%c[1A", 0x1B);
  cout << "Welcome to my Yahtzee program" << endl;
  players = 0;
  do{
    cout << "Enter number of players, 4 max" << endl;
    cin >> players;
    cin.clear();
    cin.ignore();
  }while(players == 0);
  for(int i = 0; i < players; i++){
    cout << "Enter the name of player " << i << endl;
    cin >> names[i];
  }
  cout << endl;
  startGame();
}
