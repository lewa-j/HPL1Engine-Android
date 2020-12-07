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
#include "impl/AndroidBitmap2D.h"

namespace hpl
{
	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cAndroidBitmap2D::cAndroidBitmap2D(iPixelFormat *apPxlFmt) : iBitmap2D("Android",apPxlFmt)
	{
		mpData = nullptr;
		mlHeight = 0;
		mlWidth = 0;
		mChannels = 0;
	}

	cAndroidBitmap2D::cAndroidBitmap2D(uint8_t *data,int w,int h,int c,const tString& asType)
	 : iBitmap2D("Android",nullptr)
	{
		mpData = (uint8_t*)hplMalloc(w*h*c);
		memcpy(mpData, data, w*h*c);
		mlHeight = h;
		mlWidth = w;
		mChannels = c;

		msType = asType;
	}
	
	cAndroidBitmap2D::~cAndroidBitmap2D()
	{
		if(mpData)
			hplFree(mpData);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cAndroidBitmap2D::SaveToFile(const tString& asFile)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	void cAndroidBitmap2D::DrawToBitmap(iBitmap2D *apDest, const cVector2l &avDestPos)
	{
		cAndroidBitmap2D* pBmp = static_cast<cAndroidBitmap2D*>(apDest);

		int lDestSize = pBmp->GetNumChannels();
		int lSrcSize = GetNumChannels();

		unsigned char* destBuffer = (unsigned char*)pBmp->GetRawData();
		unsigned char* srcBuffer = (unsigned char*)GetRawData();


		for(unsigned int y=0;y<mlHeight;y++)
			for(unsigned int x=0;x<mlWidth;x++)
			{
				unsigned char* destPix = &destBuffer[((y + avDestPos.y)*pBmp->GetWidth() +
													(x + avDestPos.x)) * lDestSize];
				unsigned char* srcPix = &srcBuffer[(y*mlWidth + x) * lSrcSize];

				destPix[0] = srcPix[0];
				destPix[1] = srcPix[1];
				destPix[2] = srcPix[2];
				if(lDestSize==4 && lSrcSize==4)
					destPix[3] = srcPix[3];
			}
	}

	//-----------------------------------------------------------------------

	void cAndroidBitmap2D::FillRect(const cRect2l &aRect, const cColor &aColor)
	{
		cRect2l rect;
		rect.x = aRect.x;
		rect.y = aRect.y;
		rect.w = aRect.w<=0?mlWidth:aRect.w;
		rect.h = aRect.h<=0?mlHeight:aRect.h;

		cColor col((int)(aColor.r*255.0f),(int)(aColor.g*255.0f),
										(int)(aColor.b*255.0f),(int)(aColor.a*255.0f));
		int lDestSize = GetNumChannels();
		unsigned char* destBuffer = (unsigned char*)GetRawData();

		for(unsigned int y=rect.y; y<rect.y+rect.h; y++)
			for(unsigned int x=rect.x; x<rect.x+rect.w; x++)
			{
				unsigned char* destPix = &destBuffer[(y*GetWidth() + x) * lDestSize];

				destPix[0] = col.r;
				destPix[1] = col.g;
				destPix[2] = col.b;
				if(lDestSize==4)
					destPix[3] = col.a;
			}
	}

	//-----------------------------------------------------------------------

	bool cAndroidBitmap2D::HasAlpha()
	{
		return mChannels == 4;
	}

	//-----------------------------------------------------------------------

	bool cAndroidBitmap2D::Create(cVector2l avSize, unsigned int alBpp)
	{
		mlWidth = avSize.x;
		mlHeight = avSize.y;
		mChannels = 4;
		mpData = (uint8_t*)hplMalloc(mlWidth*mlHeight*mChannels);

		return true;
	}

	//-----------------------------------------------------------------------
}
