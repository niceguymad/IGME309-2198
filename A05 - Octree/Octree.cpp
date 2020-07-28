/*
	Jared Baker, A06 Deliverable
*/

#include "Octree.h"
using namespace Simplex;
//Octree()
//Defualt Values
uint Octree::m_uOctantCount = 0;
uint Octree::m_uMaxLevel = 3;
uint Octree::m_uIdealEntityCount = 5;
//Initialize Octree Variables
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
// Constructor for the Octree class
Simplex::Octree::Octree(uint octantLevels, uint maxSub)
{
	Init();

	m_uOctantCount = 0;
	m_uMaxLevel = octantLevels;
	m_uIdealEntityCount = maxSub;
	m_uID = m_uOctantCount;
	//std::cout << octantLevels;
	m_pRoot = this;
	m_lChild.clear();

	std::vector<vector3> lMinMax;

	uint nObjects = m_pEntitiyMngr->GetEntityCount();
	for (uint i = 0; i < nObjects; i++) 
	{
		MyEntity* pEntity = m_pEntitiyMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		lMinMax.push_back(pRigidBody->GetMinGlobal());
		lMinMax.push_back(pRigidBody->GetMaxGlobal());
	}
	MyRigidBody* pRigidBody = new MyRigidBody(lMinMax);

	vector3 vHalfWidth = pRigidBody->GetHalfWidth();
	float fMax = vHalfWidth.x;
	for (int i = 1; i < 3; i++) 
	{
		if (fMax < vHalfWidth[i])
			fMax = vHalfWidth[i];
	}
	vector3 v3Center = pRigidBody->GetCenterLocal();
	lMinMax.clear();
	SafeDelete(pRigidBody);

	m_fSize = fMax * 2.0f;
	m_vCenter = v3Center;
	m_vMin = m_vCenter - (vector3(fMax));
	m_vMax = m_vCenter + (vector3(fMax));

	m_uOctantCount++;

	ConstructTree(octantLevels);
}

// Alternate Octree Octree
Simplex::Octree::Octree(vector3 a_v3Center, float a_fSize)
{
	Init();
	m_vCenter = a_v3Center;
	m_fSize = a_fSize;

	m_vMin = m_vCenter - (vector3(a_fSize) / 2.0f);
	m_vMax = m_vCenter + (vector3(a_fSize) / 2.0f);

	m_uOctantCount++;
}

// Copy Constructor
Simplex::Octree::Octree(Octree const& other)
{
	m_uChildren = other.m_uChildren;
	m_vCenter = other.m_vCenter;
	m_vMin = other.m_vMin;
	m_vMax = other.m_vMax;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;

	m_pRoot = other.m_pRoot;
	m_lChild = other.m_lChild;

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntitiyMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 0; i++) 
	{
		m_pChild[i] = other.m_pChild[i];
	}
}

// Operator Override
Octree& Simplex::Octree::operator=(Octree const& other)
{
	if (this != &other) 
	{
		Release();
		Init();
		Octree temp(other);
		Swap(temp);
	}
	return* this;
}

// Octree Destructor
Simplex::Octree::~Octree(void)
{
	Release();
}

// Octree Swap
void Simplex::Octree::Swap(Octree& other)
{
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_vCenter, other.m_vCenter);
	std::swap(m_vMin, other.m_vMin);
	std::swap(m_vMax, other.m_vMax);

	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);

	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntitiyMngr = MyEntityManager::GetInstance(); 

	for (uint i = 0; i < 0; i++)
	{
		std::swap( m_pChild[i], other.m_pChild[i]);
	}
}
// Returns Size
float Simplex::Octree::GetSize(void)
{
	return m_fSize;
}
// Returns Global Center
vector3 Simplex::Octree::GetCenterGlobal(void)
{
	return m_vCenter;
}
//Returns Global Min
vector3 Simplex::Octree::GetMinGlobal(void)
{
	return m_vMin;
}
//Returns Global Max
vector3 Simplex::Octree::GetMaxGlobal(void)
{
	return m_vMax;
}

