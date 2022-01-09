/*
 * Copyright (C) 2020-2022 - lewa_j
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

#include "impl/TextureGL.h"
#include "impl/LowLevelGraphicsAndroid.h"

namespace hpl
{
	class cAndroidTexture : public cTextureGL
	{
	public:
		cAndroidTexture(const tString &asName,iPixelFormat *apPxlFmt,iLowLevelGraphics* apLowLevelGraphics,
					eTextureType aType, bool abUseMipMaps, eTextureTarget aTarget,
					bool abCompress=false);
		~cAndroidTexture();

		bool Create(unsigned int alWidth, unsigned int alHeight, cColor aCol) override;

	protected:
		void GetSettings(iBitmap2D* apSrc, int &alChannels, GLenum &aInternalFormat, GLenum &aFormat) override;

		cLowLevelGraphicsAndroid* mpGfx = nullptr;
		//cPBuffer *mpPBuffer = nullptr;
	};

};

