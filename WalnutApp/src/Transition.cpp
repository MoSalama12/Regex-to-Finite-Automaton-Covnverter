#include "State.h"

Transition::Transition(State* from, State* to, const char symbol)
{
	this->from = from;
	this->to = to;
	this->symbol = symbol;
}

State* Transition::getFrom()
{
	return this->from;
}

State* Transition::getTo()
{
	return this->to;
}

char Transition::getSymbol()
{
	return this->symbol;
}
