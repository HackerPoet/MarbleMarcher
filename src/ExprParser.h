#pragma once
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include <map>

bool replace(std::string& str, const std::string& from, const std::string& to);

class ExprParser
{
public:

	enum TOKENTYPE { NOTHING = -1, delimiter, NUMBER, VARIABLE, FUNCTION, UNKNOWN };

	enum OPERATION_ID {
		PLUS, MINUS,                    // level 1
		INV, MULTIPLY, DIVIDE,                 // level 2
		POW,                             // level 3
		FACTORIAL,                       //level 4
		ABS, EXP, SIGN, SQRT, LOG, SIN, COS, TANH, THETA, //level 5
		LBRACK
	};                      // level 6

	TOKENTYPE token_type;

	char token[256];
	char* expression;
	char* e;
	int N;

	std::vector<int> op_stack;
	std::vector<float> IPN;
	std::vector<std::string> variables;
	std::map<std::string, int> variable_ids;

	ExprParser()
	{
		expression = new char[2 << 13];
		e = new char[2 << 13];
	}

	void getToken();

	void clear();

	int AddVariable();

	float token2float();

	void Parse(std::string expr);

	int getOperatorPrecedenceLevel(int);

	int token2operation();

	bool tokenIsRBRACK();

	void load_expr(std::string expr);

	float Evaluate(std::map<std::string, float> variable_map);
};