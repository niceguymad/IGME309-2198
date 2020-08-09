/*
	Jared Baker
*/

#include "AStar.h"
//AStar constructor
AStar::AStar(Simplex::vector3 _start, Simplex::vector3 _end)
{
	start = _start;
	end = _end;

	GenerateGGrid();
	GenerateHGrid();

	pathFound = false;

	open = std::vector<Simplex::vector3>();
	path = std::vector<Simplex::vector3>();
}

//Destructor
AStar::~AStar()
{
}

//generates obstacles
void AStar::GenerateHGrid()
{
	float hScore = 0.0f;
	for (int r = 0; r < 20; r++)
	{
		for (int c = 0; c < 20; c++)
		{
			hScore = sqrtf(pow(r - end.x, 2) + pow(c - end.y, 2));
			if (r == start.x && c == start.y) 
			{
				hScore = 0;
			}

			grid[r][c].y = hScore;
		}
	}
}
//Generates estimate distance to end
void AStar::GenerateGGrid()
{
	for (int r = 0; r < 20; r++) 
	{
		for (int c = 0; c < 20; c++) 
		{
			if(r % 6 == 0 &&  c % 3 == 0)
				grid[r][c].x = 99;
			else
				grid[r][c].x = 1;
		}
	}
}
//Generates f scores
void AStar::GenerateFScore()
{
	for (int r = 0; r < 20; r++)
	{
		for (int c = 0; c < 20; c++)
		{
			grid[r][c].z = grid[r][c].x + grid[r][c].y;
		}
	}
}

// tries to generate the path to the end, but doesn't work
void AStar::GeneratePath()
{
	generateOpen();
	generateClosed();

	float fScore = 0.0f;
	int listPlace = 0;
	Simplex::vector3 succesor = Simplex::vector3(0, 0, 99);
	static Simplex::vector3 current = Simplex::vector3(0, 0, 99);
	path.push_back(start);

	while (open.empty() != true) 
	{
		for (int i = 0; i < open.size(); i++) 
		{
			if (open[i].z < current.z) 
			{
				current.z = open[i].z;
				listPlace = i;
			}
		}
		open.erase(open.begin() + listPlace);

		for (int n = 0; n < 4; n++)
		{
			Simplex::vector3 succesorTemp;
			if (n == 0)
				if(current.x - 1 > 0)
					succesorTemp = grid[(int)current.x - 1][(int)current.y];
			if (n == 1)
				if (current.x + 1 < 20)
				succesorTemp = grid[(int)current.x + 1][(int)current.y];
			if (n == 2)
				if (current.y - 1 > 0)
				succesorTemp = grid[(int)current.x][(int)current.y - 1];
			if (n == 3)
				if (current.y + 1 < 20)
				succesorTemp = grid[(int)current.x][(int)current.y + 1];

			if (succesorTemp.x == end.x && succesorTemp.y == end.y) 
			{
				pathFound = true;
				fillPath();
				return;
			}

			else if (closed[(int)succesorTemp.x][(int)succesorTemp.y] == false)
			{
				bool onOpen = false;
				for (int i = 0; i < open.size(); i++)
				{
					if (open[i] == succesor)
					{
						onOpen = true;
					}
				}

				if (onOpen == false) 
				{		
					if (succesorTemp.z <= succesor.z) 
					{
						succesor = succesorTemp;
						closed[(int)succesorTemp.x][(int)succesorTemp.y] = false;
					}				
				}

			}
			closed[(int)current.x][(int)current.y] = true;
		}
	}
}

//would progress the creeper incrementally towards the end
void AStar::Progress(int pathPlace)
{
	start = path[pathPlace];
}

// generates the open list
void AStar::generateOpen()
{
	open.clear();
	for (int r = 0; r < 20; r++)
	{
		for (int c = 0; c < 20; c++)
		{
			open.push_back(Simplex::vector3(r, c, grid[r][c].z));
		}
	}
}
//generates the closed list
void AStar::generateClosed()
{
	for (int r = 0; r < 20; r++)
	{
		for (int c = 0; c < 20; c++)
		{
			closed[r][c] = false;
		}
	}
}
//tries to assemble a list for the path from the start to the end
std::vector<Simplex::vector3> AStar::fillPath()
{
	for (int r = 0; r < 20; r++)
	{
		for (int c = 0; c < 20; c++)
		{
			if (closed[r][c] == false) 
			{
				path.push_back(grid[r][c]);
			}
		}
	}
	return path;
}

Simplex::vector3 AStar::getStart()
{
	return start;
}

void AStar::SetStart(Simplex::vector3 _start)
{
	start = _start;
}

Simplex::vector3 AStar::getEnd()
{
	return end;
}

void AStar::SetEnd(Simplex::vector3 _end)
{
	end = _end;
}

