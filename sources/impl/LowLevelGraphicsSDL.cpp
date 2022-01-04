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
#ifdef WIN32
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")
#pragma comment(lib, "SDL_ttf.lib")
#endif

#include "impl/LowLevelGraphicsSDL.h"

#include "graphics/FontData.h"
#include "impl/SDLBitmap2D.h"
#include "impl/SDLFontData.h"
#include "impl/SDLTexture.h"
#include "impl/GLSLShader.h"
#include "impl/GLSLProgram.h"
#include "system/LowLevelSystem.h"
#include "system/Platform.h"
#include "impl/VertexBufferOGL.h"
#include "impl/VertexBufferVBO.h"
#include "impl/VertexBufferGLES.h"
#include "impl/OcclusionQueryOGL.h"
#include "impl/GLHelpers.h"

#include <assert.h>
#include <stdlib.h>

namespace hpl
{

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelGraphicsSDL::cLowLevelGraphicsSDL() : cLowLevelGraphicsGL()
	{
		mlMultisampling = 0;

		#ifdef WIN32
			mhKeyTrapper = nullptr;
		#endif

		mpPixelFormat = hplNew(cSDLPixelFormat, () );

		mpRenderTarget = nullptr;

		//Init extra stuff

		TTF_Init();
	}

	//-----------------------------------------------------------------------

	cLowLevelGraphicsSDL::~cLowLevelGraphicsSDL()
	{
		//#ifdef WIN32
		//	if(mhKeyTrapper) FreeLibrary(mhKeyTrapper);
		//#endif

		SDL_SetGammaRamp(mvStartGammaArray[0],mvStartGammaArray[1],mvStartGammaArray[2]);

		hplDelete(mpPixelFormat);

		//Exit extra stuff
		TTF_Quit();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void CALLBACK OGLDebugOutputCallback(GLenum alSource, GLenum alType, GLuint alID, GLenum alSeverity, GLsizei alLength, const GLchar* apMessage, const GLvoid* apUserParam)
	{
		Log("Source: %d Type: %d Id: %d Severity: %d '%s'\n", alSource, alType, alID, alSeverity, apMessage);
	}

	bool cLowLevelGraphicsSDL::Init(int alWidth, int alHeight, int alBpp, int abFullscreen,
									int alMultisampling, const tString& asWindowCaption)
	{
		if (!cLowLevelGraphicsGL::Init(alWidth, alHeight, alBpp, abFullscreen, alMultisampling, asWindowCaption))
			return false;

		mlBpp = alBpp;
		mlMultisampling = alMultisampling;

		//Set some GL Attributes
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		// Multisampling
		if (mlMultisampling > 0)
		{
			if (SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1)==-1)
			{
				Error("Multisample buffers not supported!\n");
			}
			else
			{
				if (SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, mlMultisampling)==-1)
				{
					Error("Couldn't set multisampling samples to %d\n",mlMultisampling);
				}
			}
		}

		unsigned int mlFlags = SDL_OPENGL;

		if(abFullscreen) mlFlags |= SDL_FULLSCREEN;

		SetWindowCaption(asWindowCaption);

		Log(" Setting video mode: %d x %d - %d bpp\n",alWidth, alHeight, alBpp);
		mpScreen = SDL_SetVideoMode( alWidth, alHeight, alBpp, mlFlags);
		if(mpScreen==NULL){
			Error("Could not set display mode setting a lower one!\n");
			mvScreenSize = cVector2l(640,480);
			mpScreen = SDL_SetVideoMode( mvScreenSize.x, mvScreenSize.y, alBpp, mlFlags);
			if(mpScreen==NULL)
			{
				FatalError("Unable to initialize display!\n");
				return false;
			}
			else
			{
				//SetWindowCaption(asWindowCaption);
				cPlatform::CreateMessageBox(_W("Warning!"),
									_W("Could not set displaymode and 640x480 is used instead!\n"));
			}
		}
		else
		{
			//SetWindowCaption(asWindowCaption);
		}

		mvScreenSize = cVector2l(mpScreen->w, mpScreen->h);

		Log(" Init glad...");
	#if defined(WIN32)
		if (gladLoadWGL(wglGetCurrentDC()))
		{
			Log("wgl OK\n");
		}
		else
		{
			Log("ERROR!\n");
			Error(" Couldn't init glad wgl!\n");
		}

