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
#ifndef HPL_TEXTURE_H
#define HPL_TEXTURE_H

#include <vector>
#include "graphics/LowLevelPicture.h"
#include "graphics/Bitmap2D.h"
#include "graphics/PixelFormat.h"
#include "graphics/GraphicsTypes.h"
#include "resources/ResourceBase.h"

namespace hpl {

	enum eTextureType
	{
		eTextureType_Normal,
		eTextureType_RenderTarget,
		eTextureType_LastEnum
	};

	//-----------------------------------------

	enum eTextureTarget
	{
		eTextureTarget_2D,
		eTextureTarget_Rect,
		eTextureTarget_CubeMap,
		eTextureTarget_3D,
		eTextureTarget_LastEnum
	};

	//-----------------------------------------

	enum eTextureWrap
	{
		eTextureWrap_Repeat,
		eTextureWrap_Clamp,
		eTextureWrap_ClampToEdge,
		eTextureWrap_ClampToBorder,
		eTextureWrap_LastEnum
	};

	//-----------------------------------------

	enum eTextureFilter
	{
		eTextureFilter_Bilinear,
		eTextureFilter_Trilinear,
		eTextureFilter_LastEnum
	};

	//-----------------------------------------

	enum eTextureAnimMode
	{
		eTextureAnimMode_None,
		eTextureAnimMode_Loop,
		eTextureAnimMode_Oscillate,
		eTextureAnimMode_LastEnum
	};

	//-----------------------------------------

	class iLowLevelGraphics;

	class iTexture : public iLowLevelPicture, public iResourceBase
	{
	public:
		iTexture(tString asName,tString asType,iPixelFormat *apPxlFmt,iLowLevelGraphics* apLowLevelGraphics,
				eTextureType aType,bool abUseMipMaps, eTextureTarget aTarget,
				bool abCompress=false)
			: iLowLevelPicture(asType), iResourceBase(asName,0),
				mType(aType), mbUseMipMaps(abUseMipMaps),
				mpLowLevelGraphics(apLowLevelGraphics), mbCompress(abCompress),
				mTarget(aTarget),
				mWrapS(eTextureWrap_Repeat), mWrapT(eTextureWrap_Repeat),mWrapR(eTextureWrap_Repeat),
				mfFrameTime(1), mAnimMode(eTextureAnimMode_Loop), mlSizeLevel(0), mvMinLevelSize(16,16),
				mfAnisotropyDegree(1.0f),mFilter(eTextureFilter_Bilinear){}

		virtual ~iTexture(){}

		void SetSizeLevel(unsigned int alLevel){mlSizeLevel = alLevel;}
		void SetMinLevelSize(const cVector2l& avSize){ mvMinLevelSize = avSize;}

		bool Reload(){ return false;}
		void Unload(){}
		void Destroy(){}

		/**
		 * Create a texture from a bitmap, work only for 1D, 2D and Rect targets. Doesn't work with render targets.
		 * \param pBmp
		 * \return
		 */
		virtual bool CreateFromBitmap(iBitmap2D* pBmp)=0;
		/**
		 * Create a cube map texture from a vector of bitmaps. Doesn't work with render targets.
		 * All bitmaps most be square, a power of 2 and the same same. The order must be: posX, negX, posY, negY, posZ and negZ.
		 * \param *avBitmaps a vector with at least 6 bitmaps
		 * \return
		 */
		virtual bool CreateCubeFromBitmapVec(tBitmap2DVec *avBitmaps)=0;
		/**
		 * Create a texture with color, works with all target types. Works with render targets.
		 * \param alWidth
		 * \param alHeight
		 * \param aCol
		 * \return
		 */
		virtual bool Create(unsigned int alWidth, unsigned int alHeight, cColor aCol)=0;

		virtual bool CreateAnimFromBitmapVec(tBitmap2DVec *avBitmaps)=0;

		virtual bool CreateFromArray(unsigned char *apPixelData, int alChannels, const cVector3l &avSize)=0;

		virtual void Update(float afTimeStep)=0;

		virtual void SetPixels2D(int alLevel, const cVector2l& avOffset, const cVector2l& avSize,
								eColorDataFormat aDataFormat, void *apPixelData)=0;

		virtual void SetFilter(eTextureFilter aFilter)=0;
		virtual void SetAnisotropyDegree(float afX)=0;
		eTextureFilter GetFilter(){ return mFilter;}
		float GetAnisotropyDegree(float afX){ return mfAnisotropyDegree;}

		virtual float GetGamma()=0;
		virtual void SetGamma(float afGamma)=0;
		virtual int GetHandle()=0;

		virtual bool HasAlpha(){return false;}

		virtual void SetWrapS(eTextureWrap aMode)=0;
		virtual void SetWrapT(eTextureWrap aMode)=0;
		virtual void SetWrapR(eTextureWrap aMode)=0;

		eTextureWrap GetWrapS(){ return mWrapS;}
		eTextureWrap GetWrapT(){ return mWrapT;}
		eTextureWrap GetWrapR(){ return mWrapR;}

		void SetFrameTime(float afX){ mfFrameTime = afX;}
		float GetFrameTime(){ return mfFrameTime;}

		eTextureAnimMode GetAnimMode() { return mAnimMode;}
		void SetAnimMode(eTextureAnimMode aMode) {mAnimMode = aMode;};

		eTextureType GetTextureType(){ return mType; }
		bool UsesMipMaps(){ return mbUseMipMaps; }
		void SetMipMapUse(bool abX){mbUseMipMaps = abX;}
		eTextureTarget GetTarget(){ return mTarget;}

		virtual bool HasAnimation()=0;
		virtual void NextFrame()=0;
		virtual void PrevFrame()=0;
		virtual float GetT()=0;
		virtual float GetTimeCount()=0;
		virtual void SetTimeCount(float afX)=0;
		virtual int GetCurrentLowlevelHandle()=0;

	protected:
		eTextureType mType;
		eTextureTarget mTarget;
		eTextureWrap mWrapS;
		eTextureWrap mWrapT;
		eTextureWrap mWrapR;
		eTextureFilter mFilter;
		float mfAnisotropyDegree;

		bool mbUseMipMaps;
		bool mbCompress;
		iLowLevelGraphics* mpLowLevelGraphics;
		float mfFrameTime;
		eTextureAnimMode mAnimMode;
		unsigned int mlSizeLevel;
		cVector2l mvMinLevelSize;

	};

	typedef std::vector<iTexture*> tTextureVec;
	typedef tTextureVec::iterator tTextureVecIt;
};
#endif // HPL_TEXTURE_H
