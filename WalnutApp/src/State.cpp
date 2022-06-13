#include "State.h"
#include<map>
State::State()
{
	this->name = std::to_string(statesCount);
	this->isAccepted = false;
	this->number = statesCount;
	statesCount++;
}

State::~State()
{
	statesCount = 0;
}

State::State(std::string& name, bool isAccepted)
{
	this->name = name;
	this->isAccepted = isAccepted;
}

void State::addNewTransition(Transition* transition)
{
	transitions.push_back(transition);
}

void State::setAcceptance(bool isAccepted)
{
	this->isAccepted = isAccepted;
}

std::vector<Transition*> State::getTransitions()
{
	return this->transitions;
}

std::string State::getName()
{
	return this->name;
}

int State::reachedFrom()
{
    return this->reachedfrom;
}

void State::incReachedFrom()
{
    this->reachedfrom++;
}

int State::getNumber()
{
    return this->number;
}

int State::getStatesCount()
{
    return statesCount;
}
int State::statesCount = 0;