//checks to see if a block is within an octant
bool Simplex::Octree::IsColliding(uint a_uRBIndex)
{
	int objectCount = m_pEntitiyMngr->GetEntityCount();

	if (a_uRBIndex >= objectCount)
		return false;

	MyEntity* pEntity = m_pEntitiyMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidbody = pEntity->GetRigidBody();
	vector3 minO = pRigidbody->GetMinGlobal();
	vector3 maxO = pRigidbody->GetMaxGlobal();

	if (m_vMax.x < minO.x)
		return false;
	if (m_vMin.x > maxO.x)
		return false;

	if (m_vMax.y < minO.y)
		return false;
	if (m_vMin.y > maxO.y)
		return false;

	if (m_vMax.z < minO.z)
		return false;
	if (m_vMin.z > maxO.z)
		return false;

	return true;
}
//Displays the specified octant
void Simplex::Octree::Display(uint octreeID, vector3 color)
{
	if (m_uID == octreeID) 
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_vCenter) * glm::scale(vector3(m_fSize)), color, RENDER_WIRE);
		return;
	}
	for (int index = 0; index < m_uChildren; index++) 
	{
		m_pChild[index]->Display(octreeID);
	}
}
//Displays the entire Octree
void Simplex::Octree::Display(vector3 color)
{
	for (int index = 0; index < m_uChildren; index++)
	{
		m_pChild[index]->Display(color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_vCenter) * glm::scale(vector3(m_fSize)), color, RENDER_WIRE);
}
//
void Simplex::Octree::DisplayLeafs(vector3 color)
{
	uint leafs = m_lChild.size();
	for (uint child = 0; child < leafs; child++)
	{
		m_lChild[child]->DisplayLeafs(color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_vCenter) * glm::scale(vector3(m_fSize)), color, RENDER_WIRE);
}
//Clears the Enitity List
void Simplex::Octree::ClearEntitiyList(void)
{
	for (uint child = 0; child < m_uChildren; child++)
	{
		m_lChild[child]->ClearEntitiyList();
	}
	m_EntityList.clear();
}
//Subdivides the octant that calls this function
void Simplex::Octree::Subdivide(void)
{
	if (m_uLevel >= m_uMaxLevel)
		return;

	if (m_uChildren != 0)
		return;

	m_uChildren = 8;

	float fSize = m_fSize / 4.0f;
	float fSizeD = fSize * 2.0f;
	vector3 v3Center;

	v3Center = m_vCenter;
	v3Center.x -= fSize;
	v3Center.y -= fSize;
	v3Center.z -= fSize;
	m_pChild[0] = new Octree(v3Center, fSizeD);

	v3Center.x += fSizeD; 
	m_pChild[1] = new Octree(v3Center, fSizeD);

	v3Center.z += fSizeD;
	m_pChild[2] = new Octree(v3Center, fSizeD);

	v3Center.x -= fSizeD;
	m_pChild[3] = new Octree(v3Center, fSizeD);

	v3Center.y += fSizeD;
	m_pChild[4] = new Octree(v3Center, fSizeD);

	v3Center.z -= fSizeD;
	m_pChild[5] = new Octree(v3Center, fSizeD);

	v3Center.x += fSizeD;
	m_pChild[6] = new Octree(v3Center, fSizeD);

	v3Center.z += fSizeD;
	m_pChild[7] = new Octree(v3Center, fSizeD);

	for (int index = 0; index < 8; index++) 
	{
		m_pChild[index]->m_pRoot = m_pRoot;
		m_pChild[index]->m_pParent = this;
		m_pChild[index]->m_uLevel = m_uLevel + 1;
		if (m_pChild[index]->ContainsMoreThan(m_uIdealEntityCount)) 
		{
			m_pChild[index]->Subdivide();
		}
	}
}
//returns the child of the octant
Octree* Simplex::Octree::GetChild(uint a_nchild)
{
	if(a_nchild > 7)return nullptr;
	return m_pChild[a_nchild];
}
//returns the parent of the octant
Octree* Simplex::Octree::GetParent(void)
{
	return m_pParent;
}
//If an octant has np children it is a leaf
bool Simplex::Octree::IsLeaf(void)
{
	return m_uChildren == 0;;
}
//checks whether the octant contains more entities than a specified amount
bool Simplex::Octree::ContainsMoreThan(uint a_nEntities)
{
	uint nCount = 0;
	uint objectCount = m_pEntitiyMngr->GetEntityCount();
	for (int n = 0; n < objectCount; n++)
	{
		if (IsColliding(n))
			nCount++;
		if (nCount > a_nEntities)
			return true;
	}
	return false;
}
// deletes the branches of the octree
void Simplex::Octree::KillBranches(void)
{
	for (uint index = 0; index < m_uChildren; index++) 
	{
		m_pChild[index]->KillBranches();
		delete m_pChild[index];
		m_pChild[index]= nullptr; 
	}
	m_uChildren = 0;
}
//constructs a new octree to the indicated level
void Simplex::Octree::ConstructTree(uint maxLevel)
{
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = maxLevel;

	m_uOctantCount = 1;

	m_EntityList.clear();

	KillBranches();
	m_lChild.clear();

	if (ContainsMoreThan(m_uIdealEntityCount)) 
	{
		Subdivide();
	}

	AssignIDtoEntity();

	ConstructList();
}
//Figures out what entity belongs to what octant
void Simplex::Octree::AssignIDtoEntity(void)
{
	for (uint child = 0; child < m_uChildren; child++)
	{
		m_pChild[child]->AssignIDtoEntity();
	}
	if (m_uChildren == 0) 
	{
		int entities = m_pEntitiyMngr->GetEntityCount();
		for (int index = 0; index < entities; index++) 
		{
			if (IsColliding(index)) 
			{
				m_EntityList.push_back(index);
				m_pEntitiyMngr->AddDimension(index, m_uID);
			}
		}
	}
}
//returns the octant count
uint Simplex::Octree::GetOctantCount(void)
{
	return m_uOctantCount;
}
// deletes the octree and its components
void Simplex::Octree::Release(void)
{
	if (m_uLevel == 0) 
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}

//constructs the root list of octants to be used with the page up and down buttons to show specified octants
void Simplex::Octree::ConstructList(void)
{
	for (int child = 0; child < m_uChildren; child++) 
	{
		m_pChild[child]->ConstructList();
	}

	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}
