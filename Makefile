CC=g++

sudokuapp: Sudoku/Sudoku.cpp Sudoku/UeberSolver.h
	$(CC) -O3 -o sudokuapp -fopenmp -std=c++11 Sudoku/Sudoku.cpp Sudoku/UeberSolver.h

UeberSolver.o: Sudoku/UeberSolver.cpp Sudoku/UeberSolver.h
	$(CC) -c -fopenmp -std=c++11 Sudoku/UeberSolver.cpp

clean:
	rm *.o sudokuapp
