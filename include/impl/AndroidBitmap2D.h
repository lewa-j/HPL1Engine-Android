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
#ifndef HPL_ANDROID_BITMAP2D_H
#define HPL_ANDROID_BITMAP2D_H

#include "graphics/Bitmap2D.h"

namespace hpl
{
	class cAndroidBitmap2D : public iBitmap2D
	{
	public:
		cAndroidBitmap2D(iPixelFormat *apPxlFmt);
		cAndroidBitmap2D(uint8_t *data,int w,int h,int c,const tString& asType);
		~cAndroidBitmap2D();

		bool SaveToFile(const tString& asFile);

		void DrawToBitmap(iBitmap2D *apBmp, const cVector2l &avDestPos);
		bool HasAlpha();
		/**
		 * \todo Support 16 bit format aswell.
		 * \param avSize
		 * \param alBpp
		 * \return
		 */
		bool Create(cVector2l avSize, unsigned int alBpp);

		void FillRect(const cRect2l &aRect, const cColor &aColor);

		void* GetRawData(){return mpData;}
		int GetNumChannels(){return mChannels;}

		tString msType;
	private:
		uint8_t *mpData;
		int mChannels;
	};
}
#endif // HPL_ANDROID_BITMAP2D_H
