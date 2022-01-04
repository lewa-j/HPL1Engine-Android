#pragma once

#include "graphics/LowLevelGraphics.h"

namespace hpl
{
	class cGLSLProgram;
	
	class cLowLevelGraphicsGL : public iLowLevelGraphics
	{
	public:
		cLowLevelGraphicsGL();
		virtual ~cLowLevelGraphicsGL();

		virtual bool Init(int alWidth, int alHeight, int alBpp, int abFullscreen, int alMultisampling,
			const tString &asWindowCaption) override;

		//virtual int GetCaps(eGraphicCaps aType) override;

		virtual cVector2f GetScreenSize() override { return cVector2f((float)mvScreenSize.x, (float)mvScreenSize.y); }
		virtual cVector2f GetVirtualSize() override { return mvVirtualSize; }
		virtual void SetVirtualSize(cVector2f avSize) override { mvVirtualSize = avSize; }

		/////////// MATRIX METHODS /////////////////////////
		virtual void SetMatrix(eMatrix aMtxType, const cMatrixf &a_mtxA) override;
		virtual void SetIdentityMatrix(eMatrix aMtxType) override;
#if 0
		virtual void PushMatrix(eMatrix aMtxType) override;
		virtual void PopMatrix(eMatrix aMtxType) override;

		virtual cMatrixf GetMatrix(eMatrix aMtxType) override;

		virtual void TranslateMatrix(eMatrix aMtxType, const cVector3f &avPos) override;
		virtual void RotateMatrix(eMatrix aMtxType, const cVector3f &avRot) override;
		virtual void ScaleMatrix(eMatrix aMtxType, const cVector3f &avScale) override;

		virtual void SetOrthoProjection(const cVector2f &avSize, float afMin, float afMax) override;
#endif
		/////////// DRAWING METHODS /////////////////////////

		// CLEARING THE FRAMEBUFFER
		virtual void ClearScreen() override;

		virtual void SetClearColor(const cColor &aCol) override;
		virtual void SetClearStencil(int alVal) override;

		virtual void SetClearColorActive(bool abX) override;
		virtual void SetClearDepthActive(bool abX) override;
		virtual void SetClearStencilActive(bool abX) override;

		virtual void SetColorWriteActive(bool abR, bool abG, bool abB, bool abA) override;
		virtual void SetDepthWriteActive(bool abX) override;

		virtual void SetCullActive(bool abX) override;
		virtual void SetCullMode(eCullMode aMode) override;

		//DEPTH
		virtual void SetDepthTestActive(bool abX) override;
		virtual void SetDepthTestFunc(eDepthTestFunc aFunc) override;

		//STENCIL
		virtual void SetStencilActive(bool abX) override;
		virtual void SetStencil(eStencilFunc aFunc, int alRef, unsigned int aMask,
			eStencilOp aFailOp, eStencilOp aZFailOp, eStencilOp aZPassOp) override;
		virtual void SetStencilTwoSide(eStencilFunc aFrontFunc, eStencilFunc aBackFunc,
			int alRef, unsigned int aMask,
			eStencilOp aFrontFailOp, eStencilOp aFrontZFailOp, eStencilOp aFrontZPassOp,
			eStencilOp aBackFailOp, eStencilOp aBackZFailOp, eStencilOp aBackZPassOp) override;

		//SCISSOR
		virtual void SetScissorActive(bool abX) override;
		virtual void SetScissorRect(const cRect2l &aRect) override;

		//BLENDING
		virtual void SetBlendActive(bool abX) override;
		virtual void SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor) override;
		virtual void SetBlendFuncSeparate(eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
			eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha) override;

		// TEXTURE
		virtual void SetActiveTextureUnit(unsigned int alUnit) override;
		virtual void SetTexture(unsigned int alUnit, iTexture *apTex) override;

		virtual void SetColor(const cColor &aColor) override;

		// POLYGONS
		virtual void DrawQuad(const tVertexVec &avVtx) override;
#if 0
		virtual void DrawRect(const cVector2f &avPos, const cVector2f &avSize, float afZ) override;

