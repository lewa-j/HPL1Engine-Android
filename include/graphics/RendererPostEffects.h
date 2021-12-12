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
#ifndef HPL_RENDERER_POST_EFFECTS_H
#define HPL_RENDERER_POST_EFFECTS_H

#include <set>
#include <list>
#include <math.h>

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {

#define kFilterProgramNum (1)

	class iLowLevelGraphics;
	class iLowLevelResources;
	class cGraphics;
	class iGpuProgram;
	class cResources;
	class iTexture;
	class cGpuShaderManager;
	class cRenderList;
	class cRenderer3D;

	enum ePostEffectFilter
	{
		ePostEffectFilter_Offset,
		ePostEffectFilter_LastEnum
	};

	enum ePostEffectProgram
	{
		ePostEffectProgram_Offset,
		ePostEffectProgram_LastEnum
	};

	class cImageTrailEffect
	{
	public:
		cImageTrailEffect() : mbActive(false), mbFirstPass(false), mlCurrentBuffer(0) {}

		bool mbActive;
		bool mbFirstPass;
		int mlCurrentBuffer;
		float mfAmount;
	};

	class cResources;

	class cRendererPostEffects
	{
	public:
		cRendererPostEffects(cGraphics *apGraphics, cResources* apResources,
							cRenderList *apRenderList);
		~cRendererPostEffects();

		/**
		 * Render post effects, called by cScene
		 */
		void Render();

		void SetImageTrailActive(bool abX){
			if(!mImageTrailData.mbActive && abX) mImageTrailData.mbFirstPass = true;
			mImageTrailData.mbActive = abX;}
		bool GetImageTrailActive(){return mImageTrailData.mbActive;}
		/**
		 * Set the amount of blur
		 * \param afAmount 0.0 - 1.0 are valid
		 */
		void SetImageTrailAmount(float afAmount){mImageTrailData.mfAmount = afAmount;}

		iTexture* GetScreenBuffer(int alNum){ return mpScreenBuffer[alNum];}

		void SetActive(bool abX){ mbActive = abX;}
		bool GetActive(){ return mbActive;}

		void SetBloomActive(bool abX);
		bool GetBloomActive(){ return mbBloomActive;}

		void SetBloomSpread(float afX){mfBloomSpread = afX;}
		float GetBloomSpread(){ return mfBloomSpread;}

		void SetMotionBlurActive(bool abX);
		bool GetMotionBlurActive(){ return mbMotionBlurActive;}

		void SetMotionBlurAmount(float afX){ mfMotionBlurAmount = afX;}
		float GetMotionBlurAmount(){ return mfMotionBlurAmount;}

		void SetDepthOfFieldActive(bool abX){ mbDofActive = abX;}
		void SetDepthOfFieldMaxBlur(float afX){ mfDofMaxBlur = afX;}
		void SetDepthOfFieldFocalPlane(float afX){ mfDofFocalPlane = afX;}
		void SetDepthOfFieldNearPlane(float afX){ mfDofNearPlane = afX;}
		void SetDepthOfFieldFarPlane(float afX){ mfDofFarPlane = afX;}

		bool GetDepthOfFieldActive(){ return mbDofActive;}
		float GetDepthOfFieldMaxBlur(){ return mfDofMaxBlur;}
		float GetDepthOfFieldFocalPlane(){ return mfDofFocalPlane;}
		float GetDepthOfFieldNearPlane(){ return mfDofNearPlane;}
		float GetDepthOfFieldFarPlane(){ return mfDofFarPlane;}

		iTexture* GetFreeScreenTexture(){ return mpScreenBuffer[mImageTrailData.mlCurrentBuffer==0?1:0];}

		void RenderBlurTexture(iTexture *apDestination, iTexture *apSource,float afBlurAmount);

	private:
		void RenderImageTrail();

		void RenderBloom();

		void RenderMotionBlur();

		void RenderDepthOfField();

		iLowLevelGraphics *mpLowLevelGraphics;
		iLowLevelResources *mpLowLevelResources;
		cGraphics *mpGraphics;
		cResources* mpResources;
		cGpuShaderManager* mpGpuManager;
		cRenderer3D *mpRenderer3D;

		cRenderList *mpRenderList;

		cVector2f mvScreenSize;

		iTexture* mpScreenBuffer[2];

		cImageTrailEffect mImageTrailData;

		iGpuProgram *mpBlur2dP;
		iGpuProgram *mpBlurRectP;
		bool mbBlurFallback;

		iGpuProgram *mpBloomP;

		iTexture *mpBloomBlurTexture;

		iGpuProgram *mpMotionBlurP;

		iGpuProgram *mpDepthOfFieldP;
		iTexture *mpDofBlurTexture;

		tVertexVec mvTexRectVtx;

		bool mbBloomActive;
		float mfBloomSpread;

		bool mbMotionBlurActive;
		float mfMotionBlurAmount;
		bool mbMotionBlurFirstTime;

		bool mbDofActive;
		float mfDofMaxBlur;
		float mfDofFocalPlane;
		float mfDofNearPlane;
		float mfDofFarPlane;

		bool mbActive;
	};

};
#endif // HPL_RENDERER_POST_EFFECTS_H
