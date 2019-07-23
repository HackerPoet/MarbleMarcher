#include<ExprParser.h>

bool replace(std::string& str, const std::string& from, const std::string& to) {
	
	int pos = str.find(from,0);
	if (pos == std::string::npos)
		return false;
	while (pos != std::string::npos)
	{
		str.replace(pos, from.length(), to);
		pos = str.find(from,pos+1);
	}
	return true;
}


/*
* checks if the given char c is a minus
*/
bool isMinus(const char c)
{
	if (c == 0) return 0;
	return c == '-';
}

// Get variable type and number
int ExprParser::AddVariable()
{
	std::string tokenn(token);
	
	if(variable_ids.count(tokenn) == 0)
	{
		variables.push_back(tokenn);
		variable_ids[tokenn] = variables.size();
	}
	
	return variable_ids[tokenn];
}

/*
* checks if the given char c is whitespace
* whitespace when space chr(32) or tab chr(9)
*/
bool isWhiteSpace(const char c)
{
	if (c == 0) return 0;
	return c == 32 || c == 9;  // space or tab
}

/*
* checks if the given char c is a delimiter
* minus is checked apart, can be unary minus
*/
bool isdelimiter(const char c)
{
	if (c == 0) return 0;
	return strchr("&|<>=+/*%^!", c) != 0;
}

/*
* checks if the given char c is NO delimiter
*/
bool isNotdelimiter(const char c)
{
	if (c == 0) return 0;
	return strchr("&|<>=+-/*%^!()", c) != 0;
}

/*
* checks if the given char c is a letter or undersquare
*/
bool isAlpha(const char c)
{
	if (c == 0) return 0;
	return strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ_", toupper(c)) != 0;
}

/*
* checks if the given char c is a digit or dot
*/
bool isDigitDot(const char c)
{
	if (c == 0) return 0;
	return strchr("0123456789.", c) != 0;
}

/*
* checks if the given char c is a digit
*/
bool isDigit(const char c)
{
	if (c == 0) return 0;
	return strchr("0123456789", c) != 0;
}


/**
* Get next token in the current string expr.
* Uses the Parser data expr, e, token, t, token_type and err
*/
void ExprParser::getToken()
{
	token_type = NOTHING;
	char* t;           // points to a character in token
	t = token;         // let t point to the first character in token
	*t = '\0';         // set token empty



					   // skip over whitespaces
	while (*e == ' ' || *e == '\t')     // space or tab
	{
		e++;
	}

	// check for end of expression
	if (*e == '\0')
	{
		// token is still empty
		token_type = delimiter;
		return;
	}

	// check for minus
	if (*e == '-')
	{
		token_type = delimiter;
		*t = *e;
		e++;
		t++;
		*t = '\0';  // add a null character at the end of token
		return;
	}

	// check for parentheses
	if (*e == '(' || *e == ')')
	{
		token_type = delimiter;
		*t = *e;
		e++;
		t++;
		*t = '\0';
		return;
	}

	// check for operators (delimiters)
	if (isdelimiter(*e))
	{
		token_type = delimiter;
		while (isdelimiter(*e))
		{
			*t = *e;
			e++;
			t++;
		}
		*t = '\0';  // add a null character at the end of token
		return;
	}

	// check for a value
	if (isDigitDot(*e))
	{
		token_type = NUMBER;
		while (isDigitDot(*e))
		{
			*t = *e;
			e++;
			t++;
		}

		// check for scientific notation like "2.3e-4" or "1.23e50"
		if (toupper(*e) == 'E')
		{
			*t = *e;
			e++;
			t++;

			if (*e == '+' || *e == '-')
			{
				*t = *e;
				e++;
				t++;
			}

			while (isDigit(*e))
			{
				*t = *e;
				e++;
				t++;
			}
		}

		*t = '\0';
		return;
	}

	// check for variables or functions
	if (isAlpha(*e))
	{
		while (isAlpha(*e) || isDigit(*e))
			//while (isNotdelimiter(*e))
		{
			*t = *e;
			e++;
			t++;
		}
		*t = '\0';  // add a null character at the end of token

					// check if this is a variable or a function.
					// a function has a parentesis '(' open after the name
		char* e2 = NULL;
		e2 = e;

		// skip whitespaces
		while (*e2 == ' ' || *e2 == '\t')     // space or tab
		{
			e2++;
		}

		if (*e2 == '(')
		{
			token_type = FUNCTION;
		}
		else
		{
			token_type = VARIABLE;
		}
		return;
	}

	// something unknown is found, wrong characters -> a syntax error
	token_type = UNKNOWN;
	while (*e != '\0')
	{
		*t = *e;
		e++;
		t++;
	}
	*t = '\0';

	return;
}

//convert number token to float
float ExprParser::token2float()
{
	std::string numb(token);

	std::istringstream io(numb);
	float number;
	io >> number;
	return number;
}

int ExprParser::getOperatorPrecedenceLevel(int op)
{
	if (op == PLUS || op == MINUS)
		return 1;
	else if (op == MULTIPLY || op == DIVIDE || op == INV)
		return 2;
	else if (op == POW)
		return 3;
	else if (op == FACTORIAL)
		return 4;
	else if (op == ABS || EXP == op || SIGN == op || SQRT == op 
		    || LOG == op || SIN == op || COS == op || TANH == op || THETA == op)
		return 5;
	else if (op == LBRACK)
		return 6;
}

