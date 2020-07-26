#include "Octree.h"
using namespace Simplex;
//Octree()

//Simplex
void Simplex::Octree::Init(void)
{
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_uID = m_uOctantCount;
	m_uLevel = 0;

	m_vCenter = vector3(0.0f);
	m_vMax = vector3(0.0f);
	m_vMin = vector3(0.0f);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntitiyMngr = MyEntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (uint i = 0; i < 8; i++) 
	{
		m_pChild[i] = nullptr;
	}
}

Simplex::Octree::Octree(uint octantLevels, uint maxSub)
{
}

Simplex::Octree::Octree(vector3 a_v3Center, float a_fSize)
{
}

Simplex::Octree::Octree(Octree const& other)
{
}

Octree& Simplex::Octree::operator=(Octree const& other)
{
	// TODO: insert return statement here
}

Simplex::Octree::~Octree(void)
{
}

void Simplex::Octree::Swap(Octree const& other)
{

}

float Simplex::Octree::GetSize(void)
{
	return 0.0f;
}

vector3 Simplex::Octree::GetCenterGlobal(void)
{
	return vector3();
}

vector3 Simplex::Octree::GetMinGlobal(void)
{
	return vector3();
}

vector3 Simplex::Octree::GetMaxGlobal(void)
{
	return vector3();
}

bool Simplex::Octree::IsColliding(uint a_uRBIndex)
{
	return false;
}

void Simplex::Octree::Display(uint octreeID, vector3 color)
{
}

void Simplex::Octree::Display(vector3 color)
{
}

void Simplex::Octree::DisplayLeafs(uint octreeID, vector3 color)
{
}

void Simplex::Octree::ClearEntitiyList(void)
{
}

void Simplex::Octree::Subdivide(void)
{
}

Octree* Simplex::Octree::GetChild(uint a_nchild)
{
	return nullptr;
}

Octree* Simplex::Octree::GetParent(void)
{
	return nullptr;
}

bool Simplex::Octree::IsLeaf(void)
{
	return false;
}

bool Simplex::Octree::ContainsMoreThan(uint a_nEntities)
{
	return false;
}

void Simplex::Octree::KillBranches(void)
{
}

void Simplex::Octree::ConstructTree(uint maxLevels)
{
}

void Simplex::Octree::AssignIDtoEntity(void)
{
}

uint Simplex::Octree::GetOctantCount(void)
{
	return uint();
}

void Simplex::Octree::Release(void)
{
}



void Simplex::Octree::ConstructList(void)
{
}
