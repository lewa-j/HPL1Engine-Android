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
#include "graphics/Material.h"
#include "graphics/MaterialType.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer2D.h"
#include "graphics/Renderer3D.h"
#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "resources/TextureManager.h"
#include "resources/ImageManager.h"
#include "resources/GpuShaderManager.h"
#include "graphics/GPUProgram.h"
#include "scene/Camera.h"
#include "system/String.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	eMaterialQuality iMaterial::mQuality = eMaterialQuality_High;

	//-----------------------------------------------------------------------

	iMaterial::iMaterial(const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture)
		: iResourceBase(asName, 0)
	{
		mpType = apType;

		if(aPicture==eMaterialPicture_Image){
			mvImage.resize(eMaterialTexture_LastEnum);
			mvImage.assign(mvImage.size(), NULL);
		}
		else {
			mvTexture.resize(eMaterialTexture_LastEnum);
			mvTexture.assign(mvTexture.size(), NULL);
		}

		mType = eMaterialType_Null;
		mPicture = aPicture;

		mpLowLevelGraphics = apGraphics->GetLowLevel();
		mpImageManager = apResources->GetImageManager();
		mpTextureManager = apResources->GetTextureManager();
		mpRenderer = apGraphics->GetRenderer2D();
		mpRenderer3D = apGraphics->GetRenderer3D();
		mpRenderSettings = mpRenderer3D->GetRenderSettings();
		mpShaderManager = apResources->GetGpuShaderManager();

		mbUsesLights = false;
		mbIsTransperant = false;
		mbIsGlowing = false;
		mbHasAlpha = false;
		mbDepthTest = true;
		mfValue = 1;

		for (int j = 0; j < kMaxProgramNum; j++)
			mpProgram[j] = nullptr;

		mlPassCount=0;

		mlId = -1;
	}

	iMaterial::~iMaterial()
	{
		int i;

		for (i = 0; i < (int)mvImage.size(); i++) {
			if (mvImage[i])
				mpImageManager->Destroy(mvImage[i]);
		}
		for (i = 0; i < (int)mvTexture.size(); i++) {
			if (mvTexture[i])
				mpTextureManager->Destroy(mvTexture[i]);
		}

		for (int j = 0; j < kMaxProgramNum; j++) {
			if (mpProgram[j])
				mpType->DestroyProgram(this, j, mpProgram[j]);
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	/*void iMaterial::Destroy()
	{

	}*/

	//-----------------------------------------------------------------------

	iTexture* iMaterial::GetTexture(eMaterialTexture aType)
	{
		if(mPicture==eMaterialPicture_Image){
			if(mvImage[aType]==NULL){
				Log("2: %d\n", aType);return NULL;
			}
			return mvImage[aType]->GetTexture();
		}
		else
		{
			return mvTexture[aType];
		}
	}

	//-----------------------------------------------------------------------

	cRect2f iMaterial::GetTextureOffset(eMaterialTexture aType)
	{
		cRect2f SizeRect;

		if(mPicture==eMaterialPicture_Image)
		{
			tVertexVec VtxVec = mvImage[aType]->GetVertexVecCopy(0,0);

			SizeRect.x = VtxVec[0].tex.x;
			SizeRect.y = VtxVec[0].tex.y;
			SizeRect.w = VtxVec[2].tex.x - VtxVec[0].tex.x;
			SizeRect.h = VtxVec[2].tex.y - VtxVec[0].tex.y;
		}
		else
		{
			SizeRect.x=0;SizeRect.y=0;
			SizeRect.w = 1;//(float) mvTexture[aType]->GetWidth();
			SizeRect.h = 1;//(float) mvTexture[aType]->GetHeight();
		}

		return SizeRect;
	}

	//-----------------------------------------------------------------------

	iMaterialType::iMaterialType(cGraphics *apGraphics)
	{
		mpGraphics = apGraphics;
	}

	void iMaterialType::DestroyProgram(iMaterial *apMaterial, int i, iGpuProgram *apProgram)
	{
		mpGraphics->DestroyGpuProgram(apProgram);
	}

}
