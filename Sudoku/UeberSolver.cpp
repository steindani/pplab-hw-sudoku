#include "UeberSolver.h"

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
	const vector<char> base;
	for (int i = 1; i <= N; ++i)
		base.push_back(i);

	for	(int i = 0; i < N; ++i)
	{
		for	(int j = 0; j < N; ++j)
		{
			const char value = init[i * N + j];
			if (value != '0')
			{
				// if the value is fixed, the only possible value is the given one
				possible[i][j].push_back(value);
			}
			else
			{
				// if it is not fixed, every value is possible
				possible[i][j](base);
			}
			
			data[i][j] = value;
		}
	}

	// remove for every row
	for	(int i = 0; i < N; ++i)
	{
		const vector<char> rowvalues;

		for	(int j = 0; j < N; ++j)
		{
			rowvalues.push_back(data[i][j]);
		}
		
		for	(int j = 0; j < N; ++j)
		{
			// remove false possible values
			if (data[i][j] == '0')
			{
				removeAll(possible[i][j], rowvalues);
				if (possible[i][j].size() == 1)
				{
					data[i][j] = possible[i][j].front();
				}
			}
		}
	}

		// remove for every row
	for	(int j = 0; j < N; ++j)
	{
		const vector<char> colvalues;

		for	(int i = 0; i < N; ++i)
		{
			colvalues.push_back(data[i][j]);
		}
		
		for	(int i = 0; i < N; ++i)
		{
			// remove false possible values
			if (data[i][j] == '0')
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
			s << (char)(data[y][x]) << " ";
		}
		s << std::endl;
	}
}

template<int N>
bool UeberSolver<N>::isSolved()
{
	// Minden cella ki van t�ltve a t�bl�ban?
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
	bool allowed = true;

	// Azonos sorban vagy oszlopban csak egy 'val' lehet
	for (int i = 0; i < N; ++i)
	{
		if (data[y][i] == val) allowed = false;
		if (data[i][x] == val) allowed = false;
	}

	int max = sqrt(N)

	// Az adott 3x3-as cell�ban csak egy 'val' lehet
	int cellBaseX = max * (int)(x / max);
	int cellBaseY = max * (int)(y / max);
	for (int y = cellBaseY; y < cellBaseY + max; ++y)
	{
		for (int x = cellBaseX; x < cellBaseX + max; ++x)
		{
			if (data[y][x] == val) allowed = false;
		}
	}

	return allowed;
}

template<int N>
bool UeberSolver<N>::solveBackTrack()
{
	// K�szen vagyunk?
	if (isSolved())
	{
		return true;
	}

	// Keress�nk egy poz�ci�t, amely m�g nincs kit�ltve
	for (int y = 0; y < N; ++y)
	{
		for (int x = 0; x < N; ++x)
		{
			// Nincs m�g kit�ltve?
			if (data[y][x] == '0')
			{
				// Keress�nk egy �rt�ket, amely megfelel a szab�lyoknak
				for (int n = 1; n <= N; ++n)
				{
					// Be�rhat� az adott poz�ci�ba?
					if (isAllowed(n + '0', x, y))
					{
						// M�soljuk le a t�bl�t
						Solver tmpSolver(this);
						// �rjuk bele az �j �rt�ket
						tmpSolver.set(n + '0', x, y);
						// Pr�b�ljuk megoldani az �j t�bl�t
						if (tmpSolver.solveBackTrack())
						{
							// Megold�s
							*this = tmpSolver;
							return true;
						}
					}
				}
			}
			// Nem tudtunk �rt�ket �rni a cell�ba, �gy l�pj�nk vissza
			if (data[y][x] == '0') return false;
		}
	}

	return false;
}

template<int N>
void UeberSolver<N>::set(char val, int x, int y)
{
	data[y][x] = val;
}