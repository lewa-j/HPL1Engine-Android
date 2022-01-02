/*
 * Copyright 2007-2010 (C) - Frictional Games
 *
 * This file is part of HPL1Engine
 *
 * For conditions of distribution and use, see copyright notice in LICENSE-tests
 */
#include <hpl.h>
#include <impl/SDLGameSetup.h>

#include "../Common/SimpleCamera.h"

using namespace hpl;

cGame *gpGame = nullptr;

class cSimpleUpdate : public iUpdateable
{
public:
	cSimpleUpdate() : iUpdateable("SimpleUpdate")
	{
		mpLowLevelGraphics = gpGame->GetGraphics()->GetLowLevel();

		//Create a world for the game objects to be in.
		mpWorld = gpGame->GetScene()->CreateWorld3D("Test");
		gpGame->GetScene()->SetWorld3D(mpWorld);

		//Set up some variables
		mlRenderMode = 2;
		mbPostEffects = false;
		mbGraphics2D = true;

		//////////////////////////////////////
		//Setup some buttons
		//1: Flat
		//2: Alpha
		//3: Simple CG
		//4: Simple CG w Alpha
		//5: Lighting CG
		//6: Lighting CG w Alpha
		//P: Posteffects on and off
		//G: 2D Graphics
		for (int i = 0; i < 6; ++i)
		{
			gpGame->GetInput()->AddAction(new cActionKeyboard(cString::ToString(i+1),
											gpGame->GetInput(),eKey (eKey_1+i) ));
		}
		gpGame->GetInput()->AddAction(new cActionKeyboard("PostEffects",gpGame->GetInput(),eKey (eKey_p)));
		gpGame->GetInput()->AddAction(new cActionKeyboard("Graphics2D",gpGame->GetInput(),eKey (eKey_g)));

		//////////////////////////////////////
		//Create a mesh and entity
		cMesh *pMesh =NULL;
		pMesh = gpGame->GetResources()->GetMeshManager()->CreateMesh("Woodbox.dae");
		mpEntity = mpWorld->CreateMeshEntity("Test",pMesh,false);
		mpEntity->SetPosition(cVector3f(0,0,0));

		//Do not render the entity, only us the PostScene function for rendering.
		mpEntity->SetVisible(false);

		//Background
		pMesh = gpGame->GetResources()->GetMeshManager()->CreateMesh("inverse_box.dae");
		mpBackground = mpWorld->CreateMeshEntity("Back",pMesh,false);
		mpBackground->SetMatrix(cMath::MatrixScale(cVector3f(4,4,4)));
		mpBackground->SetPosition(cVector3f(0,0,0));

		mpBackground->SetVisible(false);

		//////////////////////////////////////
		//Create a light and set it up
		mpLight = mpWorld->CreateLightPoint("Light");
		mpLight->SetFarAttenuation(44);
		mpLight->SetDiffuseColor(cColor(1,1,1,1));
		mpLight->SetCastShadows(true);
		mpLight->SetPosition(cVector3f(0,1,1));

		//////////////////////////////////////
		//Create simple gpu programs
		mpSimpleProgram = gpGame->GetGraphics()->CreateGpuProgramFromShaders("Simple", "SimpleVP.vert", "SimpleFP.frag");
		mpLightProgram = gpGame->GetGraphics()->CreateGpuProgramFromShaders("Light", "LightVP.vert", "LightFP.frag");

		mpPostEffectProgram = gpGame->GetGraphics()->CreateGpuProgramFromShaders("PostEffect", "SimpleVP.vert", "PostEffectFP.frag");

		//////////////////////////////////////
		//Dummy texture
		cTextureManager *pTextureManager = gpGame->GetResources()->GetTextureManager();

		mpDummyTexture = pTextureManager->Create2D("testar.jpg",true);

		//////////////////////////////////////
		//Create the screen buffer
		mpScreenBuffer = mpLowLevelGraphics->CreateTexture(cVector2l(800,600),
														32,cColor(0,0,0,0),false,
														eTextureType_Normal, eTextureTarget_2D);
		//set wrap mode to clamp
		mpScreenBuffer->SetWrapS(eTextureWrap_ClampToEdge);
		mpScreenBuffer->SetWrapT(eTextureWrap_ClampToEdge);

		//Load a font
		mpFont = gpGame->GetResources()->GetFontManager()->CreateFontData("viewer.fnt",12,32,128);

		//2d graphics
		mpGfxDrawer = gpGame->GetGraphics()->GetDrawer();
		mpGfxObj = mpGfxDrawer->CreateGfxObject("floor_grate01.tga","diffalpha2d");
	}

