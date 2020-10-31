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
#ifndef HPL_LOWLEVELGRAPHICS_ANDROID_H
#define HPL_LOWLEVELGRAPHICS_ANDROID_H

#include "graphics/LowLevelGraphics.h"
#include "math/MathTypes.h"

namespace hpl
{
	class cLowLevelGraphicsAndroid : public iLowLevelGraphics
	{
	public:
		cLowLevelGraphicsAndroid();
		~cLowLevelGraphicsAndroid();

		bool Init(	int alWidth, int alHeight, int alBpp, int abFullscreen, int alMultisampling,
					const tString& asWindowCaption);
		
		int GetCaps(eGraphicCaps aType){return 0;}
		void ShowCursor(bool abX){}
		void SetVsyncActive(bool abX){}
		void SetMultisamplingActive(bool abX){}
		void SetGammaCorrection(float afX){}
		float GetGammaCorrection(){return 1;}

		int GetMultisampling(){ return mlMultisampling;}

		void SetClipPlane(int alIdx, const cPlanef& aPlane){}
		cPlanef GetClipPlane(int alIdx, const cPlanef& aPlane){return{};}
		void SetClipPlaneActive(int alIdx, bool abX){}

		cVector2f GetScreenSize();
		cVector2f GetVirtualSize();
		void SetVirtualSize(cVector2f avSize);

		iBitmap2D* CreateBitmap2D(const cVector2l &avSize, unsigned int alBpp);
		iFontData* CreateFontData(const tString &asName);

		iTexture* CreateTexture(bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget);
		iTexture* CreateTexture(const tString &asName,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget);
		iTexture* CreateTexture(iBitmap2D* apBmp,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget);
		iTexture* CreateTexture(const cVector2l& avSize,int alBpp,cColor aFillCol,
								bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget);


		iPixelFormat* GetPixelFormat(){return nullptr;}

		iGpuProgram* CreateGpuProgram(const tString& asName, eGpuProgramType aType);

		void SaveScreenToBMP(const tString& asFile){}

		/////////// MATRIX METHODS /////////////////////////

		void PushMatrix(eMatrix aMtxType){}
		void PopMatrix(eMatrix aMtxType){}
		void SetIdentityMatrix(eMatrix aMtxType){}

		void SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA){}

		void TranslateMatrix(eMatrix aMtxType, const cVector3f &avPos){}
		void RotateMatrix(eMatrix aMtxType, const cVector3f &avRot){}
		void ScaleMatrix(eMatrix aMtxType, const cVector3f &avScale){}

