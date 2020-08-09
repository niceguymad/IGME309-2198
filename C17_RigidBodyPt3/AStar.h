#pragma once

#include "Definitions.h"
#include <vector>

class AStar
{
	public:
		AStar(Simplex::vector3 _start, Simplex::vector3 _end);
		~AStar();

		void GenerateHGrid();
		void GenerateGGrid();
		void GenerateFScore();
		
		void GeneratePath();
		void Progress(int pathPlace);

		void generateOpen();
		void generateClosed();
		std::vector<Simplex::vector3> fillPath();

		Simplex::vector3 getStart();
		void SetStart(Simplex::vector3 _start);
		Simplex::vector3 getEnd();
		void SetEnd(Simplex::vector3 _end);
					   
		Simplex::vector3 grid[20][20];
		bool closed[20][20];
					   
	private:		   
		Simplex::vector3 start;
		Simplex::vector3 end;
		
		bool pathFound;

		std::vector<Simplex::vector3> open;

		std::vector<Simplex::vector3> path;
	};


