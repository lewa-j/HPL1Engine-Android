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

#include "impl/AndroidTexture.h"

namespace hpl
{
cAndroidTexture::cAndroidTexture(const tString &asName,iPixelFormat *apPxlFmt,iLowLevelGraphics* apLowLevelGraphics,
			eTextureType aType, bool abUseMipMaps, eTextureTarget aTarget,
			bool abCompress)
		: iTexture(asName,"GL",apPxlFmt,apLowLevelGraphics,aType,abUseMipMaps, aTarget, abCompress)
{
	
}
cAndroidTexture::~cAndroidTexture()
{
	
}

bool cAndroidTexture::CreateFromBitmap(iBitmap2D* pBmp)
{
	return true;
}

bool cAndroidTexture::CreateAnimFromBitmapVec(tBitmap2DVec *avBitmaps)
{
	return true;
}

bool cAndroidTexture::CreateCubeFromBitmapVec(tBitmap2DVec *avBitmaps)
{
	return true;
}
bool cAndroidTexture::Create(unsigned int alWidth, unsigned int alHeight, cColor aCol)
{
	return true;
}

bool cAndroidTexture::CreateFromArray(unsigned char *apPixelData, int alChannels, const cVector3l &avSize)
{
	return true;
}
void cAndroidTexture::SetPixels2D(	int alLevel, const cVector2l& avOffset, const cVector2l& avSize,
					eColorDataFormat aDataFormat, void *apPixelData)
{
	
}

float cAndroidTexture::GetGamma()
{
	return 0;
}
void cAndroidTexture::SetGamma(float afGamma)
{
	
}
int cAndroidTexture::GetHandle()
{
	return (int) mvTextureHandles[0];
}

void cAndroidTexture::SetFilter(eTextureFilter aFilter)
{
	
}
void cAndroidTexture::SetAnisotropyDegree(float afX)
{
	
}

void cAndroidTexture::SetWrapS(eTextureWrap aMode)
{
	
}
void cAndroidTexture::SetWrapT(eTextureWrap aMode)
{
	
}
void cAndroidTexture::SetWrapR(eTextureWrap aMode)
{
	
}

void cAndroidTexture::Update(float afTimeStep)
{
	
}

bool cAndroidTexture::HasAnimation()
{
	return false;
}
void cAndroidTexture::NextFrame()
{
	
}
void cAndroidTexture::PrevFrame()
{
	
}
float cAndroidTexture::GetT()
{
	return 0;
}
float cAndroidTexture::GetTimeCount()
{
	return 0;
}
void cAndroidTexture::SetTimeCount(float afX)
{
	
}
int cAndroidTexture::GetCurrentLowlevelHandle()
{
	return 0;
}

unsigned int cAndroidTexture::GetTextureHandle()
{
	return 0;
}
}