		void SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax){}

		/////////// DRAWING METHODS /////////////////////////

		// OCCLUSION
		iOcclusionQuery* CreateOcclusionQuery(){return nullptr;}
		void DestroyOcclusionQuery(iOcclusionQuery *apQuery){}

		// CLEARING THE FRAMEBUFFER
		void ClearScreen(){}

		void SetClearColor(const cColor& aCol){}
		void SetClearDepth(float afDepth){}
		void SetClearStencil(int alVal){}

		void SetClearColorActive(bool abX){}
		void SetClearDepthActive(bool abX){}
		void SetClearStencilActive(bool abX){}

		void SetColorWriteActive(bool abR,bool abG,bool abB,bool abA){}
		void SetDepthWriteActive(bool abX){}

		void SetCullActive(bool abX){}
		void SetCullMode(eCullMode aMode){}

		//DEPTH
		void SetDepthTestActive(bool abX){}
		void SetDepthTestFunc(eDepthTestFunc aFunc){}

		//ALPHA
		void SetAlphaTestActive(bool abX){}
		void SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef){}

		//STENCIL
		void SetStencilActive(bool abX){}
		void SetStencil(eStencilFunc aFunc,int alRef, unsigned int aMask,
				eStencilOp aFailOp,eStencilOp aZFailOp,eStencilOp aZPassOp){}
		void SetStencilTwoSide(eStencilFunc aFrontFunc,eStencilFunc aBackFunc,
				int alRef, unsigned int aMask,
				eStencilOp aFrontFailOp,eStencilOp aFrontZFailOp,eStencilOp aFrontZPassOp,
				eStencilOp aBackFailOp,eStencilOp aBackZFailOp,eStencilOp aBackZPassOp){}
		void SetStencilTwoSide(bool abX){}


		//SCISSOR
		void SetScissorActive(bool abX){}
		void SetScissorRect(const cRect2l &aRect){}

		//BLENDING
		void SetBlendActive(bool abX){}
		void SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor){}
		void SetBlendFuncSeparate(eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
			eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha){}


		// TEXTURE
		void SetTexture(unsigned int alUnit,iTexture* apTex){}
		void SetActiveTextureUnit(unsigned int alUnit){}
		void SetTextureEnv(eTextureParam aParam, int alVal){}
		void SetTextureConstantColor(const cColor &aColor){}

		void SetColor(const cColor &aColor){}

		// POLYGONS
		iVertexBuffer* CreateVertexBuffer(tVertexFlag aFlags, eVertexBufferDrawType aDrawType,
										eVertexBufferUsageType aUsageType,
										int alReserveVtxSize=0,int alReserveIdxSize=0);

		void DrawRect(const cVector2f &avPos,const cVector2f &avSize,float afZ){}

		void DrawTri(const tVertexVec &avVtx){}
		void DrawTri(const cVertex* avVtx){}

		void DrawQuad(const tVertexVec &avVtx){}
		void DrawQuad(const tVertexVec &avVtx, const cColor aCol){}
		void DrawQuad(const tVertexVec &avVtx,const float afZ){}
		void DrawQuad(const tVertexVec &avVtx,const float afZ,const cColor &aCol){}
		void DrawQuadMultiTex(const tVertexVec &avVtx,const tVector3fVec &avExtraUvs){}

		void AddVertexToBatch(const cVertex *apVtx){}
		void AddVertexToBatch(const cVertex *apVtx, const cVector3f* avTransform){}
		void AddVertexToBatch(const cVertex *apVtx, const cMatrixf* aMtx){}

		void AddVertexToBatch_Size2D(const cVertex *apVtx, const cVector3f* avTransform,
										const cColor* apCol,const float& mfW, const float& mfH){}

		void AddVertexToBatch_Raw(	const cVector3f& avPos, const cColor &aColor,
									const cVector3f& avTex){}


		void AddTexCoordToBatch(unsigned int alUnit,const cVector3f *apCoord){}
		void SetBatchTextureUnitActive(unsigned int alUnit,bool abActive){}

		void AddIndexToBatch(int alIndex){}

		void FlushTriBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear=true){}
		void FlushQuadBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear=true){}
		void ClearBatch(){}

		//PRIMITIVES
		void DrawLine(const cVector3f& avBegin, const cVector3f& avEnd, cColor aCol){}
		void DrawBoxMaxMin(const cVector3f& avMax, const cVector3f& avMin, cColor aCol){}
		void DrawSphere(const cVector3f& avPos, float afRadius, cColor aCol){}

		void DrawLine2D(const cVector2f& avBegin, const cVector2f& avEnd, float afZ, cColor aCol){}
		void DrawLineRect2D(const cRect2f& aRect, float afZ, cColor aCol){}
		void DrawLineCircle2D(const cVector2f& avCenter, float afRadius, float afZ, cColor aCol){}

		void DrawFilledRect2D(const cRect2f& aRect, float afZ, cColor aCol){}

		//FRAMEBUFFER
		void CopyContextToTexure(iTexture* apTex, const cVector2l &avPos,
			const cVector2l &avSize, const cVector2l &avTexOffset=0){}
		void SetRenderTarget(iTexture* pTex){}
		bool RenderTargetHasZBuffer(){return false;}
		void FlushRenderTarget(){}

		void FlushRendering(){}
		void SwapBuffers(){}
		
		//Platform specific
		iBitmap2D* CreateBitmap2DFromData(uint8_t *data,int w, int h, int n,const tString& asType);
		
	private:
		cVector2l mvScreenSize;
		cVector2f mvVirtualSize;
		int mlMultisampling;
		int mlBpp;
		
		//Vertex Array variables
		//The vertex arrays used:
		float* mpVertexArray;
		unsigned int mlVertexCount;
		unsigned int* mpIndexArray;
		unsigned int mlIndexCount;

		unsigned int mlBatchStride;

		float *mpTexCoordArray[MAX_TEXTUREUNITS];
		bool mbTexCoordArrayActive[MAX_TEXTUREUNITS];
		unsigned int mlTexCoordArrayCount[MAX_TEXTUREUNITS];

		unsigned int mlBatchArraySize;

		//Clearing
		bool mbClearColor;
		bool mbClearDepth;
		bool mbClearStencil;

		//Rendertarget variables
		iTexture* mpRenderTarget;

		//Texture
		iTexture* mpCurrentTexture[MAX_TEXTUREUNITS];

	};
}
#endif // HPL_LOWLEVELGRAPHICS_ANDROID_H

