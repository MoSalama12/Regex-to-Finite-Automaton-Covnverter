#pragma once

#include <unordered_map>
#include <queue>
#include <stack>
#include <string>
#include<set>

class regularExpression
{
public:
	regularExpression(std::string& proposition);
	std::queue<std::string> parseString();
	bool isHigherPrecedence(std::string op, std::stack<std::string>& s);
	std::set<char> getSymbols();

public:
	std::string proposition;
	std::unordered_map<std::string, int> operatorPrecedence;
};
