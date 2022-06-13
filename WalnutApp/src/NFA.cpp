#include "NFA.h"

const char epsilon = '$';

NFA::~NFA()
{
	for (State* state : states)
	{
		for (Transition* transition : state->getTransitions())
		{
			delete transition;
		}
		delete state;
	}
}

void NFA::constructNFA(std::queue<std::string> postfixRegex)
{
	std::stack<State*> s;
	while(!postfixRegex.empty())
	{
		//std::cout << postfixRegex.front() << " ";
		if (!Utility::isOperator(postfixRegex.front()))
		{
			State* s1 = new State();
			State* s2 = new State();
			char sym =  postfixRegex.front()[0];
			Transition* t = new Transition(s1, s2, sym);
			s1->addNewTransition(t);
			states.push_back(s1);
			states.push_back(s2);
			s.push(s1);
			s.push(s2);
		}
		else if(postfixRegex.front() == "|")
		{
			State* s1 = new State();
			State* s2 = new State();
			// link the second option with the end state
			State* lastState = s.top();
			Transition* t1 = new Transition(lastState, s2, epsilon);
			lastState->addNewTransition(t1);
			s.pop();
			// link the second option with the start state
			lastState = s.top();
			Transition* t2 = new Transition(s1, lastState, epsilon);
			s1->addNewTransition(t2);
			s.pop();
			// link the first option with the end state
			lastState = s.top();
			Transition* t3 = new Transition(lastState, s2, epsilon);
			lastState->addNewTransition(t3);
			s.pop();
			// link the first option with the start state
			lastState = s.top();
			Transition* t4 = new Transition(s1, lastState, epsilon);
			s1->addNewTransition(t4);
			s.pop();
			states.push_back(s1);
			states.push_back(s2);
			s.push(s1);
			s.push(s2);
		}
		else if (postfixRegex.front() == ".")
		{
			State* endState = s.top();
			s.pop();
			State* right = s.top();
			s.pop();
			State* left = s.top();
			s.pop();
			Transition* t = new Transition(left, right, epsilon);
			left->addNewTransition(t);
			s.push(endState);
		}
		else
		{
			State* newStart = new State(), * newEnd = new State();
			State* oldEnd = s.top();
			s.pop();
			State* oldStart = s.top();
			s.pop();
			if (postfixRegex.front() != "+")
			{
				Transition* t1 = new Transition(newStart, newEnd, epsilon);
				newStart->addNewTransition(t1);
			}
			Transition* t2 = new Transition(oldEnd, oldStart, epsilon);
			oldEnd->addNewTransition(t2);
			Transition* t3 = new Transition(newStart, oldStart, epsilon);
			newStart->addNewTransition(t3);
			Transition* t4 = new Transition(oldEnd, newEnd, epsilon);
			oldEnd->addNewTransition(t4);
			states.push_back(newStart);
			states.push_back(newEnd);
			s.push(newStart);
			s.push(newEnd);
		}
		postfixRegex.pop();
	}
	s.top()->setAcceptance(true);
}

std::vector < std::vector < std::string>> NFA::constructTransitionTable()
{
	std::cout << "\nSTATE\t\tLINKED TO\t\tWITH SYMBOL\n";

	std::vector < std::vector < std::string>> output;
	for (int i = 0; i < states.size(); i++)
	{
		std::vector<Transition*> transitions = states[i]->getTransitions();
		for (int j = 0; j < transitions.size(); j++)
		{
			std::vector<std::string> collect  ;
			collect.push_back(states[i]->getName());
			collect.push_back(transitions[j]->getTo()->getName());
			std::string x (1, transitions[j]->getSymbol());
			collect.push_back(x);
			output.push_back(collect);

			std::cout << " " << states[i]->getName() << "\t\t  ";
			std::cout << transitions[j]->getTo()->getName() << "\t\t\t  ";
			std::cout << transitions[j]->getSymbol() << "\n";

			transitions[j]->getTo()->incReachedFrom();
		}
	}
	return output;
}


std::vector<int> ** NFA::constructTransitionTable2()
{
    std::vector<int> ** statesVector = new std::vector<int> *[20];
    for(int i=0; i<20; i++)
    {
        statesVector[i] = new std::vector<int> [10];
    }

	for (int i = 0; i < states.size(); i++)
	{
	    std::vector<Transition*> transitions = states[i]->getTransitions();
		for (int j = 0; j < transitions.size(); j++)
		{
		    int cur = states[i]->getNumber(), to = transitions[j]->getTo()->getNumber(), symbol = transitions[j]->getSymbol() - 'a';
		    if(symbol < 0) symbol = -1;
		    statesVector[cur][symbol+1].push_back(to);
			transitions[j]->getTo()->incReachedFrom();
		}
	}
	this->nfaTable = statesVector;
	return statesVector;
}


void NFA::setStartState()
{
    for (int i = 0; i < states.size(); i++)
	{
		if(states[i]->reachedFrom() == 0)
        {
            startState = *states[i];
        }
	}
}

State NFA::getStartState()
{
    return startState;
}
