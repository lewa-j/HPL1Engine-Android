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
#include "impl/SDLTexture.h"
#include "impl/SDLBitmap2D.h"
#include "impl/GLHelpers.h"
#include "math/Math.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSDLTexture::cSDLTexture(const tString &asName, iPixelFormat *apPxlFmt, iLowLevelGraphics *apLowLevelGraphics,
		eTextureType aType, bool abUseMipMaps, eTextureTarget aTarget,
		bool abCompress)
		: cTextureGL(asName, apPxlFmt, apLowLevelGraphics, aType, abUseMipMaps, aTarget, abCompress)
	{
		mpPBuffer = nullptr;

		if (aType == eTextureType_RenderTarget)
		{
			mpPBuffer = hplNew(cPBuffer, (mpLowLevelGraphics, true));
		}

		mpGfxSDL = static_cast<cLowLevelGraphicsSDL *>(mpLowLevelGraphics);
	}

	cSDLTexture::~cSDLTexture()
	{
		if (mpPBuffer)
			hplDelete(mpPBuffer);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cSDLTexture::Create(unsigned int alWidth, unsigned int alHeight, cColor aCol)
	{
		//Generate handles
		mvTextureHandles.resize(1);
		glGenTextures(1, (GLuint *)&mvTextureHandles[0]);

		if (mType != eTextureType_RenderTarget)
		{
			return false;
		}

		if (!mpPBuffer->Init(alWidth, alHeight, aCol)) {
			return false;
		}

		mlWidth = alWidth;
		mlHeight = alHeight;

		if ((!cMath::IsPow2(mlHeight) || !cMath::IsPow2(mlWidth)) && mTarget != eTextureTarget_Rect)
		{
			Warning("Texture '%s' does not have a pow2 size!\n", msName.c_str());
		}

		GLenum GLTarget = GetGLTextureTargetEnum(mTarget);

		glEnable(GLTarget);
		glBindTexture(GLTarget, mvTextureHandles[0]);
		if (mbUseMipMaps && mTarget != eTextureTarget_Rect) {
			if (mFilter == eTextureFilter_Bilinear)
				glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			else
				glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else {
			glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GLTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GLTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glDisable(GLTarget);

		mbContainsData = true;

		return true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cSDLTexture::GetSettings(iBitmap2D* apSrc, int &alChannels, GLenum &aInternalFormat, GLenum &aFormat)
	{
		cSDLBitmap2D *pSrc = static_cast<cSDLBitmap2D *>(apSrc);
		SDL_Surface *pSurface =  pSrc->GetSurface();
		alChannels = pSurface->format->BytesPerPixel;
		aFormat = GL_RGBA;
		aInternalFormat = GL_RGBA;

		tString sType = cString::ToLowerCase(pSrc->msType);

		if(alChannels==4)
		{
			if(sType == "tga")
			{
				aFormat = GL_BGRA;
			}
			else
			{
				aFormat = GL_RGBA;
			}

		}
		if(alChannels==3)
		{
			aInternalFormat = GL_RGB;
			if(sType == "tga")
			{
				aFormat = GL_BGR;
			}
			else
			{
				aFormat = GL_RGB;
			}
		}
		if(alChannels==1)
		{
			aInternalFormat = GL_ALPHA;
			aFormat = GL_ALPHA;
		}
	}

}
