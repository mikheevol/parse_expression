#include <iostream>
#include <string>
#include "parser.h"

int main(int argc, char *argv[]) {
	std::cout << "Enter expression:" << std::endl;

	std::string expr;
	getline(std::cin, expr);

	Parser pars;
	pars.Parse(expr);

	return 0;
}
