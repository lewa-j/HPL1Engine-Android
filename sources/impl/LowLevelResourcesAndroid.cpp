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
#include "impl/LowLevelResourcesAndroid.h"
#include "impl/Platform.h"
#include "impl/MeshLoaderCollada.h"
#include "resources/MeshLoaderHandler.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace hpl
{
	cLowLevelResourcesAndroid::cLowLevelResourcesAndroid(cLowLevelGraphicsAndroid *apLowLevelGraphics)
	{
		mvImageFormats[0] = "BMP";
		mvImageFormats[1] = "GIF";
		mvImageFormats[2] = "JPEG";
		mvImageFormats[3] = "JPG";
		mvImageFormats[4] = "PNG";
		mvImageFormats[5] = "TGA";
		mvImageFormats[6] = "PSD";
		mvImageFormats[7] = "HDR";
		mvImageFormats[8] = "PIC";
		mvImageFormats[9] = "PNM";
		mvImageFormats[10] = "";

		mpLowLevelGraphics = apLowLevelGraphics;
	}
	
	cLowLevelResourcesAndroid::~cLowLevelResourcesAndroid()
	{
		
	}
	
	void cLowLevelResourcesAndroid::FindFilesInDir(tWStringList &alstStrings, tWString asDir, tWString asMask)
	{
		Platform::FindFileInDir(alstStrings, asDir,asMask);
	}

	void cLowLevelResourcesAndroid::GetSupportedImageFormats(tStringList &alstFormats)
	{
		int lPos = 0;

		while(mvImageFormats[lPos]!="")
		{
			alstFormats.push_back(mvImageFormats[lPos]);
			lPos++;
		}
	}
	
	iBitmap2D* cLowLevelResourcesAndroid::LoadBitmap2D(tString asFilePath, tString asType)
	{
		tString tType;
		if(asType != "")
			asFilePath = cString::SetFileExt(asFilePath, asType);

		tType = cString::GetFileExt(asFilePath);
		
		int x,y,n;
		unsigned char *data = stbi_load(asFilePath.c_str(), &x, &y, &n, 0);
		if(!data){
			return nullptr;
		}
		
		iBitmap2D* pBmp = mpLowLevelGraphics->CreateBitmap2DFromData(data,x,y,n,tType);
		pBmp->SetPath(asFilePath);
		
		stbi_image_free(data);
		
		return pBmp;
	}

	void cLowLevelResourcesAndroid::AddMeshLoaders(cMeshLoaderHandler* apHandler)
	{
		//apHandler->AddLoader(hplNew( cMeshLoaderMSH,(mpLowLevelGraphics)));
		apHandler->AddLoader(hplNew( cMeshLoaderCollada,(mpLowLevelGraphics)));
	}
	
	void cLowLevelResourcesAndroid::AddVideoLoaders(cVideoManager* apManager)
	{
		#ifdef INCLUDE_THORA
		//apManager->AddVideoLoader(hplNew( cVideoStreamTheora_Loader,()));
		#endif
	}
}