		virtual void DrawTri(const tVertexVec &avVtx) override;
		virtual void DrawTri(const cVertex *avVtx) override;

		virtual void DrawQuad(const tVertexVec &avVtx, const cColor aCol) override;
		virtual void DrawQuad(const tVertexVec &avVtx, const float afZ) override;
		virtual void DrawQuad(const tVertexVec &avVtx, const float afZ, const cColor &aCol) override;
		virtual void DrawQuadMultiTex(const tVertexVec &avVtx, const tVector3fVec &avExtraUvs) override;
#endif
		//PRIMITIVES
		virtual void DrawLine(const cVector3f &avBegin, const cVector3f &avEnd, cColor aCol) override;
		virtual void DrawSphere(const cVector3f &avPos, float afRadius, cColor aCol) override;
#if 0
		virtual void DrawBoxMaxMin(const cVector3f &avMax, const cVector3f &avMin, cColor aCol) override;

		virtual void DrawLine2D(const cVector2f &avBegin, const cVector2f &avEnd, float afZ, cColor aCol) override;
		virtual void DrawLineRect2D(const cRect2f &aRect, float afZ, cColor aCol) override;
		virtual void DrawLineCircle2D(const cVector2f &avCenter, float afRadius, float afZ, cColor aCol) override;

		virtual void DrawFilledRect2D(const cRect2f &aRect, float afZ, cColor aCol) override;
#endif
		virtual void AddVertexToBatch(const cVertex *apVtx) override;
		virtual void AddVertexToBatch(const cVertex *apVtx, const cVector3f *avTransform) override;

		virtual void AddVertexToBatch_Size2D(const cVertex *apVtx, const cVector3f *avTransform,
			const cColor *apCol, const float &mfW, const float &mfH) override;
 
		virtual void AddVertexToBatch_Raw(const cVector3f &avPos, const cColor &aColor,
			const cVector3f &avTex) override;
#if 0
		virtual void AddTexCoordToBatch(unsigned int alUnit, const cVector3f *apCoord) override;
		virtual void SetBatchTextureUnitActive(unsigned int alUnit, bool abActive) override;
#endif
		virtual void AddIndexToBatch(int alIndex) override;

		virtual void FlushTriBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear = true) override;
		virtual void ClearBatch() override;

		//FRAMEBUFFER
		virtual void CopyContextToTexure(iTexture *apTex, const cVector2l &avPos,
			const cVector2l &avSize, const cVector2l &avTexOffset = 0) override;

		// GL and platform specific
		virtual void BindTextureGL(iTexture *apTex, int aNewTarget) = 0;
		virtual void UnbindRenderTargetTextureGL(iTexture *apTex, int aLastTarget) = 0;
	protected:
		cVector2l mvScreenSize;
		cVector2f mvVirtualSize;

		//Vertex Array variables
		//The vertex arrays used:
		float *mpVertexArray = nullptr;
		unsigned int mlVertexCount = 0;
		unsigned int *mpIndexArray = nullptr;
		unsigned int mlIndexCount = 0;

		unsigned int mlBatchStride = 0;

		float *mpTexCoordArray[MAX_TEXTUREUNITS] = { nullptr };
		bool mbTexCoordArrayActive[MAX_TEXTUREUNITS] = {};
		unsigned int mlTexCoordArrayCount[MAX_TEXTUREUNITS] = {};

		unsigned int mlBatchArraySize = 0;

		//Clearing
		bool mbClearColor = false;
		bool mbClearDepth = false;
		bool mbClearStencil = false;

		//Texture
		iTexture *mpCurrentTexture[MAX_TEXTUREUNITS] = {nullptr};

		cGLSLProgram *mSimpleShader = nullptr;
		iTexture *mDefaultTexture = nullptr;

		void SetupGL();

		//Batch helper
		void SetUpBatchArrays();
		void SetVtxBatchStates(tVtxBatchFlag aFlags);
		
		friend class cGLSLProgram;
	};
};
