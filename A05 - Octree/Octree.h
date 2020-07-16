#pragma once

#include <vector>
#include "MyEntityManager.h"
//#include "Simplex/Physics/Octant.h"

namespace Simplex
{
	class Octree
	{
	private:
		static uint octantCount;
		static uint maxLevel;
		static uint idealEntityCount;

		uint ID = 0;
		uint level = 0;
		uint children = 0;

		float size = 0.0f;

		MeshManager*   meshMngr = nullptr;
		MyEntityManager* entitiyMngr = nullptr;

		vector3 center = vector3(0);
		vector3 min = vector3(0);
		vector3 max = vector3(0);

		Octree* parent = nullptr;
		Octree* child[8];

		std::vector<uint> entityList;

		Octree* root = nullptr;
		std::vector<Octree*> nodes;

	public:
		Model* model;

		Octree(uint octantLevels, uint maxSub); //construct octree
		
		void Display(uint octreeID); //display octree
	};
}
