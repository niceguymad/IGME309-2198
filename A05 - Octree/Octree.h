	#pragma once

#include <vector>
#include "MyEntityManager.h"
//#include "Simplex/Physics/Octant.h"

namespace Simplex
{
	class Octree
	{
		static uint m_uOctantCount;
		static uint m_uMaxLevel;
		static uint m_uIdealEntityCount;

		uint m_uID = 0;
		uint m_uLevel = 0;
		uint m_uChildren = 0;

		float m_fSize = 0.0f;

		MeshManager* m_pMeshMngr = nullptr;
		MyEntityManager* m_pEntitiyMngr = nullptr;

		vector3 m_vCenter = vector3(0);
		vector3 m_vMin = vector3(0);
		vector3 m_vMax = vector3(0);

		Octree* m_pParent = nullptr;
		Octree* m_pChild[8];

		std::vector<uint> m_EntityList;

		Octree* m_pRoot = nullptr;
		std::vector<Octree*> m_lChild;

	public:
		Model* model;

		Octree(uint octantLevels, uint maxSub); //construct octree
		
		Octree(vector3 a_v3Center, float a_fSize);

		Octree(Octree const& other);

		Octree& operator=(Octree const& other);

		~Octree(void);

		void Swap(Octree& other);

		float GetSize(void);

		vector3 GetCenterGlobal(void);

		vector3 GetMinGlobal(void);

		vector3 GetMaxGlobal(void);

		bool IsColliding(uint a_uRBIndex);

		void Display(uint octreeID, vector3 color = C_YELLOW); //display octree

		void Display(vector3 color = C_YELLOW);

		void DisplayLeafs(vector3 color = C_YELLOW);

		void ClearEntitiyList(void);

		void Subdivide(void);
		
		Octree* GetChild(uint a_nchild);

		Octree* GetParent(void);

		bool IsLeaf(void);

		bool ContainsMoreThan(uint a_nEntities);

		void KillBranches(void);

		void ConstructTree(uint maxLevel = 3);

		void AssignIDtoEntity(void);

		uint GetOctantCount(void);

	private:

		void Release(void);

		void Init(void);

		void ConstructList(void);
	};
}
