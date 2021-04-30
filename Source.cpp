//Maze creation using the olc pixel game engine header file
//Credit for maze creation code and pixel game engine to oneLoneCoder 
// 
//Maze solver implemented using a similar method to the creation
//Saves elapsed time for each run of solve in the text file 'Results.txt'
//Solver implemented by Raitis

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <stack>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <fstream>

class Maze : public olc::PixelGameEngine
{
public:
	Maze()
	{
		sAppName = "Maze";
	}

private:
	int m_nScreenWidth;
	int	m_nScreenHeight;
	int m_nMazeWidth;
	int m_nMazeHeight;
	int m_nCellWidth;
	
	int* m_maze;
	int* m_search;

	int m_nStartCell;
	int m_nEndCell;


	enum
	{
		CELL_PATH_N = 0x01, // 0000 0001
		CELL_PATH_E = 0x02, // 0000 0010
		CELL_PATH_S = 0x04, // 0000 0100
		CELL_PATH_W = 0x08, // 0000 1000
		CELL_VISITED = 0x10,// 0001 0000
		CELL_SEARCHED = 0x20,// 0001 0000
		
	};

	int m_nVisitedCells;
	int m_nSearchedCells;
	std::ofstream myfile;

	std::stack<std::pair<int, int>> m_stack;

	std::stack<std::pair<int, int>> m_searchStack;

	std::vector<std::pair<int, int>> vVector;

	int m_nPathWidth;

	float fAccumilatedElapsedTime;

	bool goal;
	bool reset;

public:

	void MazeCreator(void);
	void MazeDrawer(void);
	void MazeSolver(void);
	void RandomSearch(void);
	void SearchAgain(void);

	bool OnUserCreate() override
	{
		srand(time(NULL));

		m_nStartCell = rand() % 39;
		m_nEndCell = rand() % 39; // 960 is the first need a number between 0 and m_nMazeWidth - 1

		m_nMazeWidth = 40;
		m_nMazeHeight = 25;

		m_nScreenHeight = m_nMazeHeight + 2;
		m_nScreenWidth = m_nMazeWidth + 2;

		m_maze = new int[m_nMazeWidth * m_nMazeHeight];
		memset(m_maze, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));
		m_stack.push(std::make_pair(m_nStartCell, 0));
		m_maze[m_nStartCell] = CELL_VISITED;
		m_nVisitedCells = 1;


