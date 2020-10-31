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
#ifndef HPL_LOWLEVELRESOURCES_ANDROID_H
#define HPL_LOWLEVELRESOURCES_ANDROID_H

#include "resources/LowLevelResources.h"
#include "impl/LowLevelGraphicsAndroid.h"
#include "system/SystemTypes.h"

namespace hpl
{
	class cLowLevelResourcesAndroid : public iLowLevelResources
	{
	public:
		cLowLevelResourcesAndroid(cLowLevelGraphicsAndroid *apLowLevelGraphics);
		~cLowLevelResourcesAndroid();

		void FindFilesInDir(tWStringList &alstStrings, tWString asDir, tWString asMask);

		void GetSupportedImageFormats(tStringList &alstFormats);
		iBitmap2D* LoadBitmap2D(tString asFilePath, tString asType = "");

		void AddMeshLoaders(cMeshLoaderHandler* apHandler);
		void AddVideoLoaders(cVideoManager* apManager);

	private:
		tString mvImageFormats[30];
		cLowLevelGraphicsAndroid *mpLowLevelGraphics;
	};
};
#endif // HPL_LOWLEVELRESOURCES_ANDROID_H