	//----------------------------------------------------------

	~cSimpleUpdate()
	{
		if(mpScreenBuffer) delete mpScreenBuffer;
	}

	//----------------------------------------------------------

	void Update(float afTimeStep)
	{
		for(int i=0; i<6; ++i)
		{
			if(gpGame->GetInput()->BecameTriggerd(cString::ToString(i+1)))
			{
				mlRenderMode = i;
			}
		}

		if(gpGame->GetInput()->BecameTriggerd("PostEffects"))
		{
			mbPostEffects = !mbPostEffects;
		}

		if(gpGame->GetInput()->BecameTriggerd("Graphics2D"))
		{
			mbGraphics2D = !mbGraphics2D;
		}
	}

	//----------------------------------------------------------

	void OnDraw()
	{
		if(mbGraphics2D==false) return;

		mpFont->Draw(	cVector3f(5,5,0),14,cColor(1,1),eFontAlign_Left,
						_W("WRITING SOME TEXT!"));

		mpGfxDrawer->DrawGfxObject(mpGfxObj,cVector3f(5, 40, 5),50,cColor(1,1));
	}

	//----------------------------------------------------------

	void OnPostGUIDraw()
	{
		if(mbPostEffects==false) return;

		cCamera3D *pCam = static_cast<cCamera3D*>(gpGame->GetScene()->GetCamera());

		//Set up the projection so we have 2D rendering
		mpLowLevelGraphics->SetDepthTestActive(false);
		mpLowLevelGraphics->SetIdentityMatrix(eMatrix::ModelView);
		mpLowLevelGraphics->SetOrthoProjection(mpLowLevelGraphics->GetVirtualSize(),-1000,1000);

		//Copy the current context (screen) to buffer texture
		mpLowLevelGraphics->CopyContextToTexure(mpScreenBuffer,0,cVector2l(800,600));

		//Set up a vertex vec
		tVertexVec vVertexVec; vVertexVec.resize(4);
		vVertexVec[0] = cVertex(cVector3f(0,0,0),		cVector3f(0,1,0),	cColor(1,1));
		vVertexVec[1] = cVertex(cVector3f(800,0,0),		cVector3f(1,1,0),	cColor(1,1));
		vVertexVec[2] = cVertex(cVector3f(800,600,0),	cVector3f(1,0,0),	cColor(1,1));
		vVertexVec[3] = cVertex(cVector3f(0,600,0),		cVector3f(0,0,0),	cColor(1,1));

		//Setup blending
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc::One,eBlendFunc::Zero);

		mpLowLevelGraphics->SetTexture(0,mpScreenBuffer);

		mpPostEffectProgram->Bind();
		mpPostEffectProgram->SetMatrixf("worldViewProj", eGpuProgramMatrix::ViewProjection, eGpuProgramMatrixOp::Identity);
		mpPostEffectProgram->SetFloat("timeCount",(float) gpGame->GetGameTime());

		//Draw
		mpLowLevelGraphics->DrawQuad(vVertexVec);

