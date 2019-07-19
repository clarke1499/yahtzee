#include "Dice.hxx"

Dice::Dice() : value(0), hold(false)
  , gen(std::chrono::high_resolution_clock::now().time_since_epoch().count())
  , distribution(1, 6){
}

Dice::~Dice(){
}

void Dice::roll(){
  if(!hold){
    value = distribution(gen);
  }
}

void Dice::toggleHold(){
  hold = !hold;
}

void Dice::setValue(int v){
  value = v;
}

bool Dice::getHold(){
  return hold;
}

int Dice::getValue(){
  return value;
}

bool operator==(const Dice &a, const Dice &b){
  return a.value == b.value;
}

bool operator!=(const Dice &a, const Dice &b){
  return a.value != b.value;
}

bool operator<(const Dice &a, const Dice &b){
  return a.value < b.value;
}

bool operator<=(const Dice &a, const Dice &b){
  return a.value <= b.value;
}

bool operator>(const Dice &a, const Dice &b){
  return a.value > b.value;
}

bool operator>=(const Dice &a, const Dice &b){
  return a.value >= b.value;
}

bool operator==(const Dice &a, int b){
  return a.value == b;
}
