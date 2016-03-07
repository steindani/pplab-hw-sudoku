// Sudoku.cpp : Defines the entry point for the console application.

#include <iostream>

//#include "Solver.h"
#include "UeberSolver.h"
#include "UeberSolver.cpp"

int main()
{
	// További megoldható 17 elemet tartalmazó táblák: http://http://staffhome.ecm.uwa.edu.au/~00013890/sudokumin.php
	UeberSolver<9> solver("000801000000000043500000000000070800020030000000000100600000075003400000000200600");
	std::cout << "Problem:" << std::endl << std::endl;
	solver.print(std::cout);
	std::cout << std::endl << "-----------------------------------------" << std::endl;
	std::cout << "Solution:" << std::endl << std::endl;;
	solver.solveBackTrack();
	solver.print(std::cout);
    return 0;
}