		mpPostEffectProgram->UnBind();
	}

	//----------------------------------------------------------

	void OnPostSceneDraw()
	{
		//Set up the base rendering params
		mpLowLevelGraphics->SetDepthTestActive(true);

		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc::One,eBlendFunc::Zero);

		RenderBackgroundZ(mpBackground->GetSubMeshEntity(0));

		//Iterate the sub meshes of the model
		for(int i=0; i<  mpEntity->GetSubMeshEntityNum(); i++)
		{
			switch(mlRenderMode)
			{
			case 0: RenderFlatSubMesh(mpEntity->GetSubMeshEntity(i)); break;
			case 1: RenderAlphaSubMesh(mpEntity->GetSubMeshEntity(i)); break;
			case 2: RenderSimpleCgSubMesh(mpEntity->GetSubMeshEntity(i), false); break;
			case 3: RenderSimpleCgSubMesh(mpEntity->GetSubMeshEntity(i), true); break;
			case 4: RenderLightingCgSubMesh(mpEntity->GetSubMeshEntity(i),false); break;
			case 5: RenderLightingCgSubMesh(mpEntity->GetSubMeshEntity(i),true); break;
			}
		}

		RenderBackgroundColor(mpBackground->GetSubMeshEntity(0));

		mpLowLevelGraphics->SetBlendActive(false);

		mpLowLevelGraphics->SetDepthTestActive(true);
		mpLowLevelGraphics->SetDepthWriteActive(true);
	}

	//----------------------------------------------------------

	void RenderBackgroundZ(cSubMeshEntity *pSubEntity)
	{
		cCamera3D *pCam = static_cast<cCamera3D*>(gpGame->GetScene()->GetCamera());

		//Get the data of the sub mesh
		cSubMesh *pSubMesh = pSubEntity->GetSubMesh();
		iVertexBuffer *pVtxBuffer = pSubEntity->GetVertexBuffer();
		iMaterial *pMaterial = pSubEntity->GetMaterial();
		iTexture *pTexture = pMaterial->GetTexture(eMaterialTexture_Diffuse);

		//Set matrix for the sub mesh
		cMatrixf mtxModel = cMath::MatrixMul(pCam->GetViewMatrix(), pSubEntity->GetWorldMatrix());
		mpLowLevelGraphics->SetMatrix(eMatrix::ModelView, mtxModel);

		//Set the texture
		mpLowLevelGraphics->SetTexture(0,pTexture);

		//Bind vertex buffer
		pVtxBuffer->Bind();

		//Setup rendering
		// Normal depth buffer and turn off all color rendering (increases render speed)
		mpLowLevelGraphics->SetDepthWriteActive(true);
		mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc::LessOrEqual);
		mpLowLevelGraphics->SetColorWriteActive(false,false,false,false);

		//Draw only to depth buffer
		// (Test removing this, nothing will be drawn..)
		pVtxBuffer->Draw();
		pVtxBuffer->UnBind();

		mpLowLevelGraphics->SetColorWriteActive(true,true,true,true);
		mpLowLevelGraphics->SetDepthWriteActive(false);
	}

	void RenderBackgroundColor(cSubMeshEntity *pSubEntity)
	{
		cCamera3D *pCam = static_cast<cCamera3D*>(gpGame->GetScene()->GetCamera());

		//Get the data of the sub mesh
		cSubMesh *pSubMesh = pSubEntity->GetSubMesh();
		iVertexBuffer *pVtxBuffer = pSubEntity->GetVertexBuffer();
		iMaterial *pMaterial = pSubEntity->GetMaterial();
		iTexture *pTexture = pMaterial->GetTexture(eMaterialTexture_Diffuse);

		//Set matrix for the sub mesh
		cMatrixf mtxModel = cMath::MatrixMul(pCam->GetViewMatrix(), pSubEntity->GetWorldMatrix());
		mpLowLevelGraphics->SetMatrix(eMatrix::ModelView, mtxModel);

		//Set the texture
		mpLowLevelGraphics->SetTexture(0,pTexture);

		//Bind vertex buffer
		pVtxBuffer->Bind();

		//Setup rendering
		// Normal depth buffer and turn off all color rendering (increases render speed)
		mpLowLevelGraphics->SetDepthWriteActive(false);

		//Draw only to depth buffer
		// (Test removing this, nothing will be drawn..)
		pVtxBuffer->Draw();
		pVtxBuffer->UnBind();
	}

	//----------------------------------------------------------


	//////////////////////////////////////////////////
	// Draw the sub mesh flat without any shading
	void RenderFlatSubMesh(cSubMeshEntity *pSubEntity)
	{
		cCamera3D *pCam = static_cast<cCamera3D*>(gpGame->GetScene()->GetCamera());

		//Get the data of the sub mesh
		cSubMesh *pSubMesh = pSubEntity->GetSubMesh();
		iVertexBuffer *pVtxBuffer = pSubEntity->GetVertexBuffer();
		iMaterial *pMaterial = pSubEntity->GetMaterial();
		iTexture *pTexture = pMaterial->GetTexture(eMaterialTexture_Diffuse);

		//Set matrix for the sub mesh
		cMatrixf mtxModel = cMath::MatrixMul(pCam->GetViewMatrix(), pSubEntity->GetWorldMatrix());
		//mtxModel = cMath::MatrixMul(mtxModel, cMath::MatrixRotateX(cMath::ToRad(35)));
		mpLowLevelGraphics->SetMatrix(eMatrix::ModelView, mtxModel);


		//Set the texture to be used, not setting this will only draw vertex colors.
		mpLowLevelGraphics->SetTexture(0,pTexture);

		//Setup depth buffer behaviour,
		mpLowLevelGraphics->SetDepthWriteActive(true);
		mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc::LessOrEqual);

		//cPlanef plane(0,1,0,-1);
		//plane.FromNormalPoint(cVector3f(0,1,0),cVector3f(0,0.35f,0));
		//mpLowLevelGraphics->SetClipPlane(0, plane);
		//mpLowLevelGraphics->SetClipPlaneActive(0, true);

		//Draw vertex buffer
		pVtxBuffer->Bind();
		pVtxBuffer->Draw();
		pVtxBuffer->UnBind();

		mpLowLevelGraphics->SetClipPlaneActive(0, false);

	}

	//----------------------------------------------------------

	//////////////////////////////////////////////////
	// Draw the sub mesh using alpha blending
	// Also, we try to copy the engine rendering by first drawing only to the z-buffer
	void RenderAlphaSubMesh(cSubMeshEntity *pSubEntity)
	{
		cCamera3D *pCam = static_cast<cCamera3D*>(gpGame->GetScene()->GetCamera());

		//Get the data of the sub mesh
		cSubMesh *pSubMesh = pSubEntity->GetSubMesh();
		iVertexBuffer *pVtxBuffer = pSubEntity->GetVertexBuffer();
		iMaterial *pMaterial = pSubEntity->GetMaterial();
		iTexture *pTexture = pMaterial->GetTexture(eMaterialTexture_Diffuse);

		//Set matrix for the sub mesh
		cMatrixf mtxModel = cMath::MatrixMul(pCam->GetViewMatrix(), pSubEntity->GetWorldMatrix());
		mpLowLevelGraphics->SetMatrix(eMatrix::ModelView, mtxModel);

		//Set the texture
		mpLowLevelGraphics->SetTexture(0,pTexture);

		//Alpha test
		// Here we set up a test so texels with alpha below 0.5 (1 = max) is not drawm
		mpLowLevelGraphics->SetAlphaTestActive(true);
		mpLowLevelGraphics->SetAlphaTestFunc(eAlphaTestFunc::GreaterOrEqual, 0.5f);

		//Bind vertex buffer
		pVtxBuffer->Bind();

		////////////////////////////////////////
		// FIRST PASS - Rendering z buffer

		//Setup rendering
		// Normal depth buffer and turn off all color rendering (increases render speed)
		mpLowLevelGraphics->SetDepthWriteActive(true);
		mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc::LessOrEqual);
		mpLowLevelGraphics->SetColorWriteActive(false,false,false,false);

		//Draw only to depth buffer
		// (Test removing this, nothing will be drawn..)
		pVtxBuffer->Draw();

		//Alpha testing only needed during z buffer rendering
		mpLowLevelGraphics->SetAlphaTestActive(false);

		////////////////////////////////////////
		// SECOND PASS - Rendering color

		//Setup rendering
		// No need to write to the z buffer and only draw if it is the exact same value
		// as the z buffer, this is needed for transparent areas to work and it also
		// when you have several light sources. When you have several light sources, you simply
		// rendered the model several times additively
		mpLowLevelGraphics->SetDepthWriteActive(false);
		mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc::Equal);
		mpLowLevelGraphics->SetColorWriteActive(true,true,true,true);
		mpLowLevelGraphics->SetAlphaTestActive(false);

		//Draw colors
		pVtxBuffer->Draw();

		pVtxBuffer->UnBind();
	}

	//----------------------------------------------------------

	//////////////////////////////////////////////////
	// Render the cube using a simple cg program
	void RenderSimpleCgSubMesh(cSubMeshEntity *pSubEntity, bool abUseAlpha)
	{
		cCamera3D *pCam = static_cast<cCamera3D*>(gpGame->GetScene()->GetCamera());

		//Get the data of the sub mesh
		cSubMesh *pSubMesh = pSubEntity->GetSubMesh();
		iVertexBuffer *pVtxBuffer = pSubEntity->GetVertexBuffer();
		iMaterial *pMaterial = pSubEntity->GetMaterial();
		iTexture *pTexture = pMaterial->GetTexture(eMaterialTexture_Diffuse);

		//Set matrix for the sub mesh
		cMatrixf mtxModel = cMath::MatrixMul(pCam->GetViewMatrix(), pSubEntity->GetWorldMatrix());
		mpLowLevelGraphics->SetMatrix(eMatrix::ModelView, mtxModel);

		//Set the texture
		//Since we are binding the texture directly to the program this can be set to NULL.
		mpLowLevelGraphics->SetTexture(0,NULL);

		//Bind vertex buffer
		pVtxBuffer->Bind();

		//Setup alpha test if needed.
		if(abUseAlpha)
		{
			mpLowLevelGraphics->SetAlphaTestActive(true);
			mpLowLevelGraphics->SetAlphaTestFunc(eAlphaTestFunc::GreaterOrEqual, 0.5f);
		}

		///////////////////////////////////////////////
		// FIRST PASS - Rendering z buffer

		//Set up rendering
		mpLowLevelGraphics->SetDepthWriteActive(true);
		mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc::LessOrEqual);
		mpLowLevelGraphics->SetColorWriteActive(false,false,false,false);

		//Setup program
		//This is needed for ATI cards, else the vertex buffer will not be transformted in the
		//same way as with the vertex buffer using the vertex program and z problems will occur.
		mpSimpleProgram->Bind();
		mpSimpleProgram->SetMatrixf( "worldViewProj",eGpuProgramMatrix::ViewProjection,
										eGpuProgramMatrixOp::Identity);

		//Set texture
		mpLowLevelGraphics->SetTexture(0,pTexture);

		//Draw only to depth buffer
		pVtxBuffer->Draw();

		//Unbind program
		mpSimpleProgram->UnBind();

		//Alpha testing only needed during z buffer rendering
		if(abUseAlpha)
		{
			mpLowLevelGraphics->SetAlphaTestActive(false);
		}

		///////////////////////////////////////////////
		// SECOND PASS - Rendering color

		//Setup rendering
		mpLowLevelGraphics->SetDepthWriteActive(false);
		mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc::Equal);
		mpLowLevelGraphics->SetColorWriteActive(true,true,true,true);

		//Setup program
		mpSimpleProgram->Bind();
		mpSimpleProgram->SetMatrixf(	"worldViewProj",eGpuProgramMatrix::ViewProjection,
										eGpuProgramMatrixOp::Identity);
		mpSimpleProgram->SetFloat("timeCount",(float) gpGame->GetGameTime());

		//Set the texture directly to the program using parameter name
		//mpSimpleProgram->SetTexture("diffuseMap",pTexture);

		//Set texture direct to program unit
		//mpSimpleProgram->SetTextureToUnit(0,pTexture);
		mpLowLevelGraphics->SetTexture(0, pTexture);

		//Draw vertex buffer
		pVtxBuffer->Draw();

		//Unbinf vertex buffer
		pVtxBuffer->UnBind();

		//Unbind program
		mpSimpleProgram->UnBind();
	}

	//----------------------------------------------------------

	//////////////////////////////////////////////////
	// Render the cube using a lighting cg program
	void RenderLightingCgSubMesh(cSubMeshEntity *pSubEntity, bool abUseAlpha)
	{
		cCamera3D *pCam = static_cast<cCamera3D*>(gpGame->GetScene()->GetCamera());

		//Get the data of the sub mesh
		cSubMesh *pSubMesh = pSubEntity->GetSubMesh();
		iVertexBuffer *pVtxBuffer = pSubEntity->GetVertexBuffer();
		iMaterial *pMaterial = pSubEntity->GetMaterial();
		iTexture *pTexture = pMaterial->GetTexture(eMaterialTexture_Diffuse);

		//Set matrix for the sub mesh
		cMatrixf mtxModel = cMath::MatrixMul(pCam->GetViewMatrix(), pSubEntity->GetWorldMatrix());
		mpLowLevelGraphics->SetMatrix(eMatrix::ModelView, mtxModel);

		//Set the texture
		mpLowLevelGraphics->SetTexture(0,pTexture);

		//Bind vertex buffer
		pVtxBuffer->Bind();

		//Setup alpha test if needed.
		if(abUseAlpha)
		{
			mpLowLevelGraphics->SetAlphaTestActive(true);
			mpLowLevelGraphics->SetAlphaTestFunc(eAlphaTestFunc::GreaterOrEqual, 0.5f);
		}

		///////////////////////////////////////////////
		// FIRST PASS - Rendering z buffer

		//Set up rendering
		mpLowLevelGraphics->SetDepthWriteActive(true);
		mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc::LessOrEqual);
		mpLowLevelGraphics->SetColorWriteActive(false,false,false,false);

		//Setup program
		//This is needed for ATI cards, else the vertex buffer will not be transformted in the
		//same way as with the vertex buffer using the vertex program and z problems will occur.
		mpSimpleProgram->Bind();
		mpSimpleProgram->SetMatrixf(	"worldViewProj",eGpuProgramMatrix::ViewProjection,
										eGpuProgramMatrixOp::Identity);


		//Draw only to depth buffer
		pVtxBuffer->Draw();

		//Unbind program
		mpSimpleProgram->UnBind();

		//Alpha testing only needed during z buffer rendering
		if(abUseAlpha)
		{
			mpLowLevelGraphics->SetAlphaTestActive(false);
		}

		///////////////////////////////////////////////
		// SECOND PASS - Rendering color

		//Setup rendering
		mpLowLevelGraphics->SetDepthWriteActive(false);
		mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc::Equal);
		mpLowLevelGraphics->SetColorWriteActive(true,true,true,true);

		//Setup program
		mpLightProgram->Bind();
		mpLightProgram->SetMatrixf(	"worldViewProj",eGpuProgramMatrix::ViewProjection,
										eGpuProgramMatrixOp::Identity);
		mpLightProgram->SetVec3f("LightPos", mpLight->GetWorldPosition());
		mpLightProgram->SetColor4f("LightColor",mpLight->GetDiffuseColor());

		//Draw in color
		pVtxBuffer->Draw();

		//Unbind program
		mpLightProgram->UnBind();

		//Unbind vertex buffer
		pVtxBuffer->UnBind();
	}

	//----------------------------------------------------------