		m_search = new int[m_nMazeWidth * m_nMazeHeight];
		memset(m_search, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));
		m_searchStack.push(std::make_pair(m_nStartCell, 0));
		m_search[m_nStartCell] = CELL_SEARCHED;
		m_nSearchedCells = 1;

		m_nPathWidth = 3;
		m_nCellWidth = 4;

		m_nSearchedCells = 0;

		for (int x = 0; x < m_nScreenWidth; x++)
			for (int y = 0; y < m_nScreenHeight; y++)
				vVector.push_back(std::make_pair(x * (m_nPathWidth + 1) + 1, y * (m_nPathWidth + 1) + 1));
		
		fAccumilatedElapsedTime = 0;
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::VERY_DARK_CYAN);

		if (GetKey(olc::Key::SPACE).bPressed | reset)
		{
			SearchAgain();
		}
		
		if (m_nVisitedCells != 1000)
			MazeCreator();
		
		if (m_nVisitedCells != 1000)
			MazeDrawer();
		
		if (GetKey(olc::Key::ESCAPE).bPressed)
			return false;


		if (m_nVisitedCells == m_nMazeHeight * m_nMazeWidth && m_nSearchedCells < m_nMazeHeight * m_nMazeWidth - 1 && !goal)
			MazeSolver();

		if (m_nVisitedCells == m_nMazeHeight * m_nMazeWidth)
		{
			RandomSearch();
		}
		fAccumilatedElapsedTime += fElapsedTime;

		return true;
	}
};
void Maze::RandomSearch(void)
{
	for (int x = 1; x < m_nMazeWidth + 1; x++)
	{
		for (int y = 1; y < m_nMazeHeight + 1; y++)
		{
			//vVector.push_back(std::make_pair(x * (m_nPathWidth + 1) + 1, y * (m_nPathWidth + 1) + 1));
			for (int py = 0; py < m_nPathWidth; py++)
			{
				for (int px = 0; px < m_nPathWidth; px++)
				{
					if (m_search[(y - 1) * m_nMazeWidth + (x - 1)] & CELL_SEARCHED) {
						Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::MAGENTA);
					}
					else {
						Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::WHITE);
					}

					if (y == 1 && x - 1 == m_nStartCell)
					{
						Draw(x * (m_nPathWidth + 1) + px, (y - 1) * (m_nPathWidth + 1) + py, olc::MAGENTA);
					}
					else {}

					if (y == m_nMazeHeight && x - 1 == m_nEndCell)
					{
						Draw(x * (m_nPathWidth + 1) + px, (y + 1) * (m_nPathWidth + 1) + py, olc::MAGENTA);
					}
					else {}

				}


				for (int p = 0; p < m_nPathWidth; p++)
				{
					if (m_maze[(y - 1) * m_nMazeWidth + (x - 1)] & CELL_PATH_S)
					{
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth);
					}

					if (m_maze[(y - 1) * m_nMazeWidth + (x - 1)] & CELL_PATH_E)
					{
						Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p);
					}

					if (y == m_nMazeHeight && x - 1 == m_nEndCell)
					{
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth);
					}

					if (y == 1 && x - 1 == m_nStartCell)
					{
						Draw(x * (m_nPathWidth + 1) + p, (y - 1) * (m_nPathWidth + 1) + m_nPathWidth);
					}

					if (m_search[(y - 1) * m_nMazeWidth + (x - 1)] & CELL_PATH_S)
					{
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth, olc::MAGENTA);
					}

					if (m_search[(y - 1) * m_nMazeWidth + (x - 1)] & CELL_PATH_E)
					{
						Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p, olc::MAGENTA);
					}
				}
			}
		}
	}
}
void Maze::MazeSolver(void)
{
	auto offset = [&](int x, int y)
	{
		return (m_searchStack.top().second + y) * m_nMazeWidth + (m_searchStack.top().first + x);
	};

	if (m_nSearchedCells < m_nMazeWidth * m_nMazeHeight)
	{
		std::vector<int> neighbours2;

		//North Neighbor check
		if (m_searchStack.top().second > 0 && (m_search[offset(0, -1)] & CELL_SEARCHED) == 0 && (m_maze[offset(0, 0)] & CELL_PATH_N) == CELL_PATH_N) {
			neighbours2.push_back(0);
		}
		// East neighbour
		if (m_stack.top().first < m_nMazeWidth - 1 && (m_search[offset(1, 0)] & CELL_SEARCHED) == 0 && (m_maze[offset(0, 0)] & CELL_PATH_E) == CELL_PATH_E) {
			neighbours2.push_back(1);
		}
		// South neighbour
		if (m_stack.top().second < m_nMazeHeight - 1 && (m_search[offset(0, 1)] & CELL_SEARCHED) == 0 && (m_maze[offset(0, 0)] & CELL_PATH_S) == CELL_PATH_S) {
			neighbours2.push_back(2);
		}
		// West neighbour
		if (m_stack.top().first > 0 && (m_search[offset(-1, 0)] & CELL_SEARCHED) == 0 && (m_maze[offset(0, 0)] & CELL_PATH_W) == CELL_PATH_W) {
			neighbours2.push_back(3);
		}

		//Any path available?
		if (!neighbours2.empty())
		{
			//Choose one available neighbor at random
			int next_cell_dir = neighbours2[rand() % neighbours2.size()];

			//create a path between the neighbor and the current cell
			//Search an unsearched path
			switch (next_cell_dir)
			{
			case 0: // North
				m_search[offset(0, -1)] |= CELL_SEARCHED | CELL_PATH_S;
				m_search[offset(0, 0)] |= CELL_PATH_N;
				m_searchStack.push(std::make_pair((m_searchStack.top().first + 0), (m_searchStack.top().second - 1)));
				break;

			case 1: // East
				m_search[offset(+1, 0)] |= CELL_SEARCHED | CELL_PATH_W;
				m_search[offset(0, 0)] |= CELL_PATH_E;
				m_searchStack.push(std::make_pair((m_searchStack.top().first + 1), (m_searchStack.top().second + 0)));
				break;

			case 2: // South
				m_search[offset(0, +1)] |= CELL_SEARCHED | CELL_PATH_N;
				m_search[offset(0, 0)] |= CELL_PATH_S;
				m_searchStack.push(std::make_pair((m_searchStack.top().first + 0), (m_searchStack.top().second + 1)));
				break;

			case 3: // West
				m_search[offset(-1, 0)] |= CELL_SEARCHED | CELL_PATH_E;
				m_search[offset(0, 0)] |= CELL_PATH_W;
				m_searchStack.push(std::make_pair((m_searchStack.top().first - 1), (m_searchStack.top().second + 0)));
				break;

			}

			//new cell
			m_search[offset(0, 0)] |= CELL_SEARCHED;
			m_nSearchedCells++;
		}
		else
		{
			m_searchStack.pop(); //backtrack

		}
	}
	if (m_searchStack.top().first == m_nEndCell && m_searchStack.top().second == m_nMazeHeight - 1)
	{
		std::cout << fAccumilatedElapsedTime << std::endl;
		myfile.open("results.txt", std::ios::app);
		myfile << fAccumilatedElapsedTime << std::endl;
		myfile.close();
		goal = true;
		reset = true;
	}
}
void Maze::MazeDrawer(void)
{
	for (int x = 1; x < m_nMazeWidth + 1; x++)
	{
		for (int y = 1; y < m_nMazeHeight + 1; y++)
		{
			//vVector.push_back(std::make_pair(x * (m_nPathWidth + 1) + 1, y * (m_nPathWidth + 1) + 1));
			for (int py = 0; py < m_nPathWidth; py++)
			{
				for (int px = 0; px < m_nPathWidth; px++)
				{
					if (m_maze[(y - 1) * m_nMazeWidth + (x - 1)] & CELL_VISITED) {
						Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::WHITE);
					}
					else {
						Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::BLUE);
					}

					if (y == 1 && x-1 == m_nStartCell)
					{
						Draw( x * (m_nPathWidth + 1) + px, (y-1) * (m_nPathWidth + 1) + py, olc::WHITE);
					}
					else {}

					if (y == m_nMazeHeight && x-1 == m_nEndCell)
					{
						Draw(x * (m_nPathWidth + 1) + px, (y+1) * (m_nPathWidth + 1) + py, olc::WHITE);
					}
					else {}

				}


				for (int p = 0; p < m_nPathWidth; p++)
				{
					if (y == m_nMazeHeight && x - 1 == m_nEndCell)
					{
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth);
					}

					if (y == 1 && x - 1 == m_nStartCell)
					{
						Draw(x * (m_nPathWidth + 1) + p, (y-1) * (m_nPathWidth + 1) + m_nPathWidth);
					}

					if (m_maze[(y - 1) * m_nMazeWidth + (x - 1)] & CELL_PATH_S)
					{
						Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth);
					}

					if (m_maze[(y - 1) * m_nMazeWidth + (x - 1)] & CELL_PATH_E)
					{
						Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p);
					}
				}
			}
		}
	}
}
void Maze::MazeCreator(void)
{
	auto offset = [&](int x, int y)
	{
		return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
	};


	if (m_nVisitedCells < m_nMazeWidth * m_nMazeHeight)
	{
		std::vector<int> neighbours;

		//North Neighbor check
		if (m_stack.top().second > 0 && (m_maze[offset(0, -1)] & CELL_VISITED) == 0)
			neighbours.push_back(0);
		// East neighbour
		if (m_stack.top().first < m_nMazeWidth - 1 && (m_maze[offset(1, 0)] & CELL_VISITED) == 0)
			neighbours.push_back(1);
		// South neighbour
		if (m_stack.top().second < m_nMazeHeight - 1 && (m_maze[offset(0, 1)] & CELL_VISITED) == 0)
			neighbours.push_back(2);
		// West neighbour
		if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)] & CELL_VISITED) == 0)
			neighbours.push_back(3);


		//Any neighbours available?
		if (!neighbours.empty())
		{
			//Choose one available neighbor at random
			int next_cell_dir = neighbours[rand() % neighbours.size()];

			//create a path between the neighbor and the current cell
			switch (next_cell_dir)
			{
			case 0: // North
				m_maze[offset(0, -1)] |= CELL_VISITED | CELL_PATH_S;
				m_maze[offset(0, 0)] |= CELL_PATH_N;
				m_stack.push(std::make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
				break;

			case 1: // East
				m_maze[offset(+1, 0)] |= CELL_VISITED | CELL_PATH_W;
				m_maze[offset(0, 0)] |= CELL_PATH_E;
				m_stack.push(std::make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
				break;

			case 2: // South
				m_maze[offset(0, +1)] |= CELL_VISITED | CELL_PATH_N;
				m_maze[offset(0, 0)] |= CELL_PATH_S;
				m_stack.push(std::make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
				break;

			case 3: // West
				m_maze[offset(-1, 0)] |= CELL_VISITED | CELL_PATH_E;
				m_maze[offset(0, 0)] |= CELL_PATH_W;
				m_stack.push(std::make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
				break;

			}

			//new cell
			m_maze[offset(0, 0)] |= CELL_VISITED;
			m_nVisitedCells++;
		}
		else
		{
			m_stack.pop(); //backtrack
		}
	}
}
void Maze::SearchAgain(void)
{
	while (!m_searchStack.empty())
	{
		m_searchStack.pop();               //Removing top element every time
	}
	m_search = new int[m_nMazeWidth * m_nMazeHeight];
	memset(m_search, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));
	m_searchStack.push(std::make_pair(m_nStartCell, 0));
	m_search[m_nStartCell] = CELL_SEARCHED;
	m_nSearchedCells = 0;
	goal = false;
	reset = false;
	fAccumilatedElapsedTime = 0;
}

int main()
{
	Maze maze1;
	if (maze1.Construct(168, 108, 4, 4))
		maze1.Start();
	return 0;
}