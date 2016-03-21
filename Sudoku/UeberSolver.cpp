#include "UeberSolver.h"
#include <algorithm>

using std::swap;

// Remove all in c2 from c1
// source: http://stackoverflow.com/questions/28977799/c-equivalent-to-java-arraylist-method-removeall
template<typename Collection1, typename Collection2>
void removeAll(Collection1& c1, const Collection2& c2)
{
  for(auto&& i: c2)
  {
		c1.erase(std::remove_if(c1.begin(), c1.end(), [i](decltype(i) i1){return i == i1;}), c1.end());
  }
}

template<int N>
vector<char> UeberSolver<N>::possible[N][N];


template<int N>
UeberSolver<N>::UeberSolver(const char * init)
{
	// Create the base vector for possible values in case the given input is not fixed
	vector<char> base;
	for (int i = 1; i <= N; ++i)
	{
		base.push_back(i);
	}

	#pragma omp parallel for collapse(2) schedule(dynamic)
	for	(int i = 0; i < N; ++i)
	{
		for	(int j = 0; j < N; ++j)
		{
			const char value = init[i * N + j]-'0';
			if (value == '\0')
			{
				// if it is not fixed, every value is possible
				UeberSolver<N>::possible[i][j] = base;
				#pragma omp critical
				{
					nextToSolve.push(i * N + j);
				}
			}
			else
			{
				// if the value is fixed, the only possible value is the given one
				UeberSolver<N>::possible[i][j].push_back(value);
			}

			data[i][j] = value;
		}
	}
}

template<int N>
UeberSolver<N>::UeberSolver(const UeberSolver<N> * init)
{
	#pragma omp parallel for collapse(2) schedule(dynamic)
	for (int y = 0; y < N; ++y)
	{
		for (int x = 0; x < N; ++x)
		{
			data[y][x] = init->data[y][x];
		}
	}

	nextToSolve = init->nextToSolve;
}

template<int N>
void UeberSolver<N>::print(std::ostream & s)
{
	for (int y = 0; y < N; ++y)
	{
		for (int x = 0; x < N; ++x)
		{
			s << (int)(data[y][x]) << " ";
		}
		s << std::endl;
	}
}

template<int N>
bool UeberSolver<N>::isSolved()
{
	// Minden cella ki van töltve a táblában?
	return nextToSolve.empty();
}

template<int N>
bool UeberSolver<N>::isAllowed(char val, int x, int y)
{
	if (data[y][x] != '\0')
	{
		return false;
	}
	// Azonos sorban vagy oszlopban csak egy 'val' lehet
	bool abort = false;
	#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < N; ++i)
	{
		if (!abort && (data[y][i] == val || data[i][x] == val))
		{
			abort = true;
			#pragma omp flush(abort)
		}
	}
	if (abort) {
		return false;
	}

	const int max = sqrt(N);

	// Az adott 3x3-as cellában csak egy 'val' lehet
	int cellBaseX = max * (int)(x / max);
	int cellBaseY = max * (int)(y / max);
	#pragma omp parallel for collapse(2) schedule(dynamic)
	for (int y = cellBaseY; y < cellBaseY + max; ++y)
	{
		for (int x = cellBaseX; x < cellBaseX + max; ++x)
		{
			if (!abort && (data[y][x] == val))
			{
				abort = true;
				#pragma omp flush(abort)
			}
		}
	}
	if (abort) {
		return false;
	}

	return true;
}

template<int N>
bool UeberSolver<N>::solveBackTrack()
{
	cout << nextToSolve.size() << endl;
	// Készen vagyunk?
	if (isSolved())
	{
		return true;
	}

	// Keressünk egy pozíciót, amely még nincs kitöltve
	int x, y;
  do
  {
    int next = nextToSolve.front();
  	x = next % N;
  	y = next / N;
  	nextToSolve.pop();
  } while (data[y][x] != '\0' && !nextToSolve.empty());

  if (data[y][x] != '\0')
  {
    return true;
  }

	// Keressünk egy értéket, amely megfelel a szabályoknak és a leghetséges értéknek
//	for (auto const& n : UeberSolver<N>::possible[y][x])
	for (int n = 1; n <= N; ++n)
	{
		// Beírható az adott pozícióba?
		if (isAllowed(n, x, y))
		{
			// Másoljuk le a táblát
			// Írjuk bele az új értéket
			// Próbáljuk megoldani az új táblát
			UeberSolver<N> tmpSolver(this);
			tmpSolver.set(n, x, y);

			if (tmpSolver.solveBackTrack())
			{
				*this = tmpSolver;
				return true;

//				std::cout << std::endl << "-----------------------------------------" << std::endl;
//				std::cout << "Solution:" << std::endl << std::endl;
//				tmpSolver.print(std::cout);
//				return false;
			}
		}
	}

	return false;
}

template<int N>
void UeberSolver<N>::set(char val, int x, int y)
{
	//Elem beállítása
	data[y][x] = val;
}
