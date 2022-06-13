#pragma once
#include <vector>
#include "State.h"
#include "Utility.h"
#include <queue>
#include <stack>
#include <vector>
#include <map>

class NFA
{
public:
	void constructNFA(std::queue<std::string> postfixRegex);
	std::vector < std::vector < std::string>> NFA::constructTransitionTable();
	void setStartState();
	State getStartState();
	std::vector<int> ** constructTransitionTable2();
	~NFA();
private:
	std::vector<State*> states;
	State startState;
	std::vector<State*> acceptedStates;
	std::vector<int> ** nfaTable;
};
