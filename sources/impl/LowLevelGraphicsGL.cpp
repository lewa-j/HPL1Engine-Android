
#include "impl/LowLevelGraphicsGL.h"
#include "impl/platform/gl.h"
#include "impl/GLHelpers.h"
#include "impl/GLSLProgram.h"
#include "system/Platform.h"
#include "graphics/GPUShader.h"

#include <assert.h>

namespace hpl
{

	cLowLevelGraphicsGL::cLowLevelGraphicsGL()
	{
		mvVirtualSize.x = 800;
		mvVirtualSize.y = 600;

		for (int i = 0; i < MAX_TEXTUREUNITS; i++)
			mpCurrentTexture[i] = NULL;

		mbClearColor = true;
		mbClearDepth = true;
		mbClearStencil = false;

		mlBatchArraySize = 20000;
		mlVertexCount = 0;
		mlIndexCount = 0;
		//Create the batch arrays:
		mlBatchStride = 13;
		//3 Pos floats, 4 color floats, 3 Tex coord floats .
		mpVertexArray = (float *)hplMalloc(sizeof(float) * mlBatchStride * mlBatchArraySize);
		mpIndexArray = (unsigned int *)hplMalloc(sizeof(unsigned int) * mlBatchArraySize); //Index is one int.

		for (int i = 0; i < MAX_TEXTUREUNITS; i++)
		{
			mpTexCoordArray[i] = (float *)hplMalloc(sizeof(float) * 3 * mlBatchArraySize);
			mbTexCoordArrayActive[i] = false;
			mlTexCoordArrayCount[i] = 0;
		}
	}

	cLowLevelGraphicsGL::~cLowLevelGraphicsGL()
	{
		hplFree(mpVertexArray);
		hplFree(mpIndexArray);
		for (int i = 0; i < MAX_TEXTUREUNITS; i++)
			hplFree(mpTexCoordArray[i]);
	}

	bool cLowLevelGraphicsGL::Init(int alWidth, int alHeight, int alBpp, int abFullscreen, int alMultisampling, const tString &asWindowCaption)
	{
		mvScreenSize.x = alWidth;
		mvScreenSize.y = alHeight;

		return true;
	}