//get operator id from the token
int ExprParser::token2operation()
{
	std::string fnc(token);
	std::transform(fnc.begin(), fnc.end(), fnc.begin(), ::toupper);
	if (fnc == "+")
		return PLUS;
	else if (fnc == "-")
		return MINUS;
	else if (fnc == "*")
		return MULTIPLY;
	else if (fnc == "/")
		return DIVIDE;
	else if (fnc == "^")
		return POW;
	else if (fnc == "!")
		return FACTORIAL;
	else if (fnc == "ABS")
		return ABS;
	else if (fnc == "EXP")
		return EXP;
	else if (fnc == "SIGN")
		return SIGN;
	else if (fnc == "SQRT")
		return SQRT;
	else if (fnc == "LOG")
		return LOG;
	else if (fnc == "SIN")
		return SIN;
	else if (fnc == "COS")
		return COS;
	else if (fnc == "TANH")
		return TANH;
	else if (fnc == "THETA")
		return THETA;
	else if (fnc == "(")
		return LBRACK;

	return PLUS;
}

bool ExprParser::tokenIsRBRACK()
{
	std::string fnc(token);
	if (fnc == ")")
		return 1;
	else
		return 0;
}


void ExprParser::load_expr(std::string expr)
{
	replace(expr, "**", "^");
	expr.copy(expression, expr.length());
	expression[expr.length()] = '\0';
	expr.copy(e, expr.length());
	e[expr.length()] = '\0';
}

void ExprParser::Parse(std::string expr)
{
	load_expr(expr);
	int vari = 0, cvari = 0;
	token_type = NUMBER;
	int prev_token = delimiter;
	//shunting yard algorithm
	while (*e != '\0')
	{
		getToken();

		if (token_type == NUMBER)
		{
			std::vector<int> num;
			cvari++;
			vari++;
			num.push_back(0);
			num.push_back(cvari - 1);

			IPN.push_back(-1);

			IPN.push_back(token2float());
		}

		if (token_type == VARIABLE)
		{
			int AA = AddVariable();

			vari++;

			IPN.push_back(-AA-1);
			IPN.push_back(0);
		}

		if ((token_type == FUNCTION || token_type == delimiter) && !tokenIsRBRACK())
		{
			int operation = token2operation();

			while (op_stack.size()>0 && getOperatorPrecedenceLevel(op_stack[op_stack.size() - 1]) >
				getOperatorPrecedenceLevel(operation) && (op_stack[op_stack.size() - 1] != LBRACK))
			{
				int pop_oper = op_stack[op_stack.size() - 1];
				op_stack.pop_back();
				IPN.push_back(pop_oper);
			}

			//if token is a minus sign
			if (operation == MINUS && (prev_token == delimiter || prev_token == FUNCTION))
			{
				operation = INV;
			}

			op_stack.push_back(operation);
		}

		if (tokenIsRBRACK())
		{
			while (op_stack.size()>0 && op_stack[op_stack.size() - 1] != LBRACK)
			{
				int pop_oper = op_stack[op_stack.size() - 1];
				op_stack.pop_back();
				IPN.push_back(pop_oper);
			}
			op_stack.pop_back();
		}

		prev_token = token_type;
	}
	while (op_stack.size()>0)
	{
		int pop_oper = op_stack[op_stack.size() - 1];
		op_stack.pop_back();
		IPN.push_back(pop_oper);
	}
}


float ExprParser::Evaluate(std::map<std::string, float> variable_map)
{	
	variable_map["pi"] = 3.141592653589f;
	variable_map["e"] == 2.718281828459f;	

	//code written in such a way to be compatible with OpenCL
	float varstack[64];
	int stack_size = 0;
	int N = IPN.size();

	float result = 0;

	for (int i = 0; i < N; i++)
	{
		//if operation
		if (IPN[i] >= 0)
		{
			if (stack_size>0)
			{
				float a = 0;
				if (stack_size>1)
				{
					a = varstack[stack_size - 2];
				}
				float b = varstack[stack_size - 1];
				switch ((int)(IPN[i]))
				{
					//plus
				case 0:
					stack_size--;
					varstack[stack_size - 1] = a + b;
					break;
					//minus
				case 1:
					stack_size--;
					varstack[stack_size - 1] = a - b;
					break;
					//minus sign
				case 2:
					varstack[stack_size - 1] = -b;
					break;
					//multiplication
				case 3:
					stack_size--;
					varstack[stack_size - 1] = a * b;
					break;
				case 4:
					stack_size--;
					varstack[stack_size - 1] = a / b;
					break;
				case 5:
					stack_size--;
					varstack[stack_size - 1] = pow(a, b);
					break;
				case 6:
					varstack[stack_size - 1] = b/*factorial*/;
					break;
				case 7:
					varstack[stack_size - 1] = abs(b);
					break;
				case 8:
					varstack[stack_size - 1] = exp(b);
					break;
				case 9:
					varstack[stack_size - 1] = (b >= 0) ? (1) : (-1);
					break;
				case 10:
					varstack[stack_size - 1] = sqrt(b);
					break;
				case 11:
					varstack[stack_size - 1] = log(b);
					break;
				case 12:
					varstack[stack_size - 1] = sin(b);
					break;
				case 13:
					varstack[stack_size - 1] = cos(b);
					break;
				case 14:
					varstack[stack_size - 1] = tanh(b);
					break;
				case 15:
					varstack[stack_size - 1] = (b > 0) ? b : 0;
					break;
				default:
					break;
				}
			}
		}
		//if variable
		else
		{
			int vartype = -IPN[i]-1;
			i++;
			stack_size++;

			if (vartype == 0)
			{
				varstack[stack_size - 1] = IPN[i];
			}
			else
			{
				std::string varname = variables[vartype-1];
				varstack[stack_size - 1] = variable_map[varname];
			}
		}
	}

	result = varstack[0];

	return result;
}

