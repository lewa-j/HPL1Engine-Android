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
#ifndef HPL_LOWLEVELGRAPHICS_SDL_H
#define HPL_LOWLEVELGRAPHICS_SDL_H

#include "impl/platform/gl.h"

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
// Unix's X11 Defines DestoryAll which collides with methods
#undef DestroyAll

#include "impl/LowLevelGraphicsGL.h"
#include "impl/SDLPixelFormat.h"
#include "math/MathTypes.h"

namespace hpl
{

	//-------------------------------------------------

	class cLowLevelGraphicsSDL : public cLowLevelGraphicsGL
	{
	public:
		cLowLevelGraphicsSDL();
		~cLowLevelGraphicsSDL();

		bool Init(	int alWidth, int alHeight, int alBpp, int abFullscreen, int alMultisampling,
					const tString& asWindowCaption) override;

		int GetCaps(eGraphicCaps aType) override;

		void ShowCursor(bool abX) override;

		void SetVsyncActive(bool abX) override;

		void SetMultisamplingActive(bool abX) override;
		int GetMultisampling() override { return mlMultisampling;}

		void SetGammaCorrection(float afX) override;
		float GetGammaCorrection() override;

		void SetClipPlane(int alIdx, const cPlanef& aPlane) override;
		cPlanef GetClipPlane(int alIdx, const cPlanef& aPlane) override;
		void SetClipPlaneActive(int alIdx, bool abX) override;

		void SaveScreenToBMP(const tString &asFile) override;

		iPixelFormat *GetPixelFormat() override { return mpPixelFormat; }

		iBitmap2D* CreateBitmap2D(const cVector2l &avSize, unsigned int alBpp) override;
		iFontData* CreateFontData(const tString &asName) override;

		iTexture* CreateTexture(bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget) override;
		iTexture* CreateTexture(const tString &asName,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget) override;
		iTexture* CreateTexture(iBitmap2D* apBmp,bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget) override;
		iTexture* CreateTexture(const cVector2l& avSize,int alBpp,cColor aFillCol,
								bool abUseMipMaps, eTextureType aType, eTextureTarget aTarget) override;

		iGpuProgram *CreateGpuProgram(const tString& asName) override;
		iGpuShader *CreateGpuShader(const tString &asName, eGpuShaderType aType) override;

		iOcclusionQuery *CreateOcclusionQuery() override;
		void DestroyOcclusionQuery(iOcclusionQuery *apQuery) override;

		iVertexBuffer *CreateVertexBuffer(tVertexFlag aFlags, eVertexBufferDrawType aDrawType,
			eVertexBufferUsageType aUsageType,
			int alReserveVtxSize = 0, int alReserveIdxSize = 0) override;

		/////////// MATRIX METHODS /////////////////////////

		void PushMatrix(eMatrix aMtxType) override;
		void PopMatrix(eMatrix aMtxType) override;
		void SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA) override;
		cMatrixf GetMatrix(eMatrix aMtxType) override;

		void SetIdentityMatrix(eMatrix aMtxType) override;
		void TranslateMatrix(eMatrix aMtxType, const cVector3f &avPos) override;
		void RotateMatrix(eMatrix aMtxType, const cVector3f &avRot) override;
		void ScaleMatrix(eMatrix aMtxType, const cVector3f &avScale) override;

		void SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax) override;

		/////////// DRAWING METHODS /////////////////////////

		void SetClearDepth(float afDepth) override;

		//ALPHA
		void SetAlphaTestActive(bool abX) override;
		void SetAlphaTestFunc(eAlphaTestFunc aFunc,float afRef) override;

		// TEXTURE
		void SetTextureEnv(eTextureParam aParam, int alVal) override;
		void SetTextureConstantColor(const cColor &aColor) override;

		void SetColor(const cColor &aColor) override;

		// POLYGONS
		void DrawRect(const cVector2f &avPos,const cVector2f &avSize,float afZ) override;

		void DrawTri(const tVertexVec &avVtx) override;
		void DrawTri(const cVertex* avVtx) override;

		void DrawQuad(const tVertexVec &avVtx, const cColor aCol) override;
		void DrawQuad(const tVertexVec &avVtx,const float afZ) override;
		void DrawQuad(const tVertexVec &avVtx,const float afZ,const cColor &aCol) override;
		void DrawQuadMultiTex(const tVertexVec &avVtx,const tVector3fVec &avExtraUvs) override;

		void AddTexCoordToBatch(unsigned int alUnit, const cVector3f *apCoord) override;
		void SetBatchTextureUnitActive(unsigned int alUnit, bool abActive) override;

		//PRIMITIVES
		void DrawBoxMaxMin(const cVector3f& avMax, const cVector3f& avMin, cColor aCol) override;

		void DrawLine2D(const cVector2f& avBegin, const cVector2f& avEnd, float afZ, cColor aCol) override;
		void DrawLineRect2D(const cRect2f& aRect, float afZ, cColor aCol) override;
		void DrawFilledRect2D(const cRect2f& aRect, float afZ, cColor aCol) override;

		//FRAMEBUFFER
		void SetRenderTarget(iTexture* pTex) override;
		bool RenderTargetHasZBuffer() override;
		void FlushRenderTarget() override;

		void FlushRendering() override;
		void SwapBuffers() override;

		///// SDL Specific ////////////////////////////
		virtual void BindTextureGL(iTexture *apTex, int aNewTarget) override;
		virtual void UnbindRenderTargetTextureGL(iTexture *apTex, int aLastTarget) override;

		iBitmap2D* CreateBitmap2DFromSurface(SDL_Surface* apSurface,const tString& asType);

		void SetupGL();

	private:
		int mlMultisampling = 0;
		int mlBpp = 0;

		//Windows stuff
		#if defined(WIN32)
			HGLRC mGLContext = nullptr;
			HDC   mDeviceContext = nullptr;
			HINSTANCE mhKeyTrapper = nullptr;
		#elif defined(__linux__)
			Display *gDpy = nullptr;
			GLXContext glCtx;
			GLXPbuffer gPBuffer;
		#endif

		//Gamma
		Uint16 mvStartGammaArray[3][256]={};
		float mfGammaCorrection = 1.0f;

		//SDL Variables
		SDL_Surface *mpScreen = nullptr;
		cSDLPixelFormat *mpPixelFormat = nullptr;

		//Clipping
		cPlanef mvClipPlanes[kMaxClipPlanes];

		//Rendertarget variables
		iTexture* mpRenderTarget = nullptr;

		//Multisample
		void CheckMultisampleCaps();

		//Matrix Helper
		void SetMatrixMode(eMatrix mType);

	};
};
#endif // HPL_LOWLEVELGRAPHICS_SDL_H
