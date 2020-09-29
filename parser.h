
/*
 * parser.h
 *
 *  Created on: 15.09.2020
 *      Author: user
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <vector>
#include "token.h"
#include "node.h"

class Parser {
public:
	Parser() {
	}
	~Parser() {
	}
	void Parse(std::string in);
private:
	std::vector<Token> expr;
	Node *tree;
	int Tokenizer(std::string in);
	double Compute(Node *root, int *error);
};

// create vector of tokens from string
int Parser::Tokenizer(std::string in) {
	std::string::iterator it;
	std::string symbol;
	int error = 0;

	// parse input string to token vector
	for (it = in.begin(); it < in.end(); ++it) {
		std::string token_string;
		if (error) {
			break;
		} else {
			switch (*it) {
			case '(':
			case ')':
			case '+':
			case '-':
			case '*':
			case '/':
				// operators
				token_string = *it;
				expr.push_back(Token(LEX_OPERATOR, token_string));
				break;
			default:
				switch (*it) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': {
					// parsing a number
					bool integer_part = true;
					bool exponent_part = false;
					bool sign_of_exponent = false;
					bool is_done = false;

					TypeLexem type_of_number = LEX_INTEGER;
					while (it < in.end()) {
						switch (*it) {
						case '.':
							if (integer_part) {
								integer_part = false;
								exponent_part = false;
								type_of_number = LEX_DOUBLE;

								token_string += ".";
							} else {
								// we have another point in lexem
								std::cout << "tokenize error: redundant point" << std::endl;
								error = 1;
							}
							break;
						case 'e':
						case 'E':
							if (exponent_part) {
								std::cout << "tokenize error: redundant exponent 'e'" << std::endl;
								error = 1;
							} else {
								integer_part = false;
								exponent_part = true;
								type_of_number = LEX_DOUBLE;
								token_string += "e";
							}
							break;
						case '+':
						case '-':
							if (exponent_part) {
								if (sign_of_exponent) {
									is_done = true;
								} else {
									sign_of_exponent = true;
									token_string += *it;
								}
							} else {
								is_done = true;
							}
							break;
						default:
							if (std::isdigit(*it)) {
								token_string += *it;
							} else {
								is_done = true;
							}
							break;
						}
						if (is_done || error) {
							break;
						} else {
							it++;
						}
					}

					expr.push_back(Token(type_of_number, token_string));
					if (expr.end()->data.outOfRange) {
						std::cout << "tokenize error: data is out of range" << std::endl;
						error = 1;
						break;
					}
					if (it != in.end()) {
						it--;
					}
				}
					break;
				default:
					if (!std::isspace(*it)) {
						std::cout << "tokenize error: inadmissible symbol '" << *it << "'" << std::endl;
						error = 1;
					}
					break;
				}
				break;
			}
		}
	}

	if (error) {
		// in case off any errors clean token vector
		expr.erase(expr.begin(), expr.end());
	}
	return error;
}

void Parser::Parse(std::string in) {
	if (!Tokenizer(in)) {
		tree = CreateTree(expr.begin(), expr.end(), 0, false);
		if (tree){
			std::cout << "produce calculation: " << std::endl;
			int error = 0;			
			double result = Compute(tree, &error);
			if (!error) {
				std::cout << result << std::endl;
			}
			EraseTree(tree);
		} else {
			std::cout << "parse error" << std::endl;
		}
	}
}

// traverse tree from left to right and calculate
double Parser::Compute(Node *root, int *error) {
	double result = 0.0;
	if (root) {
		if ((root->item.type == LEX_INTEGER) || (root->item.type == LEX_DOUBLE)) {
			if (root->left || root->right) {
				std::cout << "syntax error: missing an operation!"
						<< std::endl;
				*error = 1;
			} else {
				if (root->item.type == LEX_INTEGER) {
					result = static_cast<double> (root->item.data.ival);
				} else {
					result = root->item.data.dval;
				}
			}
		} else {
			switch (root->item.data.type_op) {
			case OP_PLUS:
				if (root->right) {
					if (root->left) {
						result = Compute(root->left, error) + Compute(
								root->right, error);
					} else {
						// unary plus
						result = Compute(root->right, error);
					}
				} else {
					std::cout << "syntax error: missing rvalue after '+'"
							<< std::endl;
					*error = 1;
				}
				break;
			case OP_MINUS:
				if (root->right) {
					if (root->left) {
						result = Compute(root->left, error) - Compute(
								root->right, error);
					} else {
						// unary minus
						result = -Compute(root->right, error);
					}
				} else {
					std::cout << "syntax error: missing rvalue after '-'"
							<< std::endl;
					*error = 1;
				}
				break;
			case OP_DIVISION:
				if (root->left && root->right) {
					result = Compute(root->left, error) / Compute(root->right,
							error);
				} else {
					if (root->left) {
						std::cout
							<< "syntax error: missing rvalue after '/'"
							<< std::endl;
					} else {
						std::cout
							<< "syntax error: missing lvalue antil '/'"
							<< std::endl;
					}
					*error = 1;
				}
				break;
			case OP_PRODUCT:
				if (root->left && root->right) {
					result = Compute(root->left, error) * Compute(root->right,
							error);
				} else {
					if (root->left) {
						std::cout
							<< "syntax error: missing rvalue after '*'"
							<< std::endl;
					} else {
						std::cout
							<< "syntax error: missing lvalue antil '*'"
							<< std::endl;
					}
					*error = 1;
				}
				break;
			default:
				break;
			}
		}
	}
	return result;
}
#endif /* PARSER_H_ */
