#pragma once
#include <iostream>

class Utility
{
public:
	static bool isOperator(std::string ch)
	{
		return ch == "|" || ch == "+" || ch == "*" || ch == ".";
	}
};
