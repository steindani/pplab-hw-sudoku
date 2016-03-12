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

	bool hasTrivial = true;
	while (hasTrivial)
	{
		hasTrivial = false;
		// remove for every row
		for (int i = 0; i < N; ++i)
		{
			vector<char> rowvalues;

			for (int j = 0; j < N; ++j)
			{
				rowvalues.push_back(data[i][j]);
			}

			for (int j = 0; j < N; ++j)
			{
				// remove false possible values
				if (data[i][j] == 0)
				{
					removeAll(possible[i][j], rowvalues);
					if (possible[i][j].size() == 1)
					{
						hasTrivial = true;
						set(possible[i][j].front(), j, i);
						//data[i][j] = possible[i][j].front();
					}
				}
			}
		}

		// remove for every column
		for (int j = 0; j < N; ++j)
		{
			vector<char> colvalues;

			for (int i = 0; i < N; ++i)
			{
				colvalues.push_back(data[i][j]);
			}

			for (int i = 0; i < N; ++i)
			{
				// remove false possible values
				if (data[i][j] == 0)
				{
					removeAll(possible[i][j], colvalues);
					if (possible[i][j].size() == 1)
					{
						hasTrivial = true;
						//data[i][j] = possible[i][j].front();
						set(possible[i][j].front(), j, i);
					}
				}
			}
		}

		int max = sqrt(N);
		//remove for every sqrt(N) x sqrt(N) table
		for (int j = 0; j < N; ++j)
		{
			vector<char> tablevalues;

			for (int i = 0; i < N; ++i)
			{
				int cellBaseX = max * (int)(i / max);
				int cellBaseY = max * (int)(j / max);
				for (int x = cellBaseX; x < cellBaseX + max; ++x)
				{
					for (int y = cellBaseY; y < cellBaseY + max; ++y)
					{

						tablevalues.push_back(data[x][y]);
					}
				}
				removeAll(possible[i][j], tablevalues);
				if (possible[i][j].size() == 1)
				{
					hasTrivial = true;
					//data[i][j] = possible[i][j].front();
					set(possible[i][j].front(), j, i);
				}
				tablevalues.clear();
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
	for (int y = 0; y < N; ++y)
	{
		for (int x = 0; x < N; ++x)
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
	for (int y = 0; y < N; ++y)
	{
		for (int x = 0; x < N; ++x)
		{
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
	}
	return false;
}

template<int N>
void UeberSolver<N>::set(char val, int x, int y)
{
	//Elem beállítása
	data[y][x] = val;

	//Possible tábla frissítése (val kivétele a sorból és oszlopból)
	for (int i = 0; i < N; i++)
	{
		//Source: http://stackoverflow.com/questions/39912/how-do-i-remove-an-item-from-a-stl-vector-with-a-certain-value
		auto it1 = std::find(possible[y][i].begin(), possible[y][i].end(), val);

		if (it1 != possible[y][i].end()) {
			// swap the one to be removed with the last element
			// and remove the item at the end of the container
			// to prevent moving all items after 'val' by one
			swap(*it1, possible[y][i].back());
			possible[y][i].pop_back();
		}

		if (i != x && possible[y][i].size() == 1)
		{
			if (isAllowed(possible[y][i].at(0), i, y))
			{
				data[y][i] = possible[y][i].at(0);
			}
		}
		auto it2 = std::find(possible[i][x].begin(), possible[i][x].end(), val);

		if (it2 != possible[i][x].end()) {
			swap(*it2, possible[i][x].back());
			possible[i][x].pop_back();
		}

		if (i != y && possible[i][x].size() == 1)
		{
			if (isAllowed(possible[i][x].at(0), x, i))
			{
				data[i][x] = possible[i][x].at(0);
			}
		}
	}

	int max = sqrt(N);

	// Az adott 3x3-as cellában csak egy 'val' lehet
	int cellBaseX = max * (int)(x / max);
	int cellBaseY = max * (int)(y / max);
	for (int y = cellBaseY; y < cellBaseY + max; ++y)
	{
		for (int x = cellBaseX; x < cellBaseX + max; ++x)
		{
			if (data[y][x] == 0)
			{
				auto it1 = std::find(possible[y][x].begin(), possible[y][x].end(), val);

				if (it1 != possible[y][x].end()) {
					swap(*it1, possible[y][x].back());
					possible[y][x].pop_back();
				}
				if (possible[y][x].size() == 1)
				{
					if (isAllowed(possible[y][x].at(0), x, y))
					{
						data[y][x] = possible[y][x].at(0);
					}
				}
			}
		}
	}
	possible[y][x].clear();
	possible[y][x].push_back(val);
}
