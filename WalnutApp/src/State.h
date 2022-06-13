#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "Transition.h"

class Transition;

class State
{
public:
	State();
	~State();
	State(std::string& name, bool isAccepted = false);
	void addNewTransition(Transition* transition);
	void setAcceptance(bool isAccepted);
	std::vector<Transition*> getTransitions();
	std::string getName();
	int reachedFrom();
	void incReachedFrom();
	int getNumber();
	int getStatesCount();
private:
	std::string name;
	std::vector<Transition*> transitions;
	int number;
	bool isAccepted;
	static int statesCount;
	int reachedfrom = 0;
};