	#endif

		if(gladLoadGL())
		{
			Log("gl OK\n");
		}
		else
		{
			Log("ERROR!\n");
			Error(" Couldn't init glad!\n");
		}

		///Setup up windows specifc context:
		#if defined(WIN32)
			mGLContext = wglGetCurrentContext();
			mDeviceContext = wglGetCurrentDC();
		#elif defined(__linux__)
		/*gDpy = XOpenDisplay(NULL);
		glCtx = gPBuffer = 0;*/
		#endif

		//Check Multisample properties
		CheckMultisampleCaps();

		//Turn off cursor as default
		ShowCursor(false);

		//Gamma
		mfGammaCorrection = 1.0f;
		SDL_GetGammaRamp(mvStartGammaArray[0],mvStartGammaArray[1],mvStartGammaArray[2]);

		SDL_SetGamma(mfGammaCorrection,mfGammaCorrection,mfGammaCorrection);

		//GL
		Log(" Setting up OpenGL\n");
		SetupGL();

		//Set the clear color
		SDL_GL_SwapBuffers();

		if(GLAD_GL_KHR_debug)
		{
			glDebugMessageCallbackKHR(&OGLDebugOutputCallback, NULL);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
		}

		return true;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::CheckMultisampleCaps()
	{

	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetupGL()
	{
		cLowLevelGraphicsGL::SetupGL();

		//Inits GL stuff
		glShadeModel(GL_SMOOTH);
		glDisable(GL_ALPHA_TEST);

		//Set best perspective correction
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		//int lStencilBits=-1;
		//glGetIntegerv(GL_STENCIL_BITS,&lStencilBits);
		//Log(" Stencil bits: %d\n",lStencilBits);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

		//Show some info
		Log("  Max texture image units: %d\n",GetCaps(eGraphicCaps_MaxTextureImageUnits));
		Log("  Max texture coord units: %d\n",GetCaps(eGraphicCaps_MaxTextureCoordUnits));
		Log("  Two sided stencil: %d\n",GetCaps(eGraphicCaps_TwoSideStencil));
		Log("  Vertex Buffer Object: %d\n",GetCaps(eGraphicCaps_VertexBufferObject));

		Log("  Anisotropic filtering: %d\n",GetCaps(eGraphicCaps_AnisotropicFiltering));
		if(GetCaps(eGraphicCaps_AnisotropicFiltering))
			Log("  Max Anisotropic degree: %d\n",GetCaps(eGraphicCaps_MaxAnisotropicFiltering));

		Log("  Multisampling: %d\n",GetCaps(eGraphicCaps_Multisampling));

		Log("  Vertex Program: %d\n",GetCaps(eGraphicCaps_GL_VertexProgram));
		Log("  Fragment Program: %d\n",GetCaps(eGraphicCaps_GL_FragmentProgram));

		Log("  NV Register Combiners: %d\n",GetCaps(eGraphicCaps_GL_NVRegisterCombiners));
		Log("  NV Register Combiners Stages: %d\n",GetCaps(eGraphicCaps_GL_NVRegisterCombiners_MaxStages));

		Log("  ATI Fragment Shader: %d\n",GetCaps(eGraphicCaps_GL_ATIFragmentShader));
	}
	//-----------------------------------------------------------------------

	int cLowLevelGraphicsSDL::GetCaps(eGraphicCaps aType)
	{
		switch(aType)
		{
			//Texture Rectangle
			case eGraphicCaps_TextureTargetRectangle:
			{
				return 1;//GL_ARB_texture_rectangle?1:0;
			}

			//Vertex Buffer Object
			case eGraphicCaps_VertexBufferObject:
			{
				return 1;
			}

			//Two Sided Stencil
			case eGraphicCaps_TwoSideStencil:
			{
				return 1;
			}

			//Max Texture Image Units
			case eGraphicCaps_MaxTextureImageUnits:
			{
				int lUnits;
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,(GLint *)&lUnits);
				return lUnits;
			}

			//Max Texture Coord Units
			case eGraphicCaps_MaxTextureCoordUnits:
			{
				int lUnits;
				glGetIntegerv(GL_MAX_TEXTURE_COORDS,(GLint *)&lUnits);
				return lUnits;
			}

			//Texture Anisotropy
			case eGraphicCaps_AnisotropicFiltering:
			{
				return GLAD_GL_EXT_texture_filter_anisotropic;
			}

			//Texture Anisotropy
			case eGraphicCaps_MaxAnisotropicFiltering:
			{
				if(!GLAD_GL_EXT_texture_filter_anisotropic) return 0;

				float fMax;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&fMax);
				return (int)fMax;
			}

			//Multisampling
			case eGraphicCaps_Multisampling:
			{
				return 1;
			}

			//GL Vertex program
			case eGraphicCaps_GL_VertexProgram:
			{
				return GLAD_GL_ARB_vertex_program;
			}

			//GL Fragment program
			case eGraphicCaps_GL_FragmentProgram:
			{
				return GLAD_GL_ARB_fragment_program;
			}

			//GL NV register combiners
			case eGraphicCaps_GL_NVRegisterCombiners:
			{
				return GLAD_GL_NV_register_combiners;
			}

			//GL NV register combiners Max stages
			case eGraphicCaps_GL_NVRegisterCombiners_MaxStages:
			{
				int lStages;
				glGetIntegerv(GL_MAX_GENERAL_COMBINERS_NV,(GLint *)&lStages);
				return lStages;
			}

			//GL ATI Fragment Shader
			case eGraphicCaps_GL_ATIFragmentShader:
			{
				return GLAD_GL_ATI_fragment_shader;
			}
		}

