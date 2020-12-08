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
#include "impl/GLHelpers.h"
#include "math/Math.h"

namespace hpl
{

	cLowLevelGraphicsAndroid::cLowLevelGraphicsAndroid(ANativeWindow *apAWindow)
	{
		mpAWindow = apAWindow;
		
		mlBatchArraySize = 20000;
		mlVertexCount = 0;
		mlIndexCount = 0;
		mlMultisampling = 0;

		mvVirtualSize.x = 800;
		mvVirtualSize.y = 600;

		mpRenderTarget=NULL;

		for(int i=0;i<MAX_TEXTUREUNITS;i++)
			mpCurrentTexture[i] = NULL;

		for(int i=0;i<eMatrix_LastEnum;i++)
			mMatrixStack[i].push(cMatrixf::Identity);
		
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
	}
	
	static const bool egl_log = true;
	
	cLowLevelGraphicsAndroid::~cLowLevelGraphicsAndroid()
	{

		hplFree(mpVertexArray);
		hplFree(mpIndexArray);
		for(int i=0;i<MAX_TEXTUREUNITS;i++)
			hplFree(mpTexCoordArray[i]);

		//Exit extra stuff
		//ExitCG();
		//TTF_Quit();
		
		bool r = eglDestroySurface(mEglDisplay, mEglSurface);
		if(egl_log) Log("egl destroy surface %d\n",r);
		r = eglDestroyContext(mEglDisplay, mEglContext);
		if(egl_log) Log("egl destroy context %d\n",r);
		r = eglTerminate(mEglDisplay);
		if(egl_log) Log("egl terminate %d\n",r);
	}
	
	bool cLowLevelGraphicsAndroid::Init(int alWidth, int alHeight, int alBpp, int abFullscreen,
									int alMultisampling, const tString& asWindowCaption)
	{
		mvScreenSize.x = alWidth;
		mvScreenSize.y = alHeight;
		mlBpp = alBpp;
		mlMultisampling = alMultisampling;

		mSwapInterval = 1;
		
		mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		if(egl_log) Log("egl display %p\n",mEglDisplay);
	
		int ver[2];
		bool r = eglInitialize(mEglDisplay, ver, ver+1);
		Log("egl init %d, v %d.%d\n",r,ver[0],ver[1]);

		if(egl_log) Log("egl vendor %s, APIs %s\n",
			eglQueryString(mEglDisplay, EGL_VENDOR),
			eglQueryString(mEglDisplay, EGL_CLIENT_APIS));
	
		int cfgAttribs[]={
		EGL_SAMPLE_BUFFERS, 0,
		EGL_SAMPLES, 0,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_STENCIL_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
		};
		
		// Multisampling
		if(mlMultisampling > 0)
		{
			cfgAttribs[1] = 1;//EGL_SAMPLE_BUFFERS
			cfgAttribs[3] = mlMultisampling;//EGL_SAMPLES
		}

		int cfgCount=0;
		r = eglChooseConfig(mEglDisplay, cfgAttribs, &mEglConfig, 1, &cfgCount);
		if(egl_log) Log("egl choose config %d, cfg %p, count %d\n",r,mEglConfig,cfgCount);
	
		int ctxFlags = 0;
		//ctxFlags |= EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR;
	
		int contextAttribs[]={
			EGL_CONTEXT_CLIENT_VERSION, 2,
			//EGL_CONTEXT_FLAGS_KHR, ctxFlags,
			EGL_NONE
		};
		mEglContext = eglCreateContext(mEglDisplay, mEglConfig, EGL_NO_CONTEXT, contextAttribs);
		if(egl_log) Log("egl create context %p\n",mEglContext);
	
		int surfAttribs[]={
			//EGL_RENDER_BUFFER, EGL_SINGLE_BUFFER,
			EGL_NONE
		};

		//
		int fmt;
		eglGetConfigAttrib(mEglDisplay, mEglConfig, EGL_NATIVE_VISUAL_ID , &fmt);
		ANativeWindow_setBuffersGeometry(mpAWindow,0,0,fmt);
		//

		mEglSurface = eglCreateWindowSurface(mEglDisplay, mEglConfig, mpAWindow, surfAttribs);
		if(egl_log) Log("egl create window surface %p\n",mEglSurface);
	
		r = eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext);
		if(egl_log) Log("egl make current %d\n",r);
	
