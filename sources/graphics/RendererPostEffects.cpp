/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "graphics/RendererPostEffects.h"
#include "graphics/Texture.h"
#include "scene/Scene.h"
#include "system/LowLevelSystem.h"
#include "graphics/Graphics.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/GPUProgram.h"
#include "resources/LowLevelResources.h"
#include "math/Math.h"
#include "resources/Resources.h"
#include "resources/GpuShaderManager.h"
#include "graphics/RenderList.h"
#include "graphics/Renderer3D.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cRendererPostEffects::cRendererPostEffects(cGraphics *apGraphics, cResources* apResources,
												cRenderList *apRenderList)
	{
		mpGraphics = apGraphics;
		mpLowLevelGraphics = apGraphics->GetLowLevel();
		mpLowLevelResources = apResources->GetLowLevel();
		mpResources = apResources;
		mpRenderer3D = apGraphics->GetRenderer3D();

		mpGpuManager = mpResources->GetGpuShaderManager();

		mvScreenSize = mpLowLevelGraphics->GetScreenSize();

		mpRenderList = apRenderList;

		///////////////////////////////////////////
		// Create screen buffers
		Log(" Creating screen buffers size %s\n",mvScreenSize.ToString().c_str());
		if(mpLowLevelGraphics->GetCaps(eGraphicCaps_TextureTargetRectangle))
		{
			for(int i=0;i<2;i++)
			{
				mpScreenBuffer[i] = mpLowLevelGraphics->CreateTexture(cVector2l(
					(int)mvScreenSize.x,(int)mvScreenSize.y),32,cColor(0,0,0,0),false,
					eTextureType_Normal, eTextureTarget_Rect);

				if(mpScreenBuffer[i]==NULL)
				{
					Error("Couldn't create screenbuffer!\n");
					mpScreenBuffer[0] = NULL;
					mpScreenBuffer[1] = NULL;
					break;
				}
				mpScreenBuffer[i]->SetWrapS(eTextureWrap_ClampToEdge);
				mpScreenBuffer[i]->SetWrapT(eTextureWrap_ClampToEdge);
			}
		}
		else
		{
			mpScreenBuffer[0] = NULL;
			mpScreenBuffer[1] = NULL;
			Error("Texture rectangle not supported. Posteffects will be turned off.");
		}

		///////////////////////////////////////////
		// Create programs

		Log(" Creating programs\n");

		/////////////////
		//Blur programs
		mbBlurFallback = false; //Set to true if the fallbacks are used.

		mpBlurRectP = apGraphics->CreateGpuProgramFromShaders("BlurRect", "PostEffect_Blur.vert", "PostEffect_Blur_Rect.frag");
		if (!mpBlurRectP) Error("Couldn't load 'PostEffect_Blur'!\n");
		/*if(!mpBlurRectFP)
		{
			mbBlurFallback = true;
			Log(" Using Blur Rect FP fallback\n");
			mpBlurRectFP = mpGpuManager->CreateProgram("PostEffect_Fallback01_Blur_Rect.frag","main",eGpuProgramType_Fragment);
			if(!mpBlurRectFP) Error("Couldn't load 'PostEffect_Blur_Rect'!\n");
		}*/

		mpBlur2dP = apGraphics->CreateGpuProgramFromShaders("Blur2D", "PostEffect_Blur.vert", "PostEffect_Blur_2D.frag");
		/*if(!mpBlur2dFP)
		{
			mbBlurFallback = true;
			Log(" Using Blur 2D FP fallback\n");
			mpBlur2dFP = mpGpuManager->CreateProgram("PostEffect_Fallback01_Blur_2D.frag","main",eGpuProgramType_Fragment);
			if(!mpBlur2dFP) Error("Couldn't load 'PostEffect_Blur_2D'!\n");
		}*/

		/////////////////
		// Bloom program
		mpBloomP = apGraphics->CreateGpuProgramFromShaders("Bloom", "PostEffect_Bloom.vert", "PostEffect_Bloom.frag");
		if(!mpBloomP) Error("Couldn't load 'PostEffect_Bloom'!\n");

		//Bloom blur textures
		mpBloomBlurTexture = mpLowLevelGraphics->CreateTexture(
												cVector2l(256,256),
												32,cColor(0,0,0,0),false,
												eTextureType_Normal, eTextureTarget_2D);

		if(mpBloomBlurTexture == NULL) {
			Error("Couldn't create bloom blur textures!\n");
		}
		else {
			mpBloomBlurTexture->SetWrapS(eTextureWrap_ClampToEdge);
			mpBloomBlurTexture->SetWrapT(eTextureWrap_ClampToEdge);
		}


		/////////////////
		// MotionBlur programs
		// Disable the dynamic loop version on Mac OS X until we figure why it doesn't work
		mpMotionBlurP = apGraphics->CreateGpuProgramFromShaders("MotionBlur", "PostEffect_Motion.vert", "PostEffect_Motion.frag");
		if(!mpMotionBlurP)
		{
			Log("Dynamic loops in motion blur fp not supported, loading static instead.\n");
			mpMotionBlurP = apGraphics->CreateGpuProgramFromShaders("MotionBlur_staticloop", "PostEffect_Motion.vert", "PostEffect_Motion_staticloop.frag");
			if(!mpMotionBlurP)	Error("Couldn't load 'PostEffect_Motion'!\n");
		}

		/////////////////
		// Depth of Field program
		mpDepthOfFieldP = apGraphics->CreateGpuProgramFromShaders("DepthOfField", "PostEffect_DoF.vert", "PostEffect_DoF.frag");
		if(!mpDepthOfFieldP) Error("Couldn't load 'PostEffect_DoF'!\n");

		//Depth of Field blur textures
		mpDofBlurTexture = mpLowLevelGraphics->CreateTexture(	cVector2l(256,256),
																32,cColor(0,0,0,0),false,
																eTextureType_Normal, eTextureTarget_2D);

		if(mpDofBlurTexture == NULL) {
			Error("Couldn't create Depth of Field blur textures!\n");
		}
		else {
			mpDofBlurTexture->SetWrapS(eTextureWrap_ClampToEdge);
			mpDofBlurTexture->SetWrapT(eTextureWrap_ClampToEdge);
		}

		Log("  RendererPostEffects created\n");

		////////////////////////////////////
		// Variable setup

		//General
		mbActive = false;
		mvTexRectVtx.resize(4);

		//Bloom
		mbBloomActive = false;
		mfBloomSpread = 2.0f;

		//Motion blur
		mbMotionBlurActive = false;
		mfMotionBlurAmount = 1.0f;
		mbMotionBlurFirstTime = true;

		//Depth of Field
		mbDofActive = false;
		mfDofMaxBlur = 1.0f;
		mfDofFocalPlane = 1.0f;
		mfDofNearPlane = 2.0f;
		mfDofFarPlane = 3.0f;
	}

	//-----------------------------------------------------------------------

	cRendererPostEffects::~cRendererPostEffects()
	{
		for(int i=0;i<2;i++)
			if(mpScreenBuffer[i])hplDelete(mpScreenBuffer[i]);

		if(mpBlur2dP) mpGraphics->DestroyGpuProgram(mpBlur2dP);
		if(mpBlurRectP) mpGraphics->DestroyGpuProgram(mpBlurRectP);
		if(mpBloomP) mpGraphics->DestroyGpuProgram(mpBloomP);
		if(mpMotionBlurP) mpGraphics->DestroyGpuProgram(mpMotionBlurP);
		if(mpDepthOfFieldP) mpGraphics->DestroyGpuProgram(mpDepthOfFieldP);

		if(mpBloomBlurTexture) hplDelete(mpBloomBlurTexture);
		if(mpDofBlurTexture) hplDelete(mpDofBlurTexture);

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cRendererPostEffects::SetBloomActive(bool abX)
	{
		mbBloomActive = abX;
	}

	//-----------------------------------------------------------------------

	void cRendererPostEffects::SetMotionBlurActive(bool abX)
	{
		mbMotionBlurActive = abX;
		mbMotionBlurFirstTime = true;
	}

	//-----------------------------------------------------------------------

	void cRendererPostEffects::Render()
	{
		if(mpScreenBuffer[0] == NULL || mpScreenBuffer[1] == NULL) return;
		if(!mpLowLevelGraphics->GetCaps(eGraphicCaps_TextureTargetRectangle)) return;

		if(mbActive==false ||
			(	mImageTrailData.mbActive==false && mbBloomActive==false &&
				mbMotionBlurActive==false && mbDofActive==false) )
		{
			return;
		}

		mvScreenSize = mpLowLevelGraphics->GetScreenSize();

		RenderDepthOfField();
		RenderMotionBlur();

		mpLowLevelGraphics->SetDepthTestActive(false);
		mpLowLevelGraphics->PushMatrix(eMatrix_ModelView);
		mpLowLevelGraphics->SetIdentityMatrix(eMatrix_ModelView);
		mpLowLevelGraphics->SetOrthoProjection(mpLowLevelGraphics->GetVirtualSize(),-1000,1000);

		RenderBloom();

		RenderImageTrail();

		mpLowLevelGraphics->PopMatrix(eMatrix_ModelView);
	}

	//-----------------------------------------------------------------------

	//TODO: Support source texture as 2D

	void cRendererPostEffects::RenderBlurTexture(iTexture *apDestination, iTexture *apSource,
													float afBlurAmount)
	{
		bool bProgramsLoaded = false;
		if(mpBlurRectP && mpBlur2dP) bProgramsLoaded = true;

		iLowLevelGraphics *pLowLevel = mpLowLevelGraphics;
		cVector2l vBlurSize = cVector2l(apDestination->GetWidth(), apDestination->GetHeight());
		cVector2f vBlurDrawSize;
		vBlurDrawSize.x = ((float)vBlurSize.x/mvScreenSize.x) * mpLowLevelGraphics->GetVirtualSize().x;
		vBlurDrawSize.y = ((float)vBlurSize.y/mvScreenSize.y) * mpLowLevelGraphics->GetVirtualSize().y;

		pLowLevel->SetBlendActive(false);

		///////////////////////////////////////////
		//Horizontal blur pass

		//Shader setup
		if(bProgramsLoaded)
		{
			//Setup program
			mpBlurRectP->Bind();
			mpBlurRectP->SetFloat("xOffset", 1);
			mpBlurRectP->SetFloat("yOffset", 0);
			mpBlurRectP->SetFloat("amount", afBlurAmount);
			mpBlurRectP->SetMatrixf("worldViewProj", eGpuProgramMatrix::ViewProjection, eGpuProgramMatrixOp::Identity);
		}

		//Draw the screen texture with blur
		pLowLevel->SetTexture(0,apSource);
		if(mbBlurFallback){
			mpLowLevelGraphics->SetTexture(1,apSource);
			mpLowLevelGraphics->SetTexture(2,apSource);
		}

		{
			mvTexRectVtx[0] = cVertex(cVector3f(0, 0, 40), cVector2f(0, mvScreenSize.y), cColor(1, 1.0f));
			mvTexRectVtx[1] = cVertex(cVector3f(vBlurDrawSize.x, 0, 40), cVector2f(mvScreenSize.x, mvScreenSize.y), cColor(1, 1.0f));
			mvTexRectVtx[2] = cVertex(cVector3f(vBlurDrawSize.x, vBlurDrawSize.y, 40), cVector2f(mvScreenSize.x, 0), cColor(1, 1.0f));
			mvTexRectVtx[3] = cVertex(cVector3f(0, vBlurDrawSize.y, 40), cVector2f(0, 0), cColor(1, 1.0f));

			mpLowLevelGraphics->DrawQuad(mvTexRectVtx);
		}

		pLowLevel->CopyContextToTexure(apDestination,0,vBlurSize);

		///////////////////////////////////////////
		//Vertical blur pass

		//Setup shaders
		//Shader setup
		if(bProgramsLoaded)
		{
			mpBlurRectP->UnBind();
			mpBlur2dP->Bind();
			mpBlur2dP->SetFloat("xOffset", 0);
			mpBlur2dP->SetFloat("yOffset", 1);
			mpBlur2dP->SetFloat("amount", (1 / pLowLevel->GetScreenSize().x) * afBlurAmount);
		}

		//Set texture and draw
		pLowLevel->SetTexture(0,apDestination);
		if(mbBlurFallback){
			mpLowLevelGraphics->SetTexture(1,apDestination);
			mpLowLevelGraphics->SetTexture(2,apDestination);
		}

		{
			mvTexRectVtx[0] = cVertex(cVector3f(0,0,40),cVector2f(0,1),cColor(1,1.0f) );
			mvTexRectVtx[1] = cVertex(cVector3f(vBlurDrawSize.x,0,40),cVector2f(1,1),cColor(1,1.0f));
			mvTexRectVtx[2] = cVertex(cVector3f(vBlurDrawSize.x,vBlurDrawSize.y,40),cVector2f(1,0),cColor(1,1.0f));
			mvTexRectVtx[3] = cVertex(cVector3f(0,vBlurDrawSize.y,40),cVector2f(0,0),cColor(1,1.0f));

			mpLowLevelGraphics->DrawQuad(mvTexRectVtx);
		}

		//Shader setup
		if(bProgramsLoaded)
		{
			mpBlur2dP->UnBind();
		}

		pLowLevel->CopyContextToTexure(apDestination,0, vBlurSize);

		if(mbBlurFallback){
			mpLowLevelGraphics->SetTexture(1,NULL);
			mpLowLevelGraphics->SetTexture(2,NULL);
		}
	}

	//-----------------------------------------------------------------------


	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cRendererPostEffects::RenderDepthOfField()
	{
		if (mbDofActive == false) return;

		if (mpDepthOfFieldP == NULL) return;

		//////////////////////////////
		// Setup
		cCamera3D *pCam = mpRenderList->GetCamera();


		iTexture *pScreenTexture = mpScreenBuffer[mImageTrailData.mlCurrentBuffer==0?1:0];

		//Size of the virtual screen
		cVector2f vVirtSize = mpLowLevelGraphics->GetVirtualSize();

		//Copy screen to texture
		mpLowLevelGraphics->CopyContextToTexure(pScreenTexture,0, cVector2l((int)mvScreenSize.x,(int)mvScreenSize.y));

		//Set up things needed for blurring
		mpLowLevelGraphics->SetDepthWriteActive(false);
		mpLowLevelGraphics->SetDepthTestActive(false);
		mpLowLevelGraphics->SetIdentityMatrix(eMatrix_ModelView);
		mpLowLevelGraphics->SetOrthoProjection(mpLowLevelGraphics->GetVirtualSize(),-1000,1000);

		//Render blur texture
		RenderBlurTexture(mpDofBlurTexture,pScreenTexture,2.0f);

		//Set texture
		mpLowLevelGraphics->SetTexture(0,pScreenTexture);
		mpLowLevelGraphics->SetTexture(1,mpDofBlurTexture);


		//Render entire screen textur to screen, this to cover up what has been done by the
		//blur filter, should have support for frame_buffer_object so this can be skipped.
		if(mpRenderer3D->GetSkyBoxActive())
		{
			tVector3fVec vUvVec; vUvVec.resize(4);

			vUvVec[0] = cVector2f(0,1);
			vUvVec[1] = cVector2f(1,1);
			vUvVec[2] = cVector2f(1,0);
			vUvVec[3] = cVector2f(0,0);

			mvTexRectVtx[0] = cVertex(cVector3f(0,0,40),					cVector2f(0,mvScreenSize.y),			cColor(1,1.0f) );
			mvTexRectVtx[1] = cVertex(cVector3f(vVirtSize.x,0,40),			cVector2f(mvScreenSize.x,mvScreenSize.y),	cColor(1,1.0f));
			mvTexRectVtx[2] = cVertex(cVector3f(vVirtSize.x,vVirtSize.y,40),cVector2f(mvScreenSize.x,0),			cColor(1,1.0f));
			mvTexRectVtx[3] = cVertex(cVector3f(0,vVirtSize.y,40),			cVector2f(0,0),						cColor(1,1.0f));

			mpLowLevelGraphics->SetActiveTextureUnit(1);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorSource0,eTextureSource_Texture);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorSource1,eTextureSource_Previous);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorSource2,eTextureSource_Constant);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc, eTextureFunc_Interpolate);
			mpLowLevelGraphics->SetTextureConstantColor(cColor(mfDofMaxBlur,mfDofMaxBlur));

			mpLowLevelGraphics->DrawQuadMultiTex(mvTexRectVtx,vUvVec);

			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc, eTextureFunc_Modulate);
		}

		//Set things back to normal
		mpLowLevelGraphics->SetMatrix(eMatrix_Projection, pCam->GetProjectionMatrix());

		///////////////////////////////////////////
		//Draw motion blur objects
		mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc_Equal);
		mpLowLevelGraphics->SetDepthTestActive(true);
		mpLowLevelGraphics->SetBlendActive(false);

		//Setup
		mpDepthOfFieldP->Bind();
		mpDepthOfFieldP->SetVec3f("planes", cVector3f(mfDofNearPlane, mfDofFocalPlane, mfDofFarPlane));
		mpDepthOfFieldP->SetFloat("maxBlur", mfDofMaxBlur);
		mpDepthOfFieldP->SetVec2f("screenSize", mvScreenSize);

		//////////////////
		//Render objects
		cMotionBlurObjectIterator it = mpRenderList->GetMotionBlurIterator();
		while(it.HasNext())
		{
			iRenderable *pObject = it.Next();
			cMatrixf *pMtx = pObject->GetModelMatrix(pCam);

			//////////////////
			//Non static models
			if(pMtx)
			{
				mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,cMath::MatrixMul(pCam->GetViewMatrix(), *pMtx));

				mpDepthOfFieldP->SetMatrixf("worldViewProj",	eGpuProgramMatrix::ViewProjection,
															eGpuProgramMatrixOp::Identity);
			}
			//////////////////
			//NULL Model view matrix (static)
			else
			{
				mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,pCam->GetViewMatrix());

				mpDepthOfFieldP->SetMatrixf("worldViewProj",	eGpuProgramMatrix::ViewProjection,
					eGpuProgramMatrixOp::Identity);
			}

			pObject->GetVertexBuffer()->Bind();

			pObject->GetVertexBuffer()->Draw();

			pObject->GetVertexBuffer()->UnBind();


			//Set the previous postion to the current
			if(pMtx) pObject->SetPrevMatrix(*pMtx);
		}

		mpLowLevelGraphics->SetDepthTestFunc(eDepthTestFunc_LessOrEqual);
		mpLowLevelGraphics->SetDepthWriteActive(true);

		//Reset stuff
		mpDepthOfFieldP->UnBind();
		mpLowLevelGraphics->SetTexture(0,NULL);
		mpLowLevelGraphics->SetTexture(1,NULL);
	}

	//-----------------------------------------------------------------------

	void cRendererPostEffects::RenderMotionBlur()
	{
		if (mbMotionBlurActive == false) return;

		if (mpMotionBlurP == NULL) return;

		//////////////////////////////
		// Setup
		iTexture *pScreenTexture = mpScreenBuffer[mImageTrailData.mlCurrentBuffer==0?1:0];

		//Size of the virtual screen
		cVector2f vVirtSize = mpLowLevelGraphics->GetVirtualSize();

		//Copy screen to texture
		mpLowLevelGraphics->CopyContextToTexure(pScreenTexture,0, cVector2l((int)mvScreenSize.x,(int)mvScreenSize.y));

		///////////////////////////////////////////
		//Draw motion blur objects
		mpLowLevelGraphics->SetDepthTestActive(true);
		mpLowLevelGraphics->SetBlendActive(false);

		cCamera3D *pCam = mpRenderList->GetCamera();
		if(mbMotionBlurFirstTime)
		{
			pCam->SetPrevView(pCam->GetViewMatrix());
			pCam->SetPrevProjection(pCam->GetProjectionMatrix());
			mbMotionBlurFirstTime = false;
		}

		cMotionBlurObjectIterator it = mpRenderList->GetMotionBlurIterator();

		//Setup
		mpMotionBlurP->Bind();
		mpMotionBlurP->SetFloat("blurScale",mfMotionBlurAmount);
		mpMotionBlurP->SetVec2f("halfScreenSize",
								cVector2f(	(float)pScreenTexture->GetWidth()/2.0f,
											(float)pScreenTexture->GetHeight()/2.0f));

		mpLowLevelGraphics->SetTexture(0,pScreenTexture);


		//////////////////
		//Render objects
		while(it.HasNext())
		{
			iRenderable *pObject = it.Next();
			cMatrixf *pMtx = pObject->GetModelMatrix(pCam);

			//////////////////
			//Non static models
			if(pMtx)
			{
				cMatrixf mtxPrev = pObject->GetPrevMatrix();

				mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,cMath::MatrixMul(pCam->GetViewMatrix(), *pMtx));

				mpMotionBlurP->SetMatrixf("worldViewProj",	eGpuProgramMatrix::ViewProjection,
															eGpuProgramMatrixOp::Identity);

				cMatrixf mtxModelView =		cMath::MatrixMul(pCam->GetViewMatrix(), *pMtx);
				cMatrixf mtxPrevModelView = cMath::MatrixMul(pCam->GetPrevView(),mtxPrev);

				cMatrixf mtxPrevViewProj = cMath::MatrixMul(pCam->GetPrevProjection(), mtxPrevModelView);

				mpMotionBlurP->SetMatrixf("prevWorldViewProj", mtxPrevViewProj);
				mpMotionBlurP->SetMatrixf("modelView",mtxModelView);
				mpMotionBlurP->SetMatrixf("prevModelView",mtxPrevModelView);
			}
			//////////////////
			//NULL Model view matrix (static)
			else
			{
				mpLowLevelGraphics->SetMatrix(eMatrix_ModelView,pCam->GetViewMatrix());

				mpMotionBlurP->SetMatrixf("worldViewProj",	eGpuProgramMatrix::ViewProjection,
															eGpuProgramMatrixOp::Identity);

				const cMatrixf &mtxModelView =	pCam->GetViewMatrix();
				const cMatrixf &mtxPrevModelView = pCam->GetPrevView();

				cMatrixf mtxPrevViewProj = cMath::MatrixMul(pCam->GetPrevProjection(), mtxPrevModelView);

				mpMotionBlurP->SetMatrixf("prevWorldViewProj", mtxPrevViewProj);
				mpMotionBlurP->SetMatrixf("modelView",mtxModelView);
				mpMotionBlurP->SetMatrixf("prevModelView",mtxPrevModelView);
			}

			pObject->GetVertexBuffer()->Bind();

			pObject->GetVertexBuffer()->Draw();

			pObject->GetVertexBuffer()->UnBind();


			//Set the previous postion to the current
			if(pMtx) pObject->SetPrevMatrix(*pMtx);
		}

		//Reset stuff
		mpMotionBlurP->UnBind();
		mpLowLevelGraphics->SetTexture(0,NULL);
		mpLowLevelGraphics->SetTexture(1,NULL);


		//Set the new pervious values.
		pCam->SetPrevView(pCam->GetViewMatrix());
		pCam->SetPrevProjection(pCam->GetProjectionMatrix());
	}

	//-----------------------------------------------------------------------

	void cRendererPostEffects::RenderBloom()
	{
		if (mbBloomActive == false) return;

		if (mpBloomP == NULL) return;

		//////////////////////////////
		// Setup

		iTexture *pScreenTexture = mpScreenBuffer[mImageTrailData.mlCurrentBuffer==0?1:0];

		//Copy screen to texture
		mpLowLevelGraphics->CopyContextToTexure(pScreenTexture,0,
												cVector2l((int)mvScreenSize.x,(int)mvScreenSize.y));

		//Get the blur texture
		RenderBlurTexture(mpBloomBlurTexture,pScreenTexture,mfBloomSpread);

		//Size of blur texture
		cVector2f vBlurSize = cVector2f((float)mpBloomBlurTexture->GetWidth(),(float)mpBloomBlurTexture->GetHeight());


		//Size of the virtual screen
		cVector2f vVirtSize = mpLowLevelGraphics->GetVirtualSize();

		///////////////////////////////////////////
		//Draw Bloom

		//Setup program
		mpBloomP->Bind();
		mpBloomP->SetMatrixf("worldViewProj",eGpuProgramMatrix::ViewProjection,eGpuProgramMatrixOp::Identity);

		mpLowLevelGraphics->SetTexture(0,mpBloomBlurTexture);
		mpLowLevelGraphics->SetTexture(1,pScreenTexture);

		//Draw
		{
			tVector3fVec vUvVec; vUvVec.resize(4);

			vUvVec[0] = cVector2f(0,mvScreenSize.y);
			vUvVec[1] = cVector2f(mvScreenSize.x,mvScreenSize.y);
			vUvVec[2] = cVector2f(mvScreenSize.x,0);
			vUvVec[3] = cVector2f(0,0);


			mvTexRectVtx[0] = cVertex(cVector3f(0,0,40),cVector2f(0,1),cColor(1,1.0f) );
			mvTexRectVtx[1] = cVertex(cVector3f(vVirtSize.x,0,40),cVector2f(1,1),cColor(1,1.0f));
			mvTexRectVtx[2] = cVertex(cVector3f(vVirtSize.x,vVirtSize.y,40),cVector2f(1,0),cColor(1,1.0f));
			mvTexRectVtx[3] = cVertex(cVector3f(0,vVirtSize.y,40),cVector2f(0,0),cColor(1,1.0f));

			mpLowLevelGraphics->DrawQuadMultiTex(mvTexRectVtx, vUvVec);
		}

		mpBloomP->UnBind();

		mpLowLevelGraphics->SetTexture(0,NULL);
		mpLowLevelGraphics->SetTexture(1,NULL);
	}

	//-----------------------------------------------------------------------

	void cRendererPostEffects::RenderImageTrail()
	{
		if(mImageTrailData.mbActive==false) return;

		tVertexVec vVtx;
		vVtx.push_back( cVertex(cVector3f(0,0,40),cVector2f(0,mvScreenSize.y),cColor(1,0.6f) ) );
		vVtx.push_back( cVertex(cVector3f(800,0,40),cVector2f(mvScreenSize.x,mvScreenSize.y),cColor(1,0.6f)) );
		vVtx.push_back( cVertex(cVector3f(800,600,40),cVector2f(mvScreenSize.x,0),cColor(1,0.6f)) );
		vVtx.push_back( cVertex(cVector3f(0,600,40),cVector2f(0,0),cColor(1,0.6f)) );


		mpLowLevelGraphics->CopyContextToTexure(mpScreenBuffer[mImageTrailData.mlCurrentBuffer==0?1:0],0, cVector2l(
			(int)mpLowLevelGraphics->GetScreenSize().x,
			(int)mpLowLevelGraphics->GetScreenSize().y));
		{
			if(mImageTrailData.mbActive)
			{
				if(mImageTrailData.mbFirstPass){
					mpLowLevelGraphics->SetBlendActive(false);
					mpLowLevelGraphics->SetTexture(0,mpScreenBuffer[mImageTrailData.mlCurrentBuffer==0?1:0]);
				}
				else {

					mpLowLevelGraphics->SetClearDepthActive(false);
					mpLowLevelGraphics->SetClearColor(cColor(0,0));
					mpLowLevelGraphics->ClearScreen();
					mpLowLevelGraphics->SetClearDepthActive(true);

					//Draw the new image to screen transparently
					mpLowLevelGraphics->SetBlendActive(true);
					mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_Zero);

					mpLowLevelGraphics->SetTexture(0,mpScreenBuffer[mImageTrailData.mlCurrentBuffer==0?1:0]);

					mpLowLevelGraphics->SetTextureConstantColor(cColor(1,1.0f - mImageTrailData.mfAmount));
					mpLowLevelGraphics->SetTextureEnv(eTextureParam_AlphaSource0,eTextureSource_Constant);
					mpLowLevelGraphics->SetTextureEnv(eTextureParam_AlphaFunc,eTextureFunc_Replace);
				}
			}
			else
			{
				mpLowLevelGraphics->SetBlendActive(false);
				mpLowLevelGraphics->SetTexture(0,mpScreenBuffer[mImageTrailData.mlCurrentBuffer==0?1:0]);
			}

			mpLowLevelGraphics->DrawQuad(vVtx);


			if(mImageTrailData.mbActive)
			{
				if(!mImageTrailData.mbFirstPass)
				{
					//Draw the old Blurred image to screen
					mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha, eBlendFunc_One);

					mpLowLevelGraphics->SetTexture(0,mpScreenBuffer[mImageTrailData.mlCurrentBuffer]);

					mpLowLevelGraphics->SetTextureConstantColor(cColor(1,mImageTrailData.mfAmount));
					mpLowLevelGraphics->SetTextureEnv(eTextureParam_AlphaSource0,eTextureSource_Constant);
					mpLowLevelGraphics->SetTextureEnv(eTextureParam_AlphaFunc,eTextureFunc_Replace);

					mpLowLevelGraphics->DrawQuad(vVtx);
				}
				else
				{
					mImageTrailData.mbFirstPass = false;
				}
				mImageTrailData.mlCurrentBuffer = mImageTrailData.mlCurrentBuffer==0?1:0;


				//Copy screen to new blur buffer
				mpLowLevelGraphics->CopyContextToTexure(mpScreenBuffer[mImageTrailData.mlCurrentBuffer],0, cVector2l(
					(int)mpLowLevelGraphics->GetScreenSize().x,
					(int)mpLowLevelGraphics->GetScreenSize().y));

			}



			mpLowLevelGraphics->SetTextureEnv(eTextureParam_AlphaSource0,eTextureSource_Texture);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_AlphaSource1,eTextureSource_Previous);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_AlphaFunc,eTextureFunc_Modulate);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorSource0,eTextureSource_Texture);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorSource1,eTextureSource_Previous);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc,eTextureFunc_Modulate);

			mpLowLevelGraphics->SetBlendActive(false);
			mpLowLevelGraphics->SetTexture(0,NULL);
		}
	}

	//-----------------------------------------------------------------------

}
