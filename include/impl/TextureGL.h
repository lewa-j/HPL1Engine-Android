/*
 * Copyright (C) 2022 - lewa_j
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
#pragma once

#include "graphics/Texture.h"
#include "impl/platform/gl.h"

namespace hpl
{

	class cTextureGL : public iTexture
	{
	public:
		cTextureGL(const tString &asName, iPixelFormat *apPxlFmt, iLowLevelGraphics *apLowLevelGraphics,
			eTextureType aType, bool abUseMipMaps, eTextureTarget aTarget, bool abCompress = false);
		virtual ~cTextureGL();

		virtual bool CreateFromBitmap(iBitmap2D *pBmp) override;

		virtual bool CreateAnimFromBitmapVec(tBitmap2DVec *avBitmaps) override;

		virtual bool CreateCubeFromBitmapVec(tBitmap2DVec *avBitmaps) override;
		//virtual bool Create(unsigned int alWidth, unsigned int alHeight, cColor aCol) override;

		virtual bool CreateFromArray(unsigned char *apPixelData, int alChannels, const cVector3l &avSize) override;

		virtual void SetPixels2D(int alLevel, const cVector2l &avOffset, const cVector2l &avSize,
			eColorDataFormat aDataFormat, void *apPixelData) override;


		virtual float GetGamma() override { return 0; }
		virtual void SetGamma(float afGamma) override {}
		virtual int GetHandle() override { return (int)mvTextureHandles[0]; }

		virtual void SetFilter(eTextureFilter aFilter) override;
		virtual void SetAnisotropyDegree(float afX) override;

		virtual void SetWrapS(eTextureWrap aMode) override;
		virtual void SetWrapT(eTextureWrap aMode) override;
		virtual void SetWrapR(eTextureWrap aMode) override;

		virtual void Update(float afTimeStep) override;

		virtual bool HasAnimation() override;
		virtual void NextFrame() override;
		virtual void PrevFrame() override;
		virtual float GetT() override;
		virtual float GetTimeCount() override;
		virtual void SetTimeCount(float afX) override;
		virtual int GetCurrentLowlevelHandle() override;

	protected:
		bool CreateFromBitmapToHandle(iBitmap2D *pBmp, int alHandleIdx);

		GLenum InitCreation(int alHandleIdx);
		void PostCreation(GLenum aGLTarget);

		virtual void GetSettings(iBitmap2D *apSrc, int &alChannels, GLenum &aInternalFormat, GLenum &aFormat) = 0;

		tUIntVec mvTextureHandles;
		bool mbContainsData;

		float mfTimeCount;
		int mlTextureIndex;
		float mfTimeDir;
	};

};
