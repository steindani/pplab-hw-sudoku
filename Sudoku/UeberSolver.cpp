#include "UeberSolver.h"
#include <algorithm>

using std::swap;

// Remove all in c2 from c1
// source: http://stackoverflow.com/questions/28977799/c-equivalent-to-java-arraylist-method-removeall
template<typename Collection1, typename Collection2>
void removeAll(Collection1& c1, const Collection2& c2)
{
    for(auto&& i: c2)
        c1.erase(std::remove_if(c1.begin(), c1.end(), [i](decltype(i) i1){return i == i1;}), c1.end());
}

template <int N>
vector<char> UeberSolver<N>::possible[N][N];


template <int N>
UeberSolver<N>::UeberSolver(const char * init)
{
	vector<char> base;
	for (int i = 1; i <= N; ++i)
		base.push_back(i);

  #pragma omp parallel for collapse(2) schedule(dynamic)
	for	(int i = 0; i < N; ++i)
	{
		for	(int j = 0; j < N; ++j)
		{
			const char value = init[i * N + j]-'0';
			if (value != '\0')
			{
				// if the value is fixed, the only possible value is the given one
				possible[i][j].push_back(value);
			}
			else
			{
				// if it is not fixed, every value is possible
				possible[i][j] = base;
        #pragma omp critical
        {
          nextToSolve.push(i * N + j);
        }
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
	// Azonos sorban vagy oszlopban csak egy 'val' lehet
	for (int i = 0; i < N; ++i)
	{
		//Ha a sorban vagy oszlopban egy másik elem csak 'val' értéket vehetne fel, akkor ellentmondásra jutnánk.
		//if (i != x && possible[y][i].size() == 1 && possible[y][i].at(0) == val) return false;
		//if (i != y && possible[i][x].size() == 1 && possible[i][x].at(0) == val) return false;

		// Azonos sorban vagy oszlopban csak egy 'val' lehet
		//if (data[y][i] == val) return false;
		//if (data[i][x] == val) return false;


		// szerintem elég ez:
		if (data[y][i] == val || data[i][x] == val) return false;
	}

	int max = sqrt(N);

	// Az adott 3x3-as cellában csak egy 'val' lehet
	int cellBaseX = max * (int)(x / max);
	int cellBaseY = max * (int)(y / max);
	for (int y = cellBaseY; y < cellBaseY + max; ++y)
	{
		for (int x = cellBaseX; x < cellBaseX + max; ++x)
		{
			if (data[y][x] == val) return false;
		}
	}

	return true;
}

template<int N>
bool UeberSolver<N>::solveBackTrack()
{
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

  // Nincs még kitöltve?
  if (data[y][x] == 0)
  {
  	// Keressünk egy értéket, amely megfelel a szabályoknak és a leghetséges értéknek
  	for (auto const& n : possible[y][x])
  	{
  		// Beírható az adott pozícióba?
  		if (isAllowed(n, x, y))
  		{
  			// Másoljuk le a táblát
  			UeberSolver<N> tmpSolver(this);
  			// Írjuk bele az új értéket
  			tmpSolver.set(n, x, y);
  			// Próbáljuk megoldani az új táblát
  			if (tmpSolver.solveBackTrack())
  			{
  				// Megoldás
  				*this = tmpSolver;
  				//return true;
  				std::cout << std::endl << "-----------------------------------------" << std::endl;
  				std::cout << "Solution:" << std::endl << std::endl;
  				tmpSolver.print(std::cout);
  				return false;
  			}
  		}
  	}
  }

	// Nem tudtunk értéket írni a cellába, így lépjünk vissza
	if (data[y][x] == 0) return false;
}

template<int N>
void UeberSolver<N>::set(char val, int x, int y)
{
	//Elem beállítása
	data[y][x] = val;

}
