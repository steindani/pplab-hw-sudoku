CC=g++

all: Sudoku.o UeberSolver.o Solver.o
	$(CC) -O3 -o sudokuapp -fopenmp -std=c++11 Sudoku.o UeberSolver.o Solver.o

Sudoku.o: Sudoku/Sudoku.cpp UeberSolver.o Solver.o
	$(CC) -c -fopenmp -std=c++11 Sudoku/Sudoku.cpp UeberSolver.o

UeberSolver.o: Sudoku/UeberSolver.cpp Sudoku/UeberSolver.h Sudoku/Solver.h
	$(CC) -c -fopenmp -std=c++11 Sudoku/UeberSolver.cpp

Solver.o: Sudoku/Solver.cpp Sudoku/Solver.h
	$(CC) -c -fopenmp -std=c++11 Sudoku/Solver.cpp

clean:
	rm *.o sudokuapp
