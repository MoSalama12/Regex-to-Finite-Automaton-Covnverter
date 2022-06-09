#pragma once
#include "State.h"

class State;

class Transition
{
public:
	Transition(State* from, State* to, const char symbol);
	State* getFrom();
	State* getTo();
	char getSymbol();
private:
	State* from, * to;
	char symbol;
};
