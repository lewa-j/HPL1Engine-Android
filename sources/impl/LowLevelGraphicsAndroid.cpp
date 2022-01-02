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
#include "graphics/OcclusionQuery.h"

namespace hpl
{

	cLowLevelGraphicsAndroid::cLowLevelGraphicsAndroid(ANativeWindow *apAWindow) : cLowLevelGraphicsGL()
	{
		mpAWindow = apAWindow;

		mlMultisampling = 0;

		mpRenderTarget = nullptr;

		for(int i=0;i<eMatrix_LastEnum;i++)
			mMatrixStack[i].push(cMatrixf::Identity);
	}

	static const bool egl_log = true;

	cLowLevelGraphicsAndroid::~cLowLevelGraphicsAndroid()
	{
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
		if (!cLowLevelGraphicsGL::Init(alWidth, alHeight, alBpp, abFullscreen, alMultisampling, asWindowCaption))
			return false;

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
	
		int cfgAttribs[] = {
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

		//mvVirtualSize.y = mvVirtualSize.x/(mvScreenSize.x/(float)mvScreenSize.y);
		
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
		cLowLevelGraphicsGL::SetupGL();

		//Inits GL stuff

		//glDisable(GL_ALPHA_TEST);

		static const char* storage = getenv("EXTERNAL_STORAGE");
		tString shadersPath = storage + tString("/hpl1/core/shaders/");

		mSimpleShader = CreateGpuProgram("Simple", eGpuProgramType_Linked );
		mSimpleShader->CreateFromFiles(shadersPath+"Simple.vs", shadersPath+"Simple.fs");

		mSimpleShader->Bind();

		mDefaultTexture = CreateTexture({ 1,1 }, 32, cColor(1, 1), false, eTextureType_Normal, eTextureTarget_2D);

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
		return 1.0f;
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

	//-----------------------------------------------------------------------

	class cOcclusionQueryNull : public iOcclusionQuery
	{
	public:
		cOcclusionQueryNull(){}
		~cOcclusionQueryNull(){}

		void Begin(){}
		void End(){}
		bool FetchResults(){return true;}
		unsigned int GetSampleCount(){return 100;}
	};

	iOcclusionQuery* cLowLevelGraphicsAndroid::CreateOcclusionQuery()
	{
		return hplNew(cOcclusionQueryNull, () );
	}

	void cLowLevelGraphicsAndroid::DestroyOcclusionQuery(iOcclusionQuery *apQuery)
	{
		if(apQuery)	hplDelete(apQuery);
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

	cMatrixf cLowLevelGraphicsAndroid::GetMatrix(eMatrix aMtxType)
	{
		return mMatrixStack[aMtxType].top();
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

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetClearDepth(float afDepth)
	{
		glClearDepthf(afDepth);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::SetAlphaTestActive(bool abX)
	{
		//TODO QCOM extension and shader emulation
		//if(abX) glEnable(GL_ALPHA_TEST);
		//else glDisable(GL_ALPHA_TEST);
	}

	void cLowLevelGraphicsAndroid::SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef)
	{
		//glAlphaFunc(GetGLAlphaTestFuncEnum(aFunc),afRef);
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

	void cLowLevelGraphicsAndroid::DrawQuad(const tVertexVec &avVtx)
	{
		assert(avVtx.size() == 4);

		glVertexAttribPointer(eVtxAttr_Position, 3, GL_FLOAT, false, sizeof(cVertex), &avVtx[0].pos.x);
		glVertexAttribPointer(eVtxAttr_Color0, 4, GL_FLOAT, false, sizeof(cVertex), &avVtx[0].col.r);
		glVertexAttribPointer(eVtxAttr_Texture0, 3, GL_FLOAT, false, sizeof(cVertex), &avVtx[0].tex.x);

		uint16_t quadIndices[]{0,1,2,2,3,0};
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, quadIndices);
	}

	//-----------------------------------------------------------------------

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

	void cLowLevelGraphicsAndroid::FlushTriBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear)
	{
		mSimpleShader->Bind();
		UploadShaderMatrix();
		cLowLevelGraphicsGL::FlushTriBatch(aTypeFlags, abAutoClear);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsAndroid::UploadShaderMatrix()
	{
		cMatrixf mtx = cMath::MatrixMul(mMatrixStack[eMatrix_Projection].top(), mMatrixStack[eMatrix_ModelView].top());
		mSimpleShader->SetMatrixf("worldViewProj", mtx);
	}

}