		return 0;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::ShowCursor(bool abX)
	{
		if(abX)
			SDL_ShowCursor(SDL_ENABLE);
		else
			SDL_ShowCursor(SDL_DISABLE);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetVsyncActive(bool abX)
	{
		#if defined(WIN32)
		if(GLAD_WGL_EXT_swap_control)
		{
			wglSwapIntervalEXT(abX ? 1 : 0);
		}
		#elif defined(__linux__)
		if (GLAD_GLX_SGI_swap_control)
		{
			glXSwapIntervalSGI(abX ? 1 : 0);
		}
		#endif
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetMultisamplingActive(bool abX)
	{
		if(mlMultisampling<=0) return;

		if(abX)
			glEnable(GL_MULTISAMPLE);
		else
			glDisable(GL_MULTISAMPLE);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetGammaCorrection(float afX)
	{
		//if(mfGammaCorrection == afX) return;

		mfGammaCorrection = afX;

		SDL_SetGamma(mfGammaCorrection,mfGammaCorrection,mfGammaCorrection);

		/*Uint16 GammaArray[3][256];

		for (int iIndex = 0; iIndex < 256; iIndex++)
		{
			Uint16 iArrayValue = iIndex * ((int)(afX*127.0f) + 128);

			if (iArrayValue > 65535)
				iArrayValue = 65535;

			GammaArray[0][iIndex] =
			GammaArray[1][iIndex] =
			GammaArray[2][iIndex] = iArrayValue;

		}

		//Set the GammaArray values into the display device context.
		int bReturn = SDL_SetGammaRamp(GammaArray[0],GammaArray[1],GammaArray[2]);*/
		/*if(bReturn!=-1) Log("Setting gamma worked!\n");
		else		Log("Setting gamma FAILED!\n");*/
	}

	float cLowLevelGraphicsSDL::GetGammaCorrection()
	{
		return mfGammaCorrection;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetClipPlane(int alIdx, const cPlanef& aPlane)
	{
		mvClipPlanes[alIdx] = aPlane;

		double vPlane[4];
		vPlane[0] = aPlane.a;
		vPlane[1] = aPlane.b;
		vPlane[2] = aPlane.c;
		vPlane[3] = aPlane.d;
		glClipPlane(GL_CLIP_PLANE0 + alIdx,vPlane);
	}
	cPlanef cLowLevelGraphicsSDL::GetClipPlane(int alIdx, const cPlanef& aPlane)
	{
		return mvClipPlanes[alIdx];
	}
	void cLowLevelGraphicsSDL::SetClipPlaneActive(int alIdx, bool abX)
	{
		if(abX) glEnable(GL_CLIP_PLANE0 + alIdx);
		else	glDisable(GL_CLIP_PLANE0 + alIdx);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SaveScreenToBMP(const tString& asFile)
	{
		glFinish();

		cSDLBitmap2D *pBmp = hplNew( cSDLBitmap2D, (mpPixelFormat) );
		pBmp->Create(cVector2l(mvScreenSize.x,mvScreenSize.y),32);

		unsigned char *pDestPixels = (unsigned char*)pBmp->GetSurface()->pixels;
		unsigned char *pSrcPixels = (unsigned char*)hplMalloc(mvScreenSize.x * mvScreenSize.y * 4);

		SDL_LockSurface(pBmp->GetSurface());
		glReadBuffer(GL_BACK);
		glReadPixels(0,0,mvScreenSize.x,mvScreenSize.y,GL_RGBA,GL_UNSIGNED_BYTE,pSrcPixels);

		for(int y=0; y<mvScreenSize.y; ++y)
		{
			for(int x=0; x<mvScreenSize.x; ++x)
			{
				unsigned char* pDestPix = &pDestPixels[((mvScreenSize.x * y) + x) * 4];
				unsigned char* pSrcPix = &pSrcPixels[((mvScreenSize.x * ((mvScreenSize.y-1) - y))
														+ x) * 4];

				pDestPix[0] = pSrcPix[0];
				pDestPix[1] = pSrcPix[1];
				pDestPix[2] = pSrcPix[2];
				pDestPix[3] = 255;
			}
		}

		SDL_UnlockSurface(pBmp->GetSurface());
		SDL_SaveBMP(pBmp->GetSurface(),asFile.c_str());

		hplFree(pSrcPixels);
		hplDelete(pBmp);

	}

	//-----------------------------------------------------------------------

	iBitmap2D* cLowLevelGraphicsSDL::CreateBitmap2D(const cVector2l &avSize, unsigned int alBpp)
	{
		cSDLBitmap2D *pBmp = hplNew( cSDLBitmap2D, (mpPixelFormat) );
		pBmp->Create(avSize,alBpp);

		return pBmp;
	}

	iBitmap2D *cLowLevelGraphicsSDL::CreateBitmap2DFromSurface(SDL_Surface *apSurface, const tString &asType)
	{
		cSDLBitmap2D *pBmp = hplNew(cSDLBitmap2D, (apSurface, mpPixelFormat, asType));

		pBmp->msType = asType;

		return pBmp;
	}

	//-----------------------------------------------------------------------

	iFontData* cLowLevelGraphicsSDL::CreateFontData(const tString &asName)
	{
		return hplNew( cSDLFontData, (asName, this) );
	}

	//-----------------------------------------------------------------------

	iTexture *cLowLevelGraphicsSDL::CreateTexture(bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)
	{
		return hplNew(cSDLTexture, ("", mpPixelFormat, this, aType, abUseMipMaps, aTarget));
	}

	iTexture *cLowLevelGraphicsSDL::CreateTexture(const tString &asName, bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)
	{
		return hplNew(cSDLTexture, (asName, mpPixelFormat, this, aType, abUseMipMaps, aTarget));
	}

	iTexture *cLowLevelGraphicsSDL::CreateTexture(iBitmap2D *apBmp, bool abUseMipMaps, eTextureType aType,
		eTextureTarget aTarget)
	{
		cSDLTexture *pTex = hplNew(cSDLTexture, ("", mpPixelFormat, this, aType, abUseMipMaps, aTarget));
		pTex->CreateFromBitmap(apBmp);

		return pTex;
	}

	iTexture *cLowLevelGraphicsSDL::CreateTexture(const cVector2l &avSize, int alBpp, cColor aFillCol,
		bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget)
	{
		cSDLTexture *pTex = NULL;

		if (aType == eTextureType_RenderTarget)
		{
			pTex = hplNew(cSDLTexture, ("", mpPixelFormat, this, aType, abUseMipMaps, aTarget));
			pTex->Create(avSize.x, avSize.y, aFillCol);
		}
		else
		{
			iBitmap2D *pBmp = CreateBitmap2D(avSize, alBpp);
			pBmp->FillRect(cRect2l(0, 0, 0, 0), aFillCol);

			pTex = hplNew(cSDLTexture, ("", mpPixelFormat, this, aType, abUseMipMaps, aTarget));
			bool bRet = pTex->CreateFromBitmap(pBmp);

			hplDelete(pBmp);

			if (bRet == false) {
				hplDelete(pTex);
				return NULL;
			}
		}
		return pTex;
	}

	//-----------------------------------------------------------------------

	iGpuProgram* cLowLevelGraphicsSDL::CreateGpuProgram(const tString& asName)
	{
		return hplNew( cGLSLProgram, (asName, this) );
	}

	iGpuShader *cLowLevelGraphicsSDL::CreateGpuShader(const tString &asName, eGpuShaderType aType)
	{
		return hplNew(cGLSLShader, (asName, aType, this));
	}

	//-----------------------------------------------------------------------

	iOcclusionQuery *cLowLevelGraphicsSDL::CreateOcclusionQuery()
	{
		return hplNew(cOcclusionQueryOGL, ());
	}

	void cLowLevelGraphicsSDL::DestroyOcclusionQuery(iOcclusionQuery *apQuery)
	{
		hplDelete(apQuery);
	}

	//-----------------------------------------------------------------------

	iVertexBuffer *cLowLevelGraphicsSDL::CreateVertexBuffer(tVertexFlag aFlags,
		eVertexBufferDrawType aDrawType,
		eVertexBufferUsageType aUsageType,
		int alReserveVtxSize, int alReserveIdxSize)
	{

		return hplNew(cVertexBufferGLES, (this, aFlags, aDrawType, aUsageType, alReserveVtxSize, alReserveIdxSize));
		//return hplNew( cVertexBufferVBO,(this, aFlags,aDrawType,aUsageType,alReserveVtxSize,alReserveIdxSize) );
		//return hplNew( cVertexBufferOGL, (this, aFlags,aDrawType,aUsageType,alReserveVtxSize,alReserveIdxSize) );

		if (GetCaps(eGraphicCaps_VertexBufferObject))
		{
			return hplNew(cVertexBufferVBO, (this, aFlags, aDrawType, aUsageType, alReserveVtxSize, alReserveIdxSize));
		}
		else
		{
			//Error("VBO is not supported, using Vertex array!\n");
			return hplNew(cVertexBufferOGL, (this, aFlags, aDrawType, aUsageType, alReserveVtxSize, alReserveIdxSize));
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::PushMatrix(eMatrix aMtxType)
	{
		SetMatrixMode(aMtxType);
		glPushMatrix();
	}

	void cLowLevelGraphicsSDL::PopMatrix(eMatrix aMtxType)
	{
		SetMatrixMode(aMtxType);
		glPopMatrix();
	}

	void cLowLevelGraphicsSDL::SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA)
	{
		SetMatrixMode(aMtxType);
		cMatrixf mtxTranpose = a_mtxA.GetTranspose();
		glLoadMatrixf(mtxTranpose.v);

		cLowLevelGraphicsGL::SetMatrix(aMtxType, a_mtxA);
	}

	cMatrixf cLowLevelGraphicsSDL::GetMatrix(eMatrix aMtxType)
	{
		cMatrixf result;
		switch (aMtxType)
		{
			case eMatrix::ModelView: glGetFloatv(GL_TRANSPOSE_MODELVIEW_MATRIX, result.v); break;
			case eMatrix::Projection: glGetFloatv(GL_TRANSPOSE_PROJECTION_MATRIX, result.v); break;
			case eMatrix::Texture: glGetFloatv(GL_TRANSPOSE_TEXTURE_MATRIX, result.v); break;
		}

		return result;
	}

	void cLowLevelGraphicsSDL::SetIdentityMatrix(eMatrix aMtxType)
	{
		SetMatrixMode(aMtxType);
		glLoadIdentity();

		cLowLevelGraphicsGL::SetIdentityMatrix(aMtxType);
	}

	void cLowLevelGraphicsSDL::TranslateMatrix(eMatrix aMtxType, const cVector3f &avPos)
	{
		SetMatrixMode(aMtxType);
		glTranslatef(avPos.x,avPos.y,avPos.z);
	}

	/**
	 * \todo fix so that there are X, Y , Z versions of this one.
	 * \param aMtxType
	 * \param &avRot
	 */
	void cLowLevelGraphicsSDL::RotateMatrix(eMatrix aMtxType, const cVector3f &avRot)
	{
		SetMatrixMode(aMtxType);
		glRotatef(1,avRot.x,avRot.y,avRot.z);
	}

	void cLowLevelGraphicsSDL::ScaleMatrix(eMatrix aMtxType, const cVector3f &avScale)
	{
		SetMatrixMode(aMtxType);
		glScalef(avScale.x,avScale.y,avScale.z);
	}

	void cLowLevelGraphicsSDL::SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,avSize.x,avSize.y,0,afMin,afMax);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetClearDepth(float afDepth)
	{
		glClearDepth(afDepth);
	}

	void cLowLevelGraphicsSDL::SetAlphaTestActive(bool abX)
	{
		if (abX) glEnable(GL_ALPHA_TEST);
		else glDisable(GL_ALPHA_TEST);
	}

	void cLowLevelGraphicsSDL::SetAlphaTestFunc(eAlphaTestFunc aFunc, float afRef)
	{
		glAlphaFunc(GetGLAlphaTestFuncEnum(aFunc), afRef);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::UnbindRenderTargetTextureGL(iTexture *apTex, int aLastTarget)
	{
		cSDLTexture *pSDLTex = static_cast<cSDLTexture *>(apTex);

		glBindTexture(aLastTarget, pSDLTex->GetTextureHandle());
		cPBuffer *pBuffer = pSDLTex->GetPBuffer();
		pBuffer->UnBind();
	}

	void cLowLevelGraphicsSDL::BindTextureGL(iTexture *apTex, int aNewTarget)
	{
		cSDLTexture *pSDLTex = static_cast<cSDLTexture *> (apTex);

		glBindTexture(aNewTarget, pSDLTex->GetTextureHandle());
		glEnable(aNewTarget);

		//if it is a render target we need to do some more binding.
		if (pSDLTex->GetTextureType() == eTextureType_RenderTarget)
		{
			cPBuffer *pBuffer = pSDLTex->GetPBuffer();
			pBuffer->Bind();
		}
	}

	void cLowLevelGraphicsSDL::SetTextureEnv(eTextureParam aParam, int alVal)
	{
		GLenum lParam = GetGLTextureParamEnum(aParam);

		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

		if(aParam==eTextureParam_ColorFunc || aParam==eTextureParam_AlphaFunc){
			glTexEnvi(GL_TEXTURE_ENV,lParam,GetGLTextureFuncEnum((eTextureFunc)alVal));
		}
		else if(aParam>=eTextureParam_ColorSource0 && aParam<=eTextureParam_AlphaSource2){
			glTexEnvi(GL_TEXTURE_ENV,lParam,GetGLTextureSourceEnum((eTextureSource)alVal));
		}
		else if(aParam>=eTextureParam_ColorOp0 && aParam<=eTextureParam_AlphaOp2){
			glTexEnvi(GL_TEXTURE_ENV,lParam,GetGLTextureOpEnum((eTextureOp)alVal));
		}
		else {
			glTexEnvi(GL_TEXTURE_ENV,lParam,alVal);
		}
	}

	void cLowLevelGraphicsSDL::SetTextureConstantColor(const cColor &aColor)
	{
		float vColor[4] = {	aColor.r, aColor.g, aColor.b, aColor.a	};

		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &vColor[0]);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetColor(const cColor &aColor)
	{
		glColor4f(aColor.r, aColor.g, aColor.b, aColor.a);
		cLowLevelGraphicsGL::SetColor(aColor);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawRect(const cVector2f &avPos, const cVector2f &avSize, float afZ)
	{
		glColor4f(1, 1, 1, 1);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0, 0.0); glVertex3f(avPos.x, avPos.y, afZ);
			glTexCoord2f(1.0, 0.0); glVertex3f(avPos.x + avSize.x, avPos.y, afZ);
			glTexCoord2f(1.0, 1.0); glVertex3f(avPos.x + avSize.x, avPos.y + avSize.y, afZ);
			glTexCoord2f(0.0, 1.0); glVertex3f(avPos.x, avPos.y + avSize.y, afZ);
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawTri(const tVertexVec& avVtx)
	{
		assert(avVtx.size() == 3);

		glBegin(GL_TRIANGLES);
		{
			for (int i = 0; i < 3; i++) {
				glTexCoord3f(avVtx[i].tex.x, avVtx[i].tex.y, avVtx[i].tex.z);
				glColor4f(avVtx[i].col.r, avVtx[i].col.g, avVtx[i].col.b, avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x, avVtx[i].pos.y, avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawTri(const cVertex* avVtx)
	{
		glBegin(GL_TRIANGLES);
		{
			for (int i = 0; i < 3; i++) {
				glTexCoord3f(avVtx[i].tex.x, avVtx[i].tex.y, avVtx[i].tex.z);
				glColor4f(avVtx[i].col.r, avVtx[i].col.g, avVtx[i].col.b, avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x, avVtx[i].pos.y, avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx, const cColor aCol)
	{
		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			//Make all this inline??
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,avVtx[i].tex.z);
				glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx,const float afZ)
	{
		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			//Make all this inline??
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,afZ);
				glColor4f(avVtx[i].col.r,avVtx[i].col.g,avVtx[i].col.b,avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawQuad(const tVertexVec &avVtx,const float afZ,const cColor &aCol)
	{
		assert(avVtx.size()==4);

		glBegin(GL_QUADS);
		{
			//Make all this inline??
			for(int i=0;i<4;i++){
				glTexCoord3f(avVtx[i].tex.x,avVtx[i].tex.y,afZ);
				glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
				glVertex3f(avVtx[i].pos.x,avVtx[i].pos.y,avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	void cLowLevelGraphicsSDL::DrawQuadMultiTex(const tVertexVec &avVtx, const tVector3fVec &avExtraUvs)
	{
		int lExtraUnits = (int)avExtraUvs.size() / 4;
		glBegin(GL_QUADS);
		{
			for (int i = 0; i < 4; i++)
			{
				glMultiTexCoord3f(GL_TEXTURE0, avVtx[i].tex.x, avVtx[i].tex.y, avVtx[i].tex.z);

				for (int unit = 0; unit < lExtraUnits; ++unit)
				{
					glMultiTexCoord3f(GL_TEXTURE0 + unit + 1,
						avExtraUvs[unit * 4 + i].x, avExtraUvs[unit * 4 + i].y, avExtraUvs[unit * 4 + i].z);
				}

				glColor4f(avVtx[i].col.r, avVtx[i].col.g, avVtx[i].col.b, avVtx[i].col.a);
				glVertex3f(avVtx[i].pos.x, avVtx[i].pos.y, avVtx[i].pos.z);
			}
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::AddTexCoordToBatch(unsigned int alUnit, const cVector3f *apCoord)
	{
		unsigned int lCount = mlTexCoordArrayCount[alUnit];

		mpTexCoordArray[alUnit][lCount + 0] = apCoord->x;
		mpTexCoordArray[alUnit][lCount + 1] = apCoord->y;
		mpTexCoordArray[alUnit][lCount + 2] = apCoord->z;

		mlTexCoordArrayCount[alUnit] += 3;
	}

	void cLowLevelGraphicsSDL::SetBatchTextureUnitActive(unsigned int alUnit, bool abActive)
	{
		glClientActiveTexture(GL_TEXTURE0 + alUnit);

		if (abActive == false) {
			glTexCoordPointer(3, GL_FLOAT, sizeof(float) * mlBatchStride, &mpVertexArray[7]);
		}
		else {
			glTexCoordPointer(3, GL_FLOAT, 0, &mpTexCoordArray[alUnit][0]);
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawBoxMaxMin(const cVector3f& avMax, const cVector3f& avMin, cColor aCol)
	{
		SetTexture(0,NULL);
		SetBlendActive(false);
		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);

		glBegin(GL_LINES);
		{
			//Pos Z Quad
			glVertex3f(avMax.x,avMax.y,avMax.z);
			glVertex3f(avMin.x,avMax.y,avMax.z);

			glVertex3f(avMax.x,avMax.y,avMax.z);
			glVertex3f(avMax.x,avMin.y,avMax.z);

			glVertex3f(avMin.x,avMax.y,avMax.z);
			glVertex3f(avMin.x,avMin.y,avMax.z);

			glVertex3f(avMin.x,avMin.y,avMax.z);
			glVertex3f(avMax.x,avMin.y,avMax.z);

			//Neg Z Quad
			glVertex3f(avMax.x,avMax.y,avMin.z);
			glVertex3f(avMin.x,avMax.y,avMin.z);

			glVertex3f(avMax.x,avMax.y,avMin.z);
			glVertex3f(avMax.x,avMin.y,avMin.z);

			glVertex3f(avMin.x,avMax.y,avMin.z);
			glVertex3f(avMin.x,avMin.y,avMin.z);

			glVertex3f(avMin.x,avMin.y,avMin.z);
			glVertex3f(avMax.x,avMin.y,avMin.z);

			//Lines between
			glVertex3f(avMax.x,avMax.y,avMax.z);
			glVertex3f(avMax.x,avMax.y,avMin.z);

			glVertex3f(avMin.x,avMax.y,avMax.z);
			glVertex3f(avMin.x,avMax.y,avMin.z);

			glVertex3f(avMin.x,avMin.y,avMax.z);
			glVertex3f(avMin.x,avMin.y,avMin.z);

			glVertex3f(avMax.x,avMin.y,avMax.z);
			glVertex3f(avMax.x,avMin.y,avMin.z);
		}
		glEnd();

	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawLine2D(const cVector2f& avBegin, const cVector2f& avEnd, float afZ, cColor aCol)
	{
		SetTexture(0,NULL);
		SetBlendActive(false);
		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINES);
		{
			glVertex3f(avBegin.x,avBegin.y,afZ);
			glVertex3f(avEnd.x,avEnd.y,afZ);
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawLineRect2D(const cRect2f& aRect, float afZ, cColor aCol)
	{
		SetTexture(0, NULL);
		SetBlendActive(false);
		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_LINE_STRIP);
		{
			glVertex3f(aRect.x,aRect.y,afZ);
			glVertex3f(aRect.x+aRect.w,aRect.y,afZ);
			glVertex3f(aRect.x+aRect.w,aRect.y+aRect.h,afZ);
			glVertex3f(aRect.x,aRect.y+aRect.h,afZ);
			glVertex3f(aRect.x,aRect.y,afZ);
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::DrawFilledRect2D(const cRect2f& aRect, float afZ, cColor aCol)
	{
		SetTexture(0, NULL);
		glColor4f(aCol.r,aCol.g,aCol.b,aCol.a);
		glBegin(GL_QUADS);
		{
			glVertex3f(aRect.x,aRect.y,afZ);
			glVertex3f(aRect.x+aRect.w,aRect.y,afZ);
			glVertex3f(aRect.x+aRect.w,aRect.y+aRect.h,afZ);
			glVertex3f(aRect.x,aRect.y+aRect.h,afZ);
		}
		glEnd();
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetRenderTarget(iTexture* pTex)
	{
		if(pTex == mpRenderTarget)return;
		mpRenderTarget = pTex;

		if(pTex==NULL)
		{
			#ifdef WIN32
			if (!wglMakeCurrent(mDeviceContext, mGLContext)){
				Log("Something went wrong...");
			}
			#elif defined(__linux__)
			/*if (!glXMakeCurrent(dpy, gPBuffer, glCtx)) {
				Log("Something went wrong...");
			}*/
			#endif
		}
		else
		{
			if(pTex->GetTextureType() != eTextureType_RenderTarget)return;

			cSDLTexture* pSDLTex = static_cast<cSDLTexture*>(pTex);
			cPBuffer* pPBuffer = pSDLTex->GetPBuffer();

			//pPBuffer->UnBind();//needed?

			if (!pPBuffer->MakeCurrentContext()){
				Log("PBuffer::Activate() failed.\n");
			}
		}


		//Old OGL 1.1 Code:
		/*FlushRenderTarget();

		mpRenderTarget = pTex;

		if(mpRenderTarget==NULL)
			glViewport(0,0,mvScreenSize.x,mvScreenSize.y);
		else
			glViewport(0,0,pTex->GetWidth(),pTex->GetHeight());*/
	}

	//-----------------------------------------------------------------------

	bool cLowLevelGraphicsSDL::RenderTargetHasZBuffer()
	{
		return true;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::FlushRenderTarget()
	{
		//Old OGL 1.1 Code:
		/*if(mpRenderTarget!=NULL)
		{
			SetTexture(0, mpRenderTarget);

			//Log("w: %d\n",mpRenderTarget->GetWidth());

			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0,
							mpRenderTarget->GetWidth(), mpRenderTarget->GetHeight(), 0);
		}*/
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::FlushRendering()
	{
		glFlush();
	}

	void cLowLevelGraphicsSDL::SwapBuffers()
	{
		glFlush();
		SDL_GL_SwapBuffers();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsSDL::SetMatrixMode(eMatrix mType)
	{
		switch(mType)
		{
			case eMatrix::ModelView: glMatrixMode(GL_MODELVIEW);break;
			case eMatrix::Projection: glMatrixMode(GL_PROJECTION); break;
			case eMatrix::Texture: glMatrixMode(GL_TEXTURE); break;
		}
	}

	//-----------------------------------------------------------------------

}
