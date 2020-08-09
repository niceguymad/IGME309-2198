#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(	vector3(0.0f, 3.0f, 13.0f), //Position
											vector3(0.0f, 3.0f, 12.0f),	//Target
											AXIS_Y );					//Up

	//Set the position of the light
	m_pLightMngr->SetPosition(vector3(10.0f));

	//Initialize models

	//creeper
	m_pCreeper = new Model();
	m_pCreeper->Load("Minecraft\\Creeper.obj");
	m_pCreeperRB = new MyRigidBody(m_pCreeper->GetVertexList());

	//steve
	m_pSteve = new Model();
	m_pSteve->Load("Minecraft\\Steve.obj");
	m_pSteveRB= new MyRigidBody(m_pSteve->GetVertexList());

	block = new Model();
	block->Load("MineCraft\\Cube.obj");
	blockRB = new MyRigidBody(block->GetVertexList());

	//Making the aStar
	aStar = new AStar(vector3(0, 0, 0), vector3(19, 19, 0));
	aStar->GeneratePath();
	path = aStar->fillPath();
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Set model matrix to the creeper
	matrix4 mCreeper = glm::translate(vector3(aStar->getStart().x, 0.0f, aStar->getStart().y));
	m_pCreeper->SetModelMatrix(mCreeper);
	m_pCreeperRB->SetModelMatrix(mCreeper);
	m_pMeshMngr->AddAxisToRenderList(mCreeper);
	
	//Set model matrix to Steve
	matrix4 mSteve = glm::translate(vector3(aStar->getEnd().x, 0.0f, aStar->getEnd().y));
	m_pSteve->SetModelMatrix(mSteve);
	m_pSteveRB->SetModelMatrix(mSteve);
	m_pMeshMngr->AddAxisToRenderList(mSteve);
	
	// draws all the blocks that aren't obstacles
	for (int i = 0; i < 20; i++) 
	{
		for (int j = 0; j < 20; j++)
		{
			if (aStar->grid[i][j].x != 99) 
			{
				matrix4 mBlock = glm::translate(vector3(i-.5f, -1, j-.5f)) * glm::scale(vector3(.99f,.99f,.99f));
				// my attempt to draw the path 
				if (aStar->closed[i][j] == false)
				{
					m_pMeshMngr->AddWireCubeToRenderList(mBlock * glm::translate(vector3(1, 1.0f, 1)), C_GREEN);
				}
				block->SetModelMatrix(mBlock);
				block->AddToRenderList();
			}

		}
	}
	
	bool bColliding = m_pCreeperRB->IsColliding(m_pSteveRB);
	
	m_pCreeper->AddToRenderList();
	m_pCreeperRB->AddToRenderList();

	m_pSteve->AddToRenderList();
	m_pSteveRB->AddToRenderList();

	m_pMeshMngr->Print("Colliding: ");
	if(bColliding)
		m_pMeshMngr->PrintLine("YES!", C_RED);
	else
		m_pMeshMngr->PrintLine("no", C_YELLOW);
}
void Application::Display(void)
{
	//Clear the screen
	ClearScreen();

	//Add grid to the scene
	//m_pMeshMngr->AddGridToRenderList();

	//Add skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
		
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release the model
	SafeDelete(m_pCreeper);

	//release the rigid body for the model
	SafeDelete(m_pCreeperRB);

	//release the model
	SafeDelete(m_pSteve);

	//release the rigid body for the model
	SafeDelete(m_pSteveRB);
	
	//release GUI
	ShutdownGUI();
}
