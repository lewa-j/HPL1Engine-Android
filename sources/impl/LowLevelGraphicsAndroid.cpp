/*
 * Copyright (C) 2020 - lewa_j
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
#include "impl/LowLevelGraphicsAndroid.h"
#include "impl/AndroidBitmap2D.h"
#include "impl/AndroidFontData.h"
#include "impl/AndroidTexture.h"
#include "impl/GLSLProgram.h"
#include "impl/VertexBufferGLES.h"

namespace hpl
{
	cLowLevelGraphicsAndroid::cLowLevelGraphicsAndroid()
	{
		mlBatchArraySize = 20000;
		mlVertexCount = 0;
		mlIndexCount =0;
		mlMultisampling =0;

		mvVirtualSize.x = 800;
		mvVirtualSize.y = 600;

		mpRenderTarget=NULL;

		for(int i=0;i<MAX_TEXTUREUNITS;i++)
			mpCurrentTexture[i] = NULL;

		mbClearColor = true;
		mbClearDepth = true;
		mbClearStencil = false;

		//Create the batch arrays:
		mlBatchStride = 13;
		//3 Pos floats, 4 color floats, 3 Tex coord floats .
		mpVertexArray = (float*)hplMalloc(sizeof(float) * mlBatchStride * mlBatchArraySize);
		mpIndexArray = (unsigned int*)hplMalloc(sizeof(unsigned int) * mlBatchArraySize); //Index is one int.

		for(int i=0;i<MAX_TEXTUREUNITS;i++)
		{
			mpTexCoordArray[i] = (float*)hplMalloc(sizeof(float) * 3 * mlBatchArraySize);
			mbTexCoordArrayActive[i] = false;
			mlTexCoordArrayCount[i]=0;
		}

		//Init extra stuff
		//InitCG();

		//TTF_Init();
	}
	
	cLowLevelGraphicsAndroid::~cLowLevelGraphicsAndroid()
	{

		hplFree(mpVertexArray);
		hplFree(mpIndexArray);
		for(int i=0;i<MAX_TEXTUREUNITS;i++)
			hplFree(mpTexCoordArray[i]);

		//Exit extra stuff
		//ExitCG();
		//TTF_Quit();
	}
	
	bool cLowLevelGraphicsAndroid::Init(int alWidth, int alHeight, int alBpp, int abFullscreen,
									int alMultisampling, const tString& asWindowCaption)
	{
		mvScreenSize.x = alWidth;
		mvScreenSize.y = alHeight;
		mlBpp = alBpp;

		mlMultisampling = alMultisampling;

		//Set some GL Attributes
		

		// Multisampling
		if(mlMultisampling > 0)
		{

		}

		Log(" Setting video mode: %d x %d - %d bpp\n",alWidth, alHeight, alBpp);

		/*Log(" Init Glee...");
		if(GLeeInit())
		{
			Log("OK\n");
		}
		else
		{
			Log("ERROR!\n");
			Error(" Couldn't init glee!\n");
		}*/

		///Setup up windows specifc context:

		//Check Multisample properties
		//CheckMultisampleCaps();

		//Turn off cursor as default
		ShowCursor(false);

		//GL
		Log(" Setting up OpenGL\n");
		//SetupGL();

		//Set the clear color
		//GL_SwapBuffers();

		return true;
	}
	
	
	cVector2f cLowLevelGraphicsAndroid::GetScreenSize()
	{
		return cVector2f((float)mvScreenSize.x, (float)mvScreenSize.y);
	}

	//-----------------------------------------------------------------------

	cVector2f cLowLevelGraphicsAndroid::GetVirtualSize()
	{
		return mvVirtualSize;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetVirtualSize(cVector2f avSize)
	{
		mvVirtualSize = avSize;
	}

	iBitmap2D* cLowLevelGraphicsAndroid::CreateBitmap2D(const cVector2l &avSize, unsigned int alBpp)
	{
		cAndroidBitmap2D *pBmp = hplNew( cAndroidBitmap2D, (nullptr) );
		pBmp->Create(avSize,alBpp);
		return pBmp;
	}
	
	iBitmap2D* cLowLevelGraphicsAndroid::CreateBitmap2DFromData(uint8_t *data,int w, int h, int n,const tString& asType)
	{
		cAndroidBitmap2D *pBmp = hplNew( cAndroidBitmap2D, (data,w,h,n,asType) );
		pBmp->msType = asType;
		return pBmp;
	}
	
	iFontData* cLowLevelGraphicsAndroid::CreateFontData(const tString &asName)
	{
		return hplNew( cAndroidFontData, (asName, this) );
	}
	
	iTexture* cLowLevelGraphicsAndroid::CreateTexture(bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)
	{
		return hplNew( cAndroidTexture, ("",/*mpPixelFormat*/nullptr,this,aType, abUseMipMaps, aTarget) );
	}
	
	iTexture* cLowLevelGraphicsAndroid::CreateTexture(const tString &asName,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)
	{
		return hplNew( cAndroidTexture, (asName,/*mpPixelFormat*/nullptr,this,aType, abUseMipMaps, aTarget) );
	}
	
	iTexture* cLowLevelGraphicsAndroid::CreateTexture(iBitmap2D* apBmp,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)
	{
		cAndroidTexture *pTex = hplNew( cAndroidTexture, ("",/*mpPixelFormat*/nullptr,this,aType, abUseMipMaps, aTarget) );
		pTex->CreateFromBitmap(apBmp);
		return pTex;
	}
	
	iTexture* cLowLevelGraphicsAndroid::CreateTexture(const cVector2l& avSize,int alBpp,cColor aFillCol,
							bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)
	{
		cAndroidTexture *pTex=NULL;

		if(aType==eTextureType_RenderTarget)
		{
			pTex = hplNew( cAndroidTexture, ("",/*mpPixelFormat*/nullptr,this,aType, abUseMipMaps, aTarget) );
			pTex->Create(avSize.x, avSize.y, aFillCol);
		}
		else
		{
			iBitmap2D* pBmp = CreateBitmap2D(avSize,alBpp);
			pBmp->FillRect(cRect2l(0,0,0,0),aFillCol);

			pTex = hplNew( cAndroidTexture, ("",/*mpPixelFormat*/nullptr,this,aType, abUseMipMaps, aTarget) );
			bool bRet = pTex->CreateFromBitmap(pBmp);
			
			hplDelete(pBmp);

			if(bRet==false){
				hplDelete(pTex);
				return NULL;
			}
		}
		return pTex;
	}

	iGpuProgram* cLowLevelGraphicsAndroid::CreateGpuProgram(const tString& asName, eGpuProgramType aType)
	{
		return hplNew( cGLSLProgram, (asName, aType) );
	}
	
	iVertexBuffer* cLowLevelGraphicsAndroid::CreateVertexBuffer(tVertexFlag aFlags, eVertexBufferDrawType aDrawType,
										eVertexBufferUsageType aUsageType,
										int alReserveVtxSize,int alReserveIdxSize)
	{
		return hplNew( cVertexBufferGLES, (this, aFlags,aDrawType,aUsageType,alReserveVtxSize,alReserveIdxSize) );
	}

}

