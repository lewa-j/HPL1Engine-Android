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
#ifndef HPL_ANDROID_TEXTURE_H
#define HPL_ANDROID_TEXTURE_H

#include "graphics/Texture.h"
#include "impl/LowLevelGraphicsAndroid.h"
#include "impl/AndroidBitmap2D.h"

namespace hpl
{
	class cAndroidTexture : public iTexture
	{
	public:
		cAndroidTexture(const tString &asName,iPixelFormat *apPxlFmt,iLowLevelGraphics* apLowLevelGraphics,
					eTextureType aType, bool abUseMipMaps, eTextureTarget aTarget,
					bool abCompress=false);
		~cAndroidTexture();

		bool CreateFromBitmap(iBitmap2D* pBmp);

		bool CreateAnimFromBitmapVec(tBitmap2DVec *avBitmaps);

		bool CreateCubeFromBitmapVec(tBitmap2DVec *avBitmaps);
		bool Create(unsigned int alWidth, unsigned int alHeight, cColor aCol);

		bool CreateFromArray(unsigned char *apPixelData, int alChannels, const cVector3l &avSize);

		void SetPixels2D(	int alLevel, const cVector2l& avOffset, const cVector2l& avSize,
							eColorDataFormat aDataFormat, void *apPixelData);


		float GetGamma(){return 0;}
		void SetGamma(float afGamma){}
		int GetHandle(){return (int) mvTextureHandles[0];}

		void SetFilter(eTextureFilter aFilter);
		void SetAnisotropyDegree(float afX);

		void SetWrapS(eTextureWrap aMode);
		void SetWrapT(eTextureWrap aMode);
		void SetWrapR(eTextureWrap aMode);

		void Update(float afTimeStep);

		bool HasAnimation();
		void NextFrame();
		void PrevFrame();
		float GetT();
		float GetTimeCount();
		void SetTimeCount(float afX);
		int GetCurrentLowlevelHandle();

		// GL Specific 

		unsigned int GetTextureHandle();

	private:
		bool CreateFromBitmapToHandle(iBitmap2D* pBmp, int alHandleIdx);

		unsigned int InitCreation(int alHandleIdx);
		void PostCreation(unsigned int aGLTarget);

		tUIntVec mvTextureHandles;
		bool mbContainsData;
		cLowLevelGraphicsAndroid* mpGfx;

		float mfTimeCount;
		int mlTextureIndex;
		float mfTimeDir;

		//cPBuffer *mpPBuffer;
	};

};
#endif // HPL_ANDROID_TEXTURE_H
