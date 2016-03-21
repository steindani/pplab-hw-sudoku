#ifndef UEBER_SOLVER
#define UEBER_SOLVER

#include "Solver.h"
#include <vector>
#include <queue>
#include <map>

using namespace std;

template <int N>
class UeberSolver //: public Solver
{
private:
	static vector<char> possible[N][N];
	char data[N][N];
	map<string, vector<int> > similarPossibles;
	queue<int> nextToSolve;

public:
	UeberSolver(const char * init);
	UeberSolver(const UeberSolver* init);

	~UeberSolver() {};

	void print(std::ostream &s);

	bool isSolved();
	bool isAllowed(char val, int x, int y);
	bool solveBackTrack();

	void set(char val, int x, int y);
};

#endif /* UEBER_SOLVER */