private:
	cMeshEntity *mpEntity = nullptr;
	cMeshEntity *mpBackground = nullptr;

	iFontData *mpFont = nullptr;
	cGfxObject *mpGfxObj = nullptr;
	cGraphicsDrawer *mpGfxDrawer = nullptr;

	iGpuProgram *mpSimpleProgram = nullptr;
	iGpuProgram *mpLightProgram = nullptr;
	iGpuProgram *mpPostEffectProgram = nullptr;

	iTexture *mpDummyTexture = nullptr;
	iTexture *mpDummyTexture2 = nullptr;

	iTexture *mpScreenBuffer = nullptr;

	int mlRenderMode = 0;
	bool mbPostEffects = false;
	bool mbGraphics2D = false;

	iLowLevelGraphics* mpLowLevelGraphics = nullptr;
	cWorld3D* mpWorld = nullptr;

	iLight3D *mpLight = nullptr;
	iLight3D *mpLight2 = nullptr;
};

int hplMain(const tString& asArg)
{
	iResourceBase::SetLogCreateAndDelete(true);
	iGpuProgram::SetLogDebugInformation(true);

	//Init the game engine
	gpGame = new cGame(new cSDLGameSetup(),800,600,32,false,60);
	gpGame->GetGraphics()->GetLowLevel()->SetVsyncActive(false);

	//Add resources
	gpGame->GetResources()->AddResourceDir("textures");
	gpGame->GetResources()->AddResourceDir("models");
	gpGame->GetResources()->AddResourceDir("fonts");
	gpGame->GetResources()->AddResourceDir("maps");

	//Add updates
	cSimpleUpdate Update;
	gpGame->GetUpdater()->AddUpdate("Default", &Update);

	cSimpleCamera cameraUpdate(gpGame,5,cVector3f(0,0,2),false);
	gpGame->GetUpdater()->AddUpdate("Default", &cameraUpdate);

	//Run the engine
	gpGame->Run();

	//Delete the engine
	delete gpGame;

	return 0;
}
