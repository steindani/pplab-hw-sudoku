#include "UeberSolver.h"
#include <algorithm>

// Remove all in c2 from c1
// source: http://stackoverflow.com/questions/28977799/c-equivalent-to-java-arraylist-method-removeall
template<typename Collection1, typename Collection2>
void removeAll(Collection1& c1, const Collection2& c2)
{
    for(auto&& i: c2)
		
        c1.erase(std::remove_if(c1.begin(), c1.end(), [i](decltype(i) i1){return i == i1;}), c1.end());
}

template <int N>
UeberSolver<N>::UeberSolver(const char * init)
{
	vector<char> base;
	for (int i = 1; i <= N; ++i)
		base.push_back(i);

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
			}
			
			data[i][j] = value;
		}
	}

	// remove for every row
	for	(int i = 0; i < N; ++i)
	{
		vector<char> rowvalues;

		for	(int j = 0; j < N; ++j)
		{
			rowvalues.push_back(data[i][j]);
		}
		
		for	(int j = 0; j < N; ++j)
		{
			// remove false possible values
			if (data[i][j] == 0)
			{
				removeAll(possible[i][j], rowvalues);
				if (possible[i][j].size() == 1)
				{
					data[i][j] = possible[i][j].front();
				}
			}
		}
	}

	// remove for every column
	for	(int j = 0; j < N; ++j)
	{
		vector<char> colvalues;

		for	(int i = 0; i < N; ++i)
		{
			colvalues.push_back(data[i][j]);
		}
		
		for	(int i = 0; i < N; ++i)
		{
			// remove false possible values
			if (data[i][j] == 0)
			{
				removeAll(possible[i][j], colvalues);
				if (possible[i][j].size() == 1)
				{
					data[i][j] = possible[i][j].front();
				}
			}
		}
	}
}

template<int N>
UeberSolver<N>::UeberSolver(const UeberSolver<N> * init)
{
	for (int y = 0; y < N; ++y)
	{
		for (int x = 0; x < N; ++x)
		{
			data[y][x] = init->data[y][x];
			possible[y][x] = init->possible[y][x];
		}
	}
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
	for (int y = 0; y < 9; ++y)
	{
		for (int x = 0; x < 9; ++x)
		{
			if (data[y][x] == 0) return false;
		}
	}
	return true;
}

template<int N>
bool UeberSolver<N>::isAllowed(char val, int x, int y)
{
	// Azonos sorban vagy oszlopban csak egy 'val' lehet
	for (int i = 0; i < N; ++i)
	{
		//Ha a sorban vagy oszlopban egy másik elem csak 'val' értéket vehetne fel, akkor ellentmondásra jutnánk.
		//if (i != x && possible[y][i].size() == 1 && std::find(possible[y][i].begin(), possible[y][i].end(), val) != possible[y][i].end()) allowed = false;
		//if (i != y && possible[i][x].size() == 1 && std::find(possible[i][x].begin(), possible[i][x].end(), val) != possible[i][x].end()) allowed = false;

		// Azonos sorban vagy oszlopban csak egy 'val' lehet
		//if (data[y][i] == val) allowed = false;
		//if (data[i][x] == val) allowed = false;

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
	for (int y = 0; y < N; ++y)
	{
		for (int x = 0; x < N; ++x)
		{
			// Nincs még kitöltve?
			if (data[y][x] == 0)
			{
				// Keressünk egy értéket, amely megfelel a szabályoknak és a leghetséges értéknek
				for (std::vector<char>::iterator n = possible[y][x].begin(); n != possible[y][x].end(); ++n)
				{
					// Beírható az adott pozícióba?
					if (isAllowed(*n, x, y))
					{
						// Másoljuk le a táblát
						UeberSolver<N> tmpSolver(this);
						// Írjuk bele az új értéket
						tmpSolver.set(*n, x, y);
						// Próbáljuk megoldani az új táblát
						if (tmpSolver.solveBackTrack())
						{
							// Megoldás
							*this = tmpSolver;
							return true;
						}
					}
				}
			}
			// Nem tudtunk értéket írni a cellába, így lépjünk vissza
			if (data[y][x] == 0) return false;
		}
	}

	return false;
}

template<int N>
void UeberSolver<N>::set(char val, int x, int y)
{
	//Elem beállítása
	data[y][x] = val;

	//Possible tábla frissítése
	for (int i = 0; i < N; i++)
	{
		possible[y][i].erase(std::remove(possible[y][i].begin(), possible[y][i].end(), val), possible[y][i].end());
		possible[i][x].erase(std::remove(possible[i][x].begin(), possible[i][x].end(), val), possible[i][x].end());
	}
	possible[y][x].push_back(val);
}
