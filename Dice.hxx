#include <chrono>
#include <random>
class Dice{
  private:
    int value;
    bool hold;
    std::uniform_int_distribution<int> distribution;
    std::default_random_engine gen;
  public:
    Dice();
    ~Dice();
    void roll();
    void toggleHold();
    void setValue(int);
    bool getHold();
    int getValue();

    friend bool operator==(const Dice&, const Dice&);
    friend bool operator!=(const Dice&, const Dice&);
    friend bool operator<(const Dice&, const Dice&);
    friend bool operator<=(const Dice&, const Dice&);
    friend bool operator>(const Dice&, const Dice&);
    friend bool operator>=(const Dice&, const Dice&);
    friend bool operator==(const Dice&, int);
};
