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
	
	// Ha van olyan cellahalmaz, amelyben ugyanannyi lehetséges elem van, mint amennyi a cellák száma
	// és ezek egy sorban, oszlopban vagy négyzetben helyezkednek el, akkor az adott sorban, oszlopban
	// vagy négyzetben ezek a számok máshol már nem fordulhatnak elő.
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (possible[i][j].size()>1)
			{
				string key = "";
				for (auto const& n : possible[i][j])
				{
					key += n;
					key += ',';
				}
				vector<int> similarData = similarPossibles[key];
				similarData.push_back(i * N + j);
				similarPossibles[key] = similarData;
			}
		}
	}
	for (auto& iter : similarPossibles)
	{
		if (iter.first == "" || similarPossibles[iter.first].size() == 1)
		{
			continue;
		}
		vector<int> similarData = similarPossibles[iter.first];
		size_t n = std::count(iter.first.begin(), iter.first.end(), ',');
		if (similarData.size() == n)
		{
			int firstElement = similarData.at(0);
			int rowNum = firstElement / N;
			bool isSameRow = true;
			int colNum = firstElement % N;
			bool isSameCol = true;
			int max = sqrt(N);
			bool isSameTable = true;
			int cellBaseX = max * (int)(rowNum / max);
			int cellBaseY = max * (int)(colNum / max);
			int cellMaxX = cellBaseX +(max - 1);
			int cellMaxY = cellBaseY +(max - 1);

			for (int i = 1; i < n; i++)
			{
				int currentElement = similarData.at(i);
				int currentrowNum = currentElement / N;
				int currentcolNum = currentElement % N;
				if (isSameRow && rowNum != currentrowNum)
				{
					isSameRow = false;
				}
				if (isSameCol && colNum != currentcolNum)
				{
					isSameCol = false;
				}
				if (isSameTable && (currentrowNum<cellBaseX || currentrowNum > cellMaxX || currentcolNum < cellBaseY || currentcolNum > cellMaxX) )
				{
					isSameTable = false;
				}
			}
			if (isSameCol || isSameRow || isSameTable)
			{
				vector<char> occupancyValues;				
				for (auto const c : iter.first)
				{
					if (c != ',')
					{
						occupancyValues.push_back(c);
					}
				}
				if (isSameRow)
				{
					for (size_t i = 0; i < N; i++)
					{
						removeAll(possible[rowNum][i], occupancyValues);
					}					
				}
				if (isSameCol)
				{
					for (size_t i = 0; i < N; i++)
					{
						removeAll(possible[i][colNum], occupancyValues);
					}
				}
				if (isSameTable)
				{
					for (int y = cellBaseY; y < cellBaseY + max; ++y)
					{
						for (int x = cellBaseX; x < cellBaseX + max; ++x)
						{
							removeAll(possible[x][y], occupancyValues);
						}
					}
				}
				for (int i = 0; i < n; i++)
				{
					int currentElement = similarData.at(i);
					int currentrowNum = currentElement / N;
					int currentcolNum = currentElement % N;
					for (auto const c : occupancyValues)
					{
						possible[currentrowNum][currentcolNum].push_back(c);
					}
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
			//possible[y][x] = init->possible[y][x];
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

}
