yahtzee: yahtzee.cxx Dice.cxx Dice.hxx
	g++ -c Dice.cxx -o Dice.o
	g++ -c yahtzee.cxx -o yahtzee.o
	g++ -g -lncurses yahtzee.o Dice.o -o yahtzee
