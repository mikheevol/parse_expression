/*
 * token.h
 *
 *  Created on: 15.09.2020
 *      Author: Mikheev Oleg
 */

#ifndef TOKEN_H_
#define TOKEN_H_

#include <cerrno>
#include <cstdlib>

enum TypePriority {
	PR_NUMERIC_OR_EXPRESION, PR_PARENTES, PR_DIVISON_PRODUCT, PR_PLUS_MINUS
};

enum TypeLexem {
	LEX_INTEGER, LEX_DOUBLE, LEX_OPERATOR
};

enum TypeOperation {
	OP_LP, OP_RP, OP_PLUS, OP_MINUS, OP_DIVISION, OP_PRODUCT
};

// internal token representation
struct Data {
	TypeOperation type_op;
	TypePriority pr;
	std::string op;
	double dval;
	long int ival;
	bool outOfRange;

	Data(TypeLexem type, const std::string in) :
		dval(0.0), ival(0), outOfRange(false) {
		switch (type) {
		case LEX_INTEGER:
			errno = 0;
			this->ival = std::strtol(in.c_str(), NULL, 10);
			if (errno == ERANGE) {
				this->ival = 0;
				outOfRange = true;
			}
			pr = PR_NUMERIC_OR_EXPRESION;
			break;
		case LEX_DOUBLE:
			this->dval = std::atof(in.c_str());
			errno = 0;
			this->dval = std::strtod(in.c_str(), NULL);
			if (errno == ERANGE) {
				this->dval = 0.0;
				outOfRange = true;
			}
			pr = PR_NUMERIC_OR_EXPRESION;
			break;
		case LEX_OPERATOR:
			switch (*in.begin()) {
			case '(':
				type_op = OP_LP;
				pr = PR_PARENTES;
				break;
			case ')':
				type_op = OP_RP;
				pr = PR_PARENTES;
				break;
			case '+':
				type_op = OP_PLUS;
				pr = PR_PLUS_MINUS;
				break;
			case '-':
				type_op = OP_MINUS;
				pr = PR_PLUS_MINUS;
				break;
			case '*':
				type_op = OP_PRODUCT;
				pr = PR_DIVISON_PRODUCT;
				break;
			case '/':
				type_op = OP_DIVISION;
				pr = PR_DIVISON_PRODUCT;
				break;
			default:
				break;
			}
			this->op = in;
			break;
		default:
			break;
		}
	}
};

// token class
struct Token {
	TypeLexem type;
	Data data;
	std::string val;

	Token(TypeLexem _type, const std::string _in) :
		type(_type), data(_type, _in), val(_in) {
	}
	~Token() {
	}
};

#endif /* TOKEN_H_ */