		r = eglSwapInterval(mEglDisplay, mSwapInterval);
		if(egl_log) Log("egl swap interval %d\n", r);
	
		eglQuerySurface(mEglDisplay, mEglSurface, EGL_WIDTH, &mvScreenSize.x);
		eglQuerySurface(mEglDisplay, mEglSurface, EGL_HEIGHT, &mvScreenSize.y);
		eglGetConfigAttrib(mEglDisplay, mEglConfig, EGL_BUFFER_SIZE , &mlBpp);
		Log(" Setting video mode: %d x %d - %d bpp\n", mvScreenSize.x, mvScreenSize.y, mlBpp);

		mvVirtualSize.y = mvVirtualSize.x/(mvScreenSize.x/(float)mvScreenSize.y);
		
		Log(" Init glad...");
		if(gladLoadGLES2Loader((GLADloadproc)eglGetProcAddress))
		{
			Log("OK\n");
		}
		else
		{
			Log("ERROR!\n");
			Error(" Couldn't init glad!\n");
		}

		//Turn off cursor as default
		ShowCursor(false);

		//GL
		Log(" Setting up OpenGL\n");
		SetupGL();

		//Set the clear color
		eglSwapBuffers(mEglDisplay,mEglSurface);

		return true;
	}
	
	void cLowLevelGraphicsAndroid::SetupGL()
	{
		//Inits GL stuff
		
		//Set clear color.
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		//Depth Test setup
		glClearDepthf(1.0f);//Values buffer is cleared with
		glEnable(GL_DEPTH_TEST); //enable depth testing
		glDepthFunc(GL_LEQUAL); //function to do depth test with
		//glDisable(GL_ALPHA_TEST);

		//Set best perspective correction
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		//Stencil setup
		glClearStencil(0);

		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		/////  BEGIN BATCH ARRAY STUFF ///////////////
/*
		//Enable all the vertex arrays that are used:
		glEnableClientState(GL_VERTEX_ARRAY ); //The positions
		glEnableClientState(GL_COLOR_ARRAY ); //The color
		glEnableClientState(GL_TEXTURE_COORD_ARRAY); //Tex coords
		glDisableClientState(GL_NORMAL_ARRAY);
		//Disable the once not used.
		glDisableClientState(GL_INDEX_ARRAY); //color index
		glDisableClientState(GL_EDGE_FLAG_ARRAY);
*/
		///// END BATCH ARRAY STUFF ///////////////

		static const char* storage = getenv("EXTERNAL_STORAGE");
		tString shadersPath = storage + tString("/hpl1/core/shaders/");

		mSimpleShader = CreateGpuProgram("Simple", eGpuProgramType_Linked );
		mSimpleShader->CreateFromFiles(shadersPath+"Simple.vs", shadersPath+"Simple.fs");

		mSimpleShader->Bind();

		mDefaultTexture = CreateTexture({1,1},32,cColor(1,1),false,eTextureType_Normal,eTextureTarget_2D);

		//Show some info
		Log("  Max texture image units: %d\n",GetCaps(eGraphicCaps_MaxTextureImageUnits));
		Log("  Max texture coord units: %d\n",GetCaps(eGraphicCaps_MaxTextureCoordUnits));

		Log("  Anisotropic filtering: %d\n",GetCaps(eGraphicCaps_AnisotropicFiltering));
		if(GetCaps(eGraphicCaps_AnisotropicFiltering))
			Log("  Max Anisotropic degree: %d\n",GetCaps(eGraphicCaps_MaxAnisotropicFiltering));

	}
	//-----------------------------------------------------------------------

	int cLowLevelGraphicsAndroid::GetCaps(eGraphicCaps aType)
	{
		switch(aType)
		{
		//Texture Rectangle
		case eGraphicCaps_TextureTargetRectangle:
			{
				return 0;//ARB_texture_rectangle
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
				//DEBUG:
				//return 2;

				int lUnits;
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,(GLint *)&lUnits);
				return lUnits;
			}

		//Max Texture Coord Units
		case eGraphicCaps_MaxTextureCoordUnits:
			{
				int lUnits;
				glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,(GLint *)&lUnits);
				return lUnits;
			}
		//Texture Anisotropy
		case eGraphicCaps_AnisotropicFiltering:
			{
				if(GLAD_GL_EXT_texture_filter_anisotropic) return 1;
				else return 0;
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
				//Debbug:
				//return 0;
				return 1;
			}

		//GL Fragment program
		case eGraphicCaps_GL_FragmentProgram:
			{
				//Debbug:
				//return 0;
				return 1;
			}
		
		//GL NV register combiners
		case eGraphicCaps_GL_NVRegisterCombiners:
			{
				return 0;
			}

		//GL NV register combiners Max stages
		case eGraphicCaps_GL_NVRegisterCombiners_MaxStages:
			{
				return 0;
			}

		//GL ATI Fragment Shader
		case eGraphicCaps_GL_ATIFragmentShader:
			{
				return 0;
			}

		default:
			return 0;
		}

		return 0;
	}
	
	void cLowLevelGraphicsAndroid::SetVsyncActive(bool abX)
	{
		mSwapInterval = abX ? 1 : 0;
		eglSwapInterval(mEglDisplay, mSwapInterval);
	}
	void cLowLevelGraphicsAndroid::SetMultisamplingActive(bool abX)
	{
		//There is no glEnable(GL_MULTISAMPLE) in gles2
	}
	void cLowLevelGraphicsAndroid::SetGammaCorrection(float afX)
	{
		
	}
	float cLowLevelGraphicsAndroid::GetGammaCorrection()
	{
		return 1;
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

	//-----------------------------------------------------------------------
	
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
	
	
	
	void cLowLevelGraphicsAndroid::FlushRendering()
	{
		glFlush();
	}
	void cLowLevelGraphicsAndroid::SwapBuffers()
	{
		glFlush();
		eglSwapBuffers(mEglDisplay, mEglSurface);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::PushMatrix(eMatrix aMtxType)
	{
		mMatrixStack[aMtxType].push(mMatrixStack[aMtxType].top());
	}

	void cLowLevelGraphicsAndroid::PopMatrix(eMatrix aMtxType)
	{
		mMatrixStack[aMtxType].pop();
	}

	void cLowLevelGraphicsAndroid::SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA)
	{
		mMatrixStack[aMtxType].top() = a_mtxA;

		UploadShaderMatrix();
	}

	void cLowLevelGraphicsAndroid::SetIdentityMatrix(eMatrix aMtxType)
	{
		mMatrixStack[aMtxType].top() = cMatrixf::Identity;

		UploadShaderMatrix();
	}

	void cLowLevelGraphicsAndroid::TranslateMatrix(eMatrix aMtxType, const cVector3f &avPos)
	{
		cMatrixf &r = mMatrixStack[aMtxType].top();
		cMatrixf m = r;
		for(int i=0;i<4;i++)
			r.m[i][3] = m.m[i][0] * avPos.x + m.m[i][1] * avPos.y + m.m[i][2] * avPos.z + m.m[i][3];
	}

	/**
	 * \todo fix so that there are X, Y , Z versions of this one.
	 * \param aMtxType
	 * \param &avRot
	 */
	void cLowLevelGraphicsAndroid::RotateMatrix(eMatrix aMtxType, const cVector3f &avRot)
	{
		//TODO its unused anyway
		//mMatrixStack[aMtxType].top().Rotate(avRot);
	}

	void cLowLevelGraphicsAndroid::ScaleMatrix(eMatrix aMtxType, const cVector3f &avScale)
	{
		cMatrixf &r = mMatrixStack[aMtxType].top();
		cMatrixf m = r;
		for(int i=0;i<3;i++)
			for(int j=0;j<4;j++)
				r.m[j][i] = m.m[j][i] * avScale.v[i];
	}

	void cLowLevelGraphicsAndroid::SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax)
	{
		cMatrixf &r = mMatrixStack[eMatrix_Projection].top();
		r = cMatrixf::Identity;
		r.m[0][0] = 2.0f / avSize.x;
		r.m[1][1] = 2.0f / -avSize.y;
		r.m[2][2] = -2.0f / (afMax - afMin);
		r.m[0][3] = -1.0f;
		r.m[1][3] = 1.0f;
		r.m[2][3] = - (afMax + afMin) / (afMax - afMin);
		r.m[3][3] = 1;
	}

	void cLowLevelGraphicsAndroid::ClearScreen()
	{
		GLbitfield bitmask=0;

		if(mbClearColor)bitmask |= GL_COLOR_BUFFER_BIT;
		if(mbClearDepth)bitmask |= GL_DEPTH_BUFFER_BIT;
		if(mbClearStencil)bitmask |= GL_STENCIL_BUFFER_BIT;

		glClear(bitmask);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetClearColor(const cColor& aCol)
	{
		glClearColor(aCol.r, aCol.g, aCol.b, aCol.a);
	}

	void cLowLevelGraphicsAndroid::SetClearDepth(float afDepth)
	{
		glClearDepthf(afDepth);
	}

	void cLowLevelGraphicsAndroid::SetClearStencil(int alVal)
	{
		glClearStencil(alVal);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetClearColorActive(bool abX)
	{
		mbClearColor=abX;
	}

	void cLowLevelGraphicsAndroid::SetClearDepthActive(bool abX)
	{
		mbClearDepth=abX;
	}

	void cLowLevelGraphicsAndroid::SetClearStencilActive(bool abX)
	{
		mbClearStencil=abX;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetColorWriteActive(bool abR,bool abG,bool abB,bool abA)
	{
		glColorMask(abR,abG,abB,abA);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetDepthWriteActive(bool abX)
	{
		glDepthMask(abX);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetDepthTestActive(bool abX)
	{
		if(abX) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetDepthTestFunc(eDepthTestFunc aFunc)
	{
		glDepthFunc(GetGLDepthTestFuncEnum(aFunc));
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetAlphaTestActive(bool abX)
	{
		//TODO QCOM extension and shader emulation
		//if(abX) glEnable(GL_ALPHA_TEST);
		//else glDisable(GL_ALPHA_TEST);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef)
	{
		//glAlphaFunc(GetGLAlphaTestFuncEnum(aFunc),afRef);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetStencilActive(bool abX)
	{
		if(abX) glEnable(GL_STENCIL_TEST);
		else glDisable(GL_STENCIL_TEST);
	}
	
	void cLowLevelGraphicsAndroid::SetStencil(eStencilFunc aFunc,int alRef, unsigned int aMask,
					eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp)
	{
		glStencilFunc(GetGLStencilFuncEnum(aFunc), alRef, aMask);

		glStencilOp(GetGLStencilOpEnum(aFailOp), GetGLStencilOpEnum(aZFailOp),
					GetGLStencilOpEnum(aZPassOp));
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetStencilTwoSide(eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
					int alRef, unsigned int aMask,
					eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
					eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp)
	{
		//Front
		glStencilOpSeparate( GL_FRONT, GetGLStencilOpEnum(aFrontFailOp),
								GetGLStencilOpEnum(aFrontZFailOp),
								GetGLStencilOpEnum(aFrontZPassOp));
		//Back
		glStencilOpSeparate( GL_BACK, GetGLStencilOpEnum(aBackFailOp),
								GetGLStencilOpEnum(aBackZFailOp),
								GetGLStencilOpEnum(aBackZPassOp));

		glStencilFuncSeparate( GL_FRONT, GetGLStencilFuncEnum(aFrontFunc),
								alRef, aMask);
		glStencilFuncSeparate( GL_BACK, GetGLStencilFuncEnum(aBackFunc),
								alRef, aMask);
	}

	void cLowLevelGraphicsAndroid::SetStencilTwoSide(bool abX)
	{
		//glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
		//in gles2 it's always on
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetCullActive(bool abX)
	{
		if(abX) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	void cLowLevelGraphicsAndroid::SetCullMode(eCullMode aMode)
	{
		glCullFace(GL_BACK);
		if(aMode == eCullMode_Clockwise)
			glFrontFace(GL_CCW);
		else
			glFrontFace(GL_CW);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetScissorActive(bool abX)
	{
		if(abX) glEnable(GL_SCISSOR_TEST);
		else glDisable(GL_SCISSOR_TEST);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetScissorRect(const cRect2l &aRect)
	{
		glScissor(aRect.x, (mvScreenSize.y - aRect.y - 1)-aRect.h, aRect.w, aRect.h);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetBlendActive(bool abX)
	{
		if(abX)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor)
	{
		glBlendFunc(GetGLBlendEnum(aSrcFactor),GetGLBlendEnum(aDestFactor));
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetBlendFuncSeparate(eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
		eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha)
	{
		glBlendFuncSeparate(GetGLBlendEnum(aSrcFactorColor),
								GetGLBlendEnum(aDestFactorColor),
								GetGLBlendEnum(aSrcFactorAlpha),
								GetGLBlendEnum(aDestFactorAlpha));
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetTexture(unsigned int alUnit,iTexture* apTex)
	{
		if(apTex == NULL){
			apTex = mDefaultTexture;
		}

		if(apTex == mpCurrentTexture[alUnit])
			return;

		GLenum NewTarget = 0;
		if(apTex)
			NewTarget = GetGLTextureTargetEnum(apTex->GetTarget());
		GLenum LastTarget = 0;
		if(mpCurrentTexture[alUnit])
			LastTarget = GetGLTextureTargetEnum(mpCurrentTexture[alUnit]->GetTarget());

		glActiveTexture(GL_TEXTURE0 + alUnit);

		glBindTexture(NewTarget, apTex->GetCurrentLowlevelHandle());

		mpCurrentTexture[alUnit] = apTex;
	}

	void cLowLevelGraphicsAndroid::SetActiveTextureUnit(unsigned int alUnit)
	{
		glActiveTexture(GL_TEXTURE0 + alUnit);
	}

	void cLowLevelGraphicsAndroid::SetTextureEnv(eTextureParam aParam, int alVal)
	{

	}

	void cLowLevelGraphicsAndroid::SetTextureConstantColor(const cColor &aColor)
	{

	}

	void cLowLevelGraphicsAndroid::SetColor(const cColor &aColor)
	{

	}

	//-----------------------------------------------------------------------
	
	void cLowLevelGraphicsAndroid::AddVertexToBatch(const cVertex *apVtx)
	{
		//Coord
		mpVertexArray[mlVertexCount + 0] =	apVtx->pos.x;
		mpVertexArray[mlVertexCount + 1] =	apVtx->pos.y;
		mpVertexArray[mlVertexCount + 2] =	apVtx->pos.z;
		//Color
		mpVertexArray[mlVertexCount + 3] =	apVtx->col.r;
		mpVertexArray[mlVertexCount + 4] =	apVtx->col.g;
		mpVertexArray[mlVertexCount + 5] =	apVtx->col.b;
		mpVertexArray[mlVertexCount + 6] =	apVtx->col.a;
		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	apVtx->tex.x;
		mpVertexArray[mlVertexCount + 8] =	apVtx->tex.y;
		mpVertexArray[mlVertexCount + 9] =	apVtx->tex.z;
		//Normal coord
		mpVertexArray[mlVertexCount + 10] =	apVtx->norm.x;
		mpVertexArray[mlVertexCount + 11] =	apVtx->norm.y;
		mpVertexArray[mlVertexCount + 12] =	apVtx->norm.z;

		mlVertexCount += mlBatchStride;

		if(mlVertexCount/mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::AddVertexToBatch(const cVertex *apVtx, const cVector3f* avTransform)
	{
		//Coord
		mpVertexArray[mlVertexCount + 0] =	apVtx->pos.x+avTransform->x;
		mpVertexArray[mlVertexCount + 1] =	apVtx->pos.y+avTransform->y;
		mpVertexArray[mlVertexCount + 2] =	apVtx->pos.z+avTransform->z;
		//Color
		mpVertexArray[mlVertexCount + 3] =	apVtx->col.r;
		mpVertexArray[mlVertexCount + 4] =	apVtx->col.g;
		mpVertexArray[mlVertexCount + 5] =	apVtx->col.b;
		mpVertexArray[mlVertexCount + 6] =	apVtx->col.a;
		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	apVtx->tex.x;
		mpVertexArray[mlVertexCount + 8] =	apVtx->tex.y;
		mpVertexArray[mlVertexCount + 9] =	apVtx->tex.z;
		//Normal coord
		mpVertexArray[mlVertexCount + 10] =	apVtx->norm.x;
		mpVertexArray[mlVertexCount + 11] =	apVtx->norm.y;
		mpVertexArray[mlVertexCount + 12] =	apVtx->norm.z;

		mlVertexCount += mlBatchStride;

		if(mlVertexCount/mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::AddVertexToBatch_Size2D(const cVertex *apVtx, const cVector3f* avTransform,
		const cColor* apCol,const float& mfW, const float& mfH)
	{
		//Coord
		mpVertexArray[mlVertexCount + 0] =	avTransform->x + mfW;
		mpVertexArray[mlVertexCount + 1] =	avTransform->y + mfH;
		mpVertexArray[mlVertexCount + 2] =	avTransform->z;
		//Color
		mpVertexArray[mlVertexCount + 3] =	apCol->r;
		mpVertexArray[mlVertexCount + 4] =	apCol->g;
		mpVertexArray[mlVertexCount + 5] =	apCol->b;
		mpVertexArray[mlVertexCount + 6] =	apCol->a;
		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	apVtx->tex.x;
		mpVertexArray[mlVertexCount + 8] =	apVtx->tex.y;
		mpVertexArray[mlVertexCount + 9] =	apVtx->tex.z;

		mlVertexCount += mlBatchStride;

		if(mlVertexCount/mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::AddVertexToBatch_Raw(	const cVector3f& avPos, const cColor &aColor,
														const cVector3f& avTex)
	{
		//Coord
		mpVertexArray[mlVertexCount + 0] =	avPos.x;
		mpVertexArray[mlVertexCount + 1] =	avPos.y;
		mpVertexArray[mlVertexCount + 2] =	avPos.z;
		//Color
		mpVertexArray[mlVertexCount + 3] =	aColor.r;
		mpVertexArray[mlVertexCount + 4] =	aColor.g;
		mpVertexArray[mlVertexCount + 5] =	aColor.b;
		mpVertexArray[mlVertexCount + 6] =	aColor.a;
		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	avTex.x;
		mpVertexArray[mlVertexCount + 8] =	avTex.y;
		mpVertexArray[mlVertexCount + 9] =	avTex.z;

		mlVertexCount += mlBatchStride;
	}


	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::AddIndexToBatch(int alIndex)
	{
		mpIndexArray[mlIndexCount] = alIndex;
		mlIndexCount++;

		if(mlIndexCount>=mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::FlushTriBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear)
	{
		mSimpleShader->Bind();
		UploadShaderMatrix();

		SetVtxBatchStates(aTypeFlags);
		SetUpBatchArrays();

		glDrawElements(GL_TRIANGLES,mlIndexCount,GL_UNSIGNED_INT, mpIndexArray);

		if(abAutoClear){
			mlIndexCount = 0;
			mlVertexCount = 0;
			for(int i=0;i<MAX_TEXTUREUNITS;i++)
				mlTexCoordArrayCount[i]=0;
		}
	}

	void cLowLevelGraphicsAndroid::ClearBatch()
	{
		mlIndexCount = 0;
		mlVertexCount = 0;
	}
	
	//-----------------------------------------------------------------------
	
	void cLowLevelGraphicsAndroid::SetUpBatchArrays()
	{
		//Set the arrays
		glVertexAttribPointer(eVtxAttr_Position, 3, GL_FLOAT, false, sizeof(float)*mlBatchStride, mpVertexArray);
		glVertexAttribPointer(eVtxAttr_Color0, 4, GL_FLOAT, false, sizeof(float)*mlBatchStride, &mpVertexArray[3]);
		glVertexAttribPointer(eVtxAttr_Normal, 3, GL_FLOAT, false, sizeof(float)*mlBatchStride, &mpVertexArray[10]);

		glVertexAttribPointer(eVtxAttr_Texture0, 3, GL_FLOAT, false, sizeof(float)*mlBatchStride, &mpVertexArray[7]);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetVtxBatchStates(tVtxBatchFlag aFlags)
	{
		if(aFlags & eVtxBatchFlag_Position)
			glEnableVertexAttribArray( eVtxAttr_Position );
		else glDisableVertexAttribArray( eVtxAttr_Position );

		if(aFlags & eVtxBatchFlag_Color0)
			glEnableVertexAttribArray( eVtxAttr_Color0 );
		else glDisableVertexAttribArray( eVtxAttr_Color0 );

		if(aFlags & eVtxBatchFlag_Normal)
			glEnableVertexAttribArray( eVtxAttr_Normal );
		else glDisableVertexAttribArray( eVtxAttr_Normal );

		if(aFlags & eVtxBatchFlag_Texture0)
			glEnableVertexAttribArray( eVtxAttr_Texture0 );
		else glDisableVertexAttribArray( eVtxAttr_Texture0 );
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::UploadShaderMatrix()
	{
		cMatrixf mtx = cMath::MatrixMul(mMatrixStack[eMatrix_Projection].top(),mMatrixStack[eMatrix_ModelView].top());
		mSimpleShader->SetMatrixf("worldViewProj",mtx);
	}

}

