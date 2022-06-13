#include "regularExpression.h"

#include <iostream>

regularExpression::regularExpression(std::string& proposition)
{

	for (size_t i = 0; i < proposition.size(); i++)
	{
		if (proposition[i] != ' ')
			this->proposition += proposition[i];

		if (proposition[i] == '*' || proposition[i] == '+')
		{
			this->proposition += ')';
			bool foundRightBracket = false;

			for (size_t j = this->proposition.size()-2; j >= 0; j--)
			{
				if (this->proposition[j] == ')')
					foundRightBracket = true;

				if (foundRightBracket)
				{
					if (this->proposition[j] == '(')
					{
						this->proposition.insert(j, "(");
						break;
					}
				}
				else {
					if (this->proposition[j] != '+' && this->proposition[j] != '*' && this->proposition[j] != '|' && this->proposition[j] != '.')
					{
						this->proposition.insert(j, "(");
						break;
					}
				}

			}
		}
	}

	operatorPrecedence["."] = operatorPrecedence["|"] = 2;
	operatorPrecedence["*"] = operatorPrecedence["+"] = 1;
	operatorPrecedence["("] = 0;
}


bool regularExpression::isHigherPrecedence(std::string op, std::stack<std::string>& s)
{
	{
		if (!s.empty())
		{
			return operatorPrecedence[op] < operatorPrecedence[s.top()];
		}
		return false;
	}
}

std::queue<std::string> regularExpression::parseString()
{
	std::queue<std::string> outputQueue;
	std::stack<std::string> operatorStack;
	for (size_t i = 0; i < proposition.size(); i++)
	{
		if (proposition[i] == ' ')
		{
			continue;
		}

		else if (isalpha(proposition.at(i)) && tolower(proposition[i]) != 'v')
		{
			outputQueue.push(std::string(1, proposition[i]));
		}
		else if (proposition[i] == '(')
		{
			operatorStack.push(std::string(1, proposition[i]));
		}
		else if (proposition[i] == ')')
		{
			while (operatorStack.top() != "(")
			{
				outputQueue.push(operatorStack.top());
				operatorStack.pop();
			}
			operatorStack.pop();
		}
		else
		{
			std::string temp = std::string(1, proposition[i]);
			if (proposition[i] == '=') // signature for implies
			{
				temp = proposition.substr(i, 2);
				i++;
			}
			else if (proposition[i] == '<') // signature for bicondition
			{
				temp = proposition.substr(i, 3);
				i += 2;
			}
			while (isHigherPrecedence(temp, operatorStack))
			{
				outputQueue.push(operatorStack.top());
				operatorStack.pop();
			}
			operatorStack.push(temp);
		}
	}
	while (!operatorStack.empty())
	{
		outputQueue.push(operatorStack.top());
		operatorStack.pop();
	}
	return outputQueue;
}

std::set<char> regularExpression::getSymbols()
{
    std::set<char> symbols;
    for(auto ch: proposition)
    {
        if(isalpha(ch))
        {
            symbols.insert(ch);
        }
    }
    return symbols;
}
