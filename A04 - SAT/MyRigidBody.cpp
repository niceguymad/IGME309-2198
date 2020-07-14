/*
	Jared Baker, A04, 
*/
#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = true;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if(SAT(a_pOther) != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

// The checks should work everywhere, except near the top of steves head, when the creeper isn't rotated. I can't find the reason why it did this.
uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	//variables for the plane
	vector3 planeColor;
	vector3 planeAngle;

	eSATResults results = eSATResults::SAT_NONE;

	// center of the two models
	vector3 centers = a_pOther->GetCenterGlobal() -  this->GetCenterGlobal();

	//Half Measures for this model
	float lWidth = glm::abs(m_v3MinL.x + GetCenterLocal().x);
	float lHeight = glm::abs(m_v3MinL.y + GetCenterLocal().y);
	float lDepth = glm::abs(m_v3MinL.z + GetCenterLocal().z);

	//Axes for this model
	vector3 localX = this->GetModelMatrix()[0];
	vector3 localY = this->GetModelMatrix()[1];
	vector3 localZ = this->GetModelMatrix()[2];

	// half measures for the other model
	float oWidth = glm::abs(a_pOther->GetMinLocal().x + a_pOther->GetCenterLocal().x);
	float oHeight = glm::abs(a_pOther->GetMinLocal().y + a_pOther->GetCenterLocal().y);
	float oDepth = glm::abs(a_pOther->GetMinLocal().z + a_pOther->GetCenterLocal().z);

	// Axes for the other model
	vector3 otherX = a_pOther->GetModelMatrix()[0];
	vector3 otherY = a_pOther->GetModelMatrix()[1];
	vector3 otherZ = a_pOther->GetModelMatrix()[2];

	// These are the 15 checks to see if there is any axis of seperation between the two models
	if (glm::dot(centers, localX) > lWidth + glm::abs(oWidth * glm::dot(localX, otherX)) + glm::abs(oHeight* glm::dot(otherY, localX)) + glm::abs(oDepth * glm::dot(otherZ, localX)))
	{
		results = eSATResults::SAT_AX;
		planeColor = C_RED;
		planeAngle = localY;
	}
	else if (glm::dot(centers, localY) > lHeight + glm::abs(oWidth * glm::dot(localY, otherX)) + glm::abs(oHeight * glm::dot(otherY, localY)) + glm::abs(oDepth * glm::dot(otherZ, localY)))
	{
		results = results = eSATResults::SAT_AY;
		planeColor = C_GREEN;
		planeAngle = localX;
	}
	else if (glm::dot(centers, localZ) > lDepth + glm::abs(oWidth * glm::dot(localZ, otherX)) + glm::abs(oHeight * glm::dot(otherY, localZ)) + glm::abs(oDepth * glm::dot(otherZ, localZ)))
	{
		results = eSATResults::SAT_AZ;
		planeColor = C_BLUE;
		planeAngle = localZ;
	}
	else if (glm::dot(centers, otherX) > oWidth + glm::abs(lWidth * glm::dot(localX, otherX)) + glm::abs(lHeight * glm::dot(otherX, localY)) + glm::abs(lDepth * glm::dot(otherX, localZ)))
	{
		results = eSATResults::SAT_BX;
		planeColor = C_RED;
		planeAngle = otherY;
	}
	else if (glm::dot(centers, otherY) > oHeight + glm::abs(lWidth * glm::dot(localX, otherY)) + glm::abs(lHeight * glm::dot(otherY, localY)) + glm::abs(lDepth * glm::dot(otherY, localZ)))
	{
		results = eSATResults::SAT_BY;
		planeColor = C_GREEN;
		planeAngle = otherX;
	}
	else if (glm::abs(glm::dot(centers, otherZ)) > oDepth + glm::abs(lWidth * glm::dot(localX, otherZ)) + glm::abs(lHeight * glm::dot(localY, otherZ)) + glm::abs(lDepth * glm::dot(localZ, otherZ)))
	{
		results = eSATResults::SAT_BZ;
		planeColor = C_BLUE;
		planeAngle = otherZ;
	}
	else if (glm::dot(centers, glm::cross(localX, otherX)) > DAC(lHeight, localY, localX, otherX) + DAC(lDepth, localZ, localX, otherX) + DAC(oHeight, otherY, localX, otherX) + DAC(oDepth, otherZ, localX, localX))
	{
		results = eSATResults::SAT_AXxBX;
		planeAngle = glm::cross(otherX, localX);
	}
	else if (glm::dot(centers, glm::cross(localX, otherY)) > DAC(lHeight, localY, localX, otherY) + DAC(lDepth, localZ, localX, otherY) + DAC(oWidth, otherX, localX, otherY) + DAC(oDepth, otherZ, localX, otherY))
	{
		results = eSATResults::SAT_AXxBY;
		planeAngle = glm::cross(localX, otherY);
	}
	else if (glm::dot(centers, glm::cross(localX, otherZ)) > DAC(lHeight, localY, localX, otherZ) + DAC(lDepth, localZ, localX, otherZ) + DAC(oWidth, otherX, localX, otherZ) + DAC(oHeight, otherY, localX, otherZ))
	{
		results = eSATResults::SAT_AXxBZ;
		planeAngle = glm::cross(localX, otherZ);
	}
	else if (glm::dot(centers, glm::cross(localY, otherX)) > DAC(lWidth, localX, localY, otherX) + DAC(lDepth, localZ, localY, otherX) + DAC(oHeight, otherY, localY, otherX) + DAC(oDepth, otherZ, localY, otherX))
	{
		results = eSATResults::SAT_AYxBX;
		planeAngle = glm::cross(localY, otherX);
	}
	else if (glm::dot(centers, glm::cross(localY, otherY)) > DAC(lWidth, localX, localY, otherY) + DAC(lDepth, localZ, localY, otherY) + DAC(oWidth, otherX, localY, otherY) + DAC(oDepth, otherZ, localY, otherY))
	{
		results = eSATResults::SAT_AYxBY;
		planeAngle = glm::cross(localY, otherY);
	}
	else if (glm::dot(centers, glm::cross(localY, otherZ)) > DAC(lWidth, localX, localY, otherZ) + DAC(lDepth, localZ, localY, otherZ) + DAC(oWidth, otherX, localY, otherZ) + DAC(oHeight, otherY, localY, otherZ))
	{
		results = eSATResults::SAT_AYxBZ;
		planeAngle = glm::cross(localY, otherZ);
	}
	else if (glm::dot(centers, glm::cross(localZ, otherX)) > DAC(lWidth, localX, localZ, otherX) + DAC(lHeight, localY, localZ, otherX) + DAC(oHeight, otherY, localZ, otherX) + DAC(oDepth, otherZ, localZ, otherX))
	{
		results = eSATResults::SAT_AZxBX;
		planeAngle = glm::cross(localZ, otherX);
	}
	else if (glm::dot(centers, glm::cross(localZ, otherY)) > DAC(lWidth, localX, localZ, otherY) + DAC(lHeight, localY, localZ, otherY) + DAC(oWidth, otherX, localZ, otherY) + DAC(oDepth, otherZ, localZ, otherY))
	{
		results = eSATResults::SAT_AZxBY;
		planeAngle = glm::cross(localZ, otherY);
	}
	else if (glm::dot(centers, glm::cross(localZ, otherZ)) > DAC(lWidth, localX, localZ, otherZ) + DAC(lHeight, localY, localZ, otherZ) + DAC(oWidth, otherX, localZ, otherZ) + DAC(oHeight, otherY, localZ, otherZ))
	{
		results = eSATResults::SAT_AZxBZ;
		planeAngle = glm::cross(localZ, otherZ);
	}
	else
	{
		//there is no axis test that separates this two objects
		results = eSATResults::SAT_NONE;
	}

	if (results != eSATResults::SAT_NONE)
	{
		// creates two planes so you can see it from behind to represent the axis of seperation,  The angles are correct for the first 6 tests, then it becomes perpendicular to the correct oreientation
		m_pMeshMngr->AddPlaneToRenderList(glm::rotate(m_m4ToWorld, glm::radians(90.0f), planeAngle) * glm::scale(IDENTITY_M4, vector3(3, 3, 3)), planeColor);
		m_pMeshMngr->AddPlaneToRenderList(glm::rotate(m_m4ToWorld, glm::radians(270.0f), planeAngle) * glm::scale(IDENTITY_M4, vector3(3, 3, 3)), planeColor);
	}

	return results;
}

// returns the absolute value of a cross product in a dot product multipled by a value
float Simplex::MyRigidBody::DAC(float value, vector3 dot1, vector3 cross1, vector3 cross2)
{
	return glm::abs(value * glm::dot(dot1, glm::cross(cross1, cross2)));
}

