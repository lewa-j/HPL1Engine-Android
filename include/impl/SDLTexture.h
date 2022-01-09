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
#ifndef HPL_SDL_TEXTURE_H
#define HPL_SDL_TEXTURE_H

#include "impl/TextureGL.h"
#include "impl/PBuffer.h"
#include "impl/LowLevelGraphicsSDL.h"

namespace hpl {

	class cSDLTexture : public cTextureGL
	{
	public:
		cSDLTexture(const tString &asName, iPixelFormat *apPxlFmt, iLowLevelGraphics *apLowLevelGraphics,
			eTextureType aType, bool abUseMipMaps, eTextureTarget aTarget, bool abCompress = false);
		~cSDLTexture();

		bool Create(unsigned int alWidth, unsigned int alHeight, cColor aCol) override;

		/// SDL  Specific ///////////
		cPBuffer *GetPBuffer() { return mpPBuffer; }

	protected:
		void GetSettings(iBitmap2D *apSrc, int &alChannels, GLenum &aInternalFormat, GLenum &aFormat) override;

		cLowLevelGraphicsSDL *mpGfxSDL = nullptr;
		cPBuffer *mpPBuffer = nullptr;
	};

};
#endif // HPL_SDL_TEXTURE_H
