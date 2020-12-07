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
#ifndef HPL_ANDROID_FONTDATA_H
#define HPL_ANDROID_FONTDATA_H

#include "graphics/FontData.h"

namespace hpl
{
	class cAndroidFontData : public iFontData
	{
	public:
		cAndroidFontData(const tString &asName, iLowLevelGraphics* apLowLevelGraphics);
		~cAndroidFontData();

		bool CreateFromFontFile(const tString &asFileName, int alSize,unsigned short alFirstChar,
								unsigned short alLastChar);
		bool CreateFromBitmapFile(const tString &asFileName);

	private:
		//cGlyph* RenderGlyph(TTF_Font* apFont,unsigned short aChar, int alFontSize);
	};

}
#endif // HPL_ANDROID_FONTDATA_H