	void cLowLevelGraphicsGL::SetupGL()
	{
		//Inits GL stuff

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		SetClearDepth(1.0);//Values buffer is cleared with
		glEnable(GL_DEPTH_TEST); //enable depth testing
		glDepthFunc(GL_LEQUAL); //function to do depth test with

		glClearStencil(0);

		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//Show some info
		Log("  Vendor: %s\n", glGetString(GL_VENDOR));
		Log("  Renderer: %s\n", glGetString(GL_RENDERER));
		Log("  Version: %s\n", glGetString(GL_VERSION));
		Log("  GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		tString shadersPath = cPlatform::GetPlatformPath(tString("core/shaders/"));

		iGpuShader *pShaderV = CreateGpuShader(shadersPath + "Simple.vert", eGpuShaderType::Vertex);
		if (pShaderV->CreateFromFile(cString::To16Char(shadersPath + "Simple.vert")) == false)
		{
			Error("Couldn't create shader 'Simple.vert'\n");
			hplDelete(pShaderV);
			pShaderV = nullptr;
		}
		iGpuShader *pShaderF = CreateGpuShader(shadersPath + "Simple.frag", eGpuShaderType::Fragment);
		if (pShaderF->CreateFromFile(cString::To16Char(shadersPath + "Simple.frag")) == false)
		{
			Error("Couldn't create shader 'Simple.frag'\n");
			hplDelete(pShaderF);
			pShaderF = nullptr;
		}
		if (pShaderV && pShaderF)
		{
			mSimpleShader = (cGLSLProgram*)CreateGpuProgram("Simple");
			mSimpleShader->SetShader(eGpuShaderType::Vertex, pShaderV);
			mSimpleShader->SetShader(eGpuShaderType::Fragment, pShaderF);
			mSimpleShader->Link();

			mSimpleShader->Bind();
			mSimpleShader->SetColor4f("color", cColor(1, 1, 1, 1));
			Log("Basic shader created: %d %p\n", mSimpleShader->mlHandle, mSimpleShader);
			mSimpleShader->UnBind();
		}

		mDefaultTexture = CreateTexture({ 1,1 }, 32, cColor(1, 1), false, eTextureType_Normal, eTextureTarget_2D);

		glVertexAttrib4f(eVtxAttr_Color0, 1, 1, 1, 1);
	}

	void cLowLevelGraphicsGL::SetMatrix(eMatrix aMtxType, const cMatrixf &a_mtxA)
	{
		if (mSimpleShader->IsBound())
			mSimpleShader->SetMatrixf("worldViewProj", eGpuProgramMatrix::ViewProjection, eGpuProgramMatrixOp::Identity);
	}

	void cLowLevelGraphicsGL::SetIdentityMatrix(eMatrix aMtxType)
	{
		if (mSimpleShader->IsBound())
			mSimpleShader->SetMatrixf("worldViewProj", eGpuProgramMatrix::ViewProjection, eGpuProgramMatrixOp::Identity);
	}

	void cLowLevelGraphicsGL::ClearScreen()
	{
		GLbitfield bitmask = 0;

		if (mbClearColor)bitmask |= GL_COLOR_BUFFER_BIT;
		if (mbClearDepth)bitmask |= GL_DEPTH_BUFFER_BIT;
		if (mbClearStencil)bitmask |= GL_STENCIL_BUFFER_BIT;

		glClear(bitmask);
	}

	void cLowLevelGraphicsGL::SetClearColor(const cColor &aCol)
	{
		glClearColor(aCol.r, aCol.g, aCol.b, aCol.a);
	}

	void cLowLevelGraphicsGL::SetClearStencil(int alVal)
	{
		glClearStencil(alVal);
	}

	void cLowLevelGraphicsGL::SetClearColorActive(bool abX)
	{
		mbClearColor = abX;
	}

	void cLowLevelGraphicsGL::SetClearDepthActive(bool abX)
	{
		mbClearDepth = abX;
	}

	void cLowLevelGraphicsGL::SetClearStencilActive(bool abX)
	{
		mbClearStencil = abX;
	}

	void cLowLevelGraphicsGL::SetColorWriteActive(bool abR, bool abG, bool abB, bool abA)
	{
		glColorMask(abR, abG, abB, abA);
	}

	void cLowLevelGraphicsGL::SetDepthWriteActive(bool abX)
	{
		glDepthMask(abX);
	}

	void cLowLevelGraphicsGL::SetDepthTestActive(bool abX)
	{
		if (abX) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}

	void cLowLevelGraphicsGL::SetDepthTestFunc(eDepthTestFunc aFunc)
	{
		glDepthFunc(GetGLDepthTestFuncEnum(aFunc));
	}

	void cLowLevelGraphicsGL::SetStencilActive(bool abX)
	{
		if (abX) glEnable(GL_STENCIL_TEST);
		else glDisable(GL_STENCIL_TEST);
	}

	void cLowLevelGraphicsGL::SetStencil(eStencilFunc aFunc, int alRef, unsigned int aMask,
		eStencilOp aFailOp, eStencilOp aZFailOp, eStencilOp aZPassOp)
	{
		glStencilFunc(GetGLStencilFuncEnum(aFunc), alRef, aMask);

		glStencilOp(GetGLStencilOpEnum(aFailOp), GetGLStencilOpEnum(aZFailOp),
			GetGLStencilOpEnum(aZPassOp));
	}

	void cLowLevelGraphicsGL::SetStencilTwoSide(eStencilFunc aFrontFunc, eStencilFunc aBackFunc,
		int alRef, unsigned int aMask,
		eStencilOp aFrontFailOp, eStencilOp aFrontZFailOp, eStencilOp aFrontZPassOp,
		eStencilOp aBackFailOp, eStencilOp aBackZFailOp, eStencilOp aBackZPassOp)
	{
		//Front
		glStencilOpSeparate(GL_FRONT, GetGLStencilOpEnum(aFrontFailOp),
								GetGLStencilOpEnum(aFrontZFailOp),
								GetGLStencilOpEnum(aFrontZPassOp));
		//Back
		glStencilOpSeparate(GL_BACK, GetGLStencilOpEnum(aBackFailOp),
								GetGLStencilOpEnum(aBackZFailOp),
								GetGLStencilOpEnum(aBackZPassOp));

		glStencilFuncSeparate(GL_FRONT, GetGLStencilFuncEnum(aFrontFunc),
								alRef, aMask);
		glStencilFuncSeparate(GL_BACK, GetGLStencilFuncEnum(aBackFunc),
								alRef, aMask);
	}

	void cLowLevelGraphicsGL::SetCullActive(bool abX)
	{
		if (abX) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void cLowLevelGraphicsGL::SetCullMode(eCullMode aMode)
	{
		glCullFace(GL_BACK);
		if (aMode == eCullMode::Clockwise) glFrontFace(GL_CCW);
		else							glFrontFace(GL_CW);
	}

	void cLowLevelGraphicsGL::SetScissorActive(bool abX)
	{
		if (abX) glEnable(GL_SCISSOR_TEST);
		else glDisable(GL_SCISSOR_TEST);
	}

	void cLowLevelGraphicsGL::SetScissorRect(const cRect2l &aRect)
	{
		glScissor(aRect.x, (mvScreenSize.y - aRect.y - 1) - aRect.h, aRect.w, aRect.h);
	}

	void cLowLevelGraphicsGL::SetBlendActive(bool abX)
	{
		if (abX)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}

	void cLowLevelGraphicsGL::SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor)
	{
		glBlendFunc(GetGLBlendEnum(aSrcFactor), GetGLBlendEnum(aDestFactor));
	}

	void cLowLevelGraphicsGL::SetBlendFuncSeparate(eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
		eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha)
	{
		glBlendFuncSeparate(GetGLBlendEnum(aSrcFactorColor),
			GetGLBlendEnum(aDestFactorColor),
			GetGLBlendEnum(aSrcFactorAlpha),
			GetGLBlendEnum(aDestFactorAlpha));
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsGL::SetActiveTextureUnit(unsigned int alUnit)
	{
		glActiveTexture(GL_TEXTURE0 + alUnit);
	}

	void cLowLevelGraphicsGL::SetTexture(unsigned int alUnit, iTexture *apTex)
	{
		//Bind default white texture if NULL
		if (apTex == nullptr)
			apTex = mDefaultTexture;

		if (apTex == mpCurrentTexture[alUnit])
			return;

		GLenum NewTarget = GL_TEXTURE_2D;
		if (apTex)
			NewTarget = GetGLTextureTargetEnum(apTex->GetTarget());
		GLenum LastTarget = GL_TEXTURE_2D;
		if (mpCurrentTexture[alUnit])
			LastTarget = GetGLTextureTargetEnum(mpCurrentTexture[alUnit]->GetTarget());

		glActiveTexture(GL_TEXTURE0 + alUnit);

		//If the current texture in this unit is a render target, unbind it.
		if (mpCurrentTexture[alUnit] && mpCurrentTexture[alUnit]->GetTextureType() == eTextureType_RenderTarget)
		{
			UnbindRenderTargetTextureGL(mpCurrentTexture[alUnit], LastTarget);
		}

		//Enable the unit, set the texture handle and bind the pbuffer
		if (apTex)
		{
			BindTextureGL(apTex, NewTarget);
		}

		mpCurrentTexture[alUnit] = apTex;
	}

	void cLowLevelGraphicsGL::SetColor(const cColor &aColor)
	{
		if (mSimpleShader->IsBound())
			mSimpleShader->SetColor4f("color", aColor);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsGL::CopyContextToTexure(iTexture *apTex, const cVector2l &avPos,
		const cVector2l &avSize, const cVector2l &avTexOffset)
	{
		if (apTex == NULL)return;

		int lScreenY = (mvScreenSize.y - avSize.y) - avPos.y;
		int lTexY = (apTex->GetHeight() - avSize.y) - avTexOffset.y;

		//Log("TExoffset: %d %d\n",avTexOffset.x,lTexY);
		//Log("ScreenOffset: %d %d (h: %d s: %d p: %d)\n",avPos.x,lScreenY,mvScreenSize.y,
		//												avSize.y,avPos.y);

		SetTexture(0, apTex);
		glCopyTexSubImage2D(GetGLTextureTargetEnum(apTex->GetTarget()), 0,
			avTexOffset.x, lTexY,
			avPos.x, lScreenY, avSize.x, avSize.y);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsGL::DrawQuad(const tVertexVec &avVtx)
	{
		assert(avVtx.size() == 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray(eVtxAttr_Position);
		glEnableVertexAttribArray(eVtxAttr_Color0);
		glEnableVertexAttribArray(eVtxAttr_Texture0);
		glVertexAttribPointer(eVtxAttr_Position, 3, GL_FLOAT, false, sizeof(cVertex), &avVtx[0].pos.x);
		glVertexAttribPointer(eVtxAttr_Color0, 4, GL_FLOAT, false, sizeof(cVertex), &avVtx[0].col.r);
		glVertexAttribPointer(eVtxAttr_Texture0, 3, GL_FLOAT, false, sizeof(cVertex), &avVtx[0].tex.x);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	void cLowLevelGraphicsGL::DrawLine(const cVector3f &avBegin, const cVector3f &avEnd, cColor aCol)
	{
		float verts[6]{
			avBegin.x, avBegin.y, avBegin.z,
			avEnd.x, avEnd.y, avEnd.z
		};

		SetTexture(0, NULL);
		SetColor(aCol);

		glDisableVertexAttribArray(eVtxAttr_Color0);
		glDisableVertexAttribArray(eVtxAttr_Texture0);
		glEnableVertexAttribArray(eVtxAttr_Position);
		glVertexAttribPointer(eVtxAttr_Position, 3, GL_FLOAT, false, 0, verts);
		glDrawArrays(GL_LINES, 0, 2);
	}

	void cLowLevelGraphicsGL::DrawSphere(const cVector3f &avPos, float afRadius, cColor aCol)
	{
		constexpr int alSegments = 32;
		constexpr float afAngleStep = k2Pif / (float)alSegments;
		float verts[alSegments * 6 * 3];
		int vi = 0;

		{
			//X Circle:
			for (float a = 0; a < k2Pif; a += afAngleStep)
			{
				verts[vi++] = avPos.x;
				verts[vi++] = avPos.y + sin(a) * afRadius;
				verts[vi++] = avPos.z + cos(a) * afRadius;

				verts[vi++] = avPos.x;
				verts[vi++] = avPos.y + sin(a + afAngleStep) * afRadius;
				verts[vi++] = avPos.z + cos(a + afAngleStep) * afRadius;
			}

			//Y Circle:
			for (float a = 0; a < k2Pif; a += afAngleStep)
			{
				verts[vi++] = avPos.x + cos(a) * afRadius;
				verts[vi++] = avPos.y;
				verts[vi++] = avPos.z + sin(a) * afRadius;

				verts[vi++] = avPos.x + cos(a + afAngleStep) * afRadius;
				verts[vi++] = avPos.y;
				verts[vi++] = avPos.z + sin(a + afAngleStep) * afRadius;
			}

			//Z Circle:
			for (float a = 0; a < k2Pif; a += afAngleStep)
			{
				verts[vi++] = avPos.x + cos(a) * afRadius;
				verts[vi++] = avPos.y + sin(a) * afRadius;
				verts[vi++] = avPos.z;

				verts[vi++] = avPos.x + cos(a + afAngleStep) * afRadius;
				verts[vi++] = avPos.y + sin(a + afAngleStep) * afRadius;
				verts[vi++] = avPos.z;
			}
		}

		SetTexture(0, NULL);
		SetBlendActive(false);
		SetColor(aCol);

		glDisableVertexAttribArray(eVtxAttr_Color0);
		glDisableVertexAttribArray(eVtxAttr_Texture0);
		glEnableVertexAttribArray(eVtxAttr_Position);
		glVertexAttribPointer(eVtxAttr_Position, 3, GL_FLOAT, false, 0, verts);
		glDrawArrays(GL_LINES, 0, alSegments * 6);
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsGL::AddVertexToBatch(const cVertex *apVtx)
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

		if (mlVertexCount / mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	void cLowLevelGraphicsGL::AddVertexToBatch(const cVertex *apVtx, const cVector3f *avTransform)
	{
		//Coord
		mpVertexArray[mlVertexCount + 0] =	apVtx->pos.x + avTransform->x;
		mpVertexArray[mlVertexCount + 1] =	apVtx->pos.y + avTransform->y;
		mpVertexArray[mlVertexCount + 2] =	apVtx->pos.z + avTransform->z;
		//Color
		mpVertexArray[mlVertexCount + 3] =	apVtx->col.r;
		mpVertexArray[mlVertexCount + 4] =	apVtx->col.g;
		mpVertexArray[mlVertexCount + 5] =	apVtx->col.b;
		mpVertexArray[mlVertexCount + 6] =	apVtx->col.a;
		//Texture coord
		mpVertexArray[mlVertexCount + 7] =	apVtx->tex.x;
		mpVertexArray[mlVertexCount + 8] =	apVtx->tex.y;
		mpVertexArray[mlVertexCount + 9] =	apVtx->tex.z;
		//Normal
		mpVertexArray[mlVertexCount + 10] =	apVtx->norm.x;
		mpVertexArray[mlVertexCount + 11] =	apVtx->norm.y;
		mpVertexArray[mlVertexCount + 12] =	apVtx->norm.z;

		mlVertexCount += mlBatchStride;

		if (mlVertexCount / mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	void cLowLevelGraphicsGL::AddVertexToBatch_Size2D(const cVertex *apVtx, const cVector3f *avTransform,
		const cColor *apCol, const float &mfW, const float &mfH)
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

		if (mlVertexCount / mlBatchStride >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	void cLowLevelGraphicsGL::AddVertexToBatch_Raw(const cVector3f &avPos, const cColor &aColor,
		const cVector3f &avTex)
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

	void cLowLevelGraphicsGL::AddIndexToBatch(int alIndex)
	{
		mpIndexArray[mlIndexCount] = alIndex;
		mlIndexCount++;

		if (mlIndexCount >= mlBatchArraySize)
		{
			//Make the array larger.
		}
	}

	void cLowLevelGraphicsGL::FlushTriBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear)
	{
		mSimpleShader->Bind();
		if (mSimpleShader->IsBound())
			mSimpleShader->SetMatrixf("worldViewProj", eGpuProgramMatrix::ViewProjection, eGpuProgramMatrixOp::Identity);

		SetVtxBatchStates(aTypeFlags);
		SetUpBatchArrays();

		glDrawElements(GL_TRIANGLES, mlIndexCount, GL_UNSIGNED_INT, mpIndexArray);

		if (abAutoClear) {
			mlIndexCount = 0;
			mlVertexCount = 0;
			for (int i = 0; i < MAX_TEXTUREUNITS; i++)
				mlTexCoordArrayCount[i] = 0;
		}
	}

	void cLowLevelGraphicsGL::ClearBatch()
	{
		mlIndexCount = 0;
		mlVertexCount = 0;
	}

	//-----------------------------------------------------------------------

	void cLowLevelGraphicsGL::SetUpBatchArrays()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//Set the arrays
		glVertexAttribPointer(eVtxAttr_Position, 3, GL_FLOAT, false, sizeof(float) * mlBatchStride, mpVertexArray);
		glVertexAttribPointer(eVtxAttr_Color0, 4, GL_FLOAT, false, sizeof(float) * mlBatchStride, &mpVertexArray[3]);
		glVertexAttribPointer(eVtxAttr_Normal, 3, GL_FLOAT, false, sizeof(float) * mlBatchStride, &mpVertexArray[10]);

		glVertexAttribPointer(eVtxAttr_Texture0, 3, GL_FLOAT, false, sizeof(float) * mlBatchStride, &mpVertexArray[7]);
		// TODO
		// eVtxBatchFlag_Texture1
		// eVtxBatchFlag_Texture2
	}

	void cLowLevelGraphicsGL::SetVtxBatchStates(tVtxBatchFlag aFlags)
	{
		if (aFlags & eVtxBatchFlag_Position)
			glEnableVertexAttribArray(eVtxAttr_Position);
		else glDisableVertexAttribArray(eVtxAttr_Position);

		if (aFlags & eVtxBatchFlag_Color0)
			glEnableVertexAttribArray(eVtxAttr_Color0);
		else glDisableVertexAttribArray(eVtxAttr_Color0);

		if (aFlags & eVtxBatchFlag_Normal)
			glEnableVertexAttribArray(eVtxAttr_Normal);
		else glDisableVertexAttribArray(eVtxAttr_Normal);

		if (aFlags & eVtxBatchFlag_Texture0)
			glEnableVertexAttribArray(eVtxAttr_Texture0);
		else glDisableVertexAttribArray(eVtxAttr_Texture0);

		// TODO
		// eVtxBatchFlag_Texture1
		// eVtxBatchFlag_Texture2
	}

} // hpl
