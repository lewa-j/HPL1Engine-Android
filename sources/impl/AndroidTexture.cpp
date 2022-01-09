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
#include "impl/GLHelpers.h"

#include "math/Math.h"

namespace hpl
{

	cAndroidTexture::cAndroidTexture(const tString &asName,iPixelFormat *apPxlFmt,iLowLevelGraphics* apLowLevelGraphics,
				eTextureType aType, bool abUseMipMaps, eTextureTarget aTarget,
				bool abCompress)
	: cTextureGL(asName,apPxlFmt,apLowLevelGraphics,aType,abUseMipMaps, aTarget, abCompress)
	{
	}

	cAndroidTexture::~cAndroidTexture()
	{
	}

	bool cAndroidTexture::Create(unsigned int alWidth, unsigned int alHeight, cColor aCol)
	{
		Warning("Unimplemented cAndroidTexture::Create\n");
		return true;
	}

	void cAndroidTexture::GetSettings(iBitmap2D* apSrc, int &alChannels, GLenum &aInternalFormat, GLenum &aFormat)
	{
		alChannels = apSrc->GetNumChannels();
		switch (alChannels)
		{
		case 1: aFormat = GL_ALPHA; break;
		case 2: aFormat = GL_LUMINANCE_ALPHA; break;
		case 3: aFormat = GL_RGB; break;
		case 4: aFormat = GL_RGBA; break;
		}
		aInternalFormat = aFormat;
	}

}
