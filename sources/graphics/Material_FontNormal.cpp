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
#include "graphics/Material_FontNormal.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_FontNormal::cMaterial_FontNormal(const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture)
	: iMaterial(asName, apGraphics, apResources, apType, aPicture)
	{
		mbIsTransperant = true;
		mbIsGlowing= true;
		mType = eMaterialType_FontNormal;
	}

	//-----------------------------------------------------------------------

	cMaterial_FontNormal::~cMaterial_FontNormal()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cMaterial_FontNormal::Compile()
	{

	}

	//-----------------------------------------------------------------------

	bool cMaterial_FontNormal::StartRendering(eMaterialRenderType aType,iCamera* apCam,iLight *pLight)
	{
		if(aType == eMaterialRenderType_Diffuse)
		{
			mpLowLevelGraphics->SetBlendActive(true);
			mpLowLevelGraphics->SetBlendFunc(eBlendFunc_SrcAlpha,eBlendFunc_OneMinusSrcAlpha);

			mpLowLevelGraphics->SetTexture(0, GetTexture(eMaterialTexture_Diffuse));

			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	void cMaterial_FontNormal::EndRendering(eMaterialRenderType aType)
	{
		if(aType == eMaterialRenderType_Diffuse)
		{
			//mpLowLevelGraphics->SetTextureParam(eTextureParam_AlphaSource0,eTextureSource_Texture);
		}

	}

	//-----------------------------------------------------------------------

	tVtxBatchFlag cMaterial_FontNormal::GetBatchFlags(eMaterialRenderType aType)
	{
		return eVtxBatchFlag_Position |	eVtxBatchFlag_Texture0 | eVtxBatchFlag_Color0;
	}

	//-----------------------------------------------------------------------

	bool cMaterial_FontNormal::NextPass(eMaterialRenderType aType)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	bool cMaterial_FontNormal::HasMultiplePasses(eMaterialRenderType aType)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	eMaterialType cMaterial_FontNormal::GetType(eMaterialRenderType aType)
	{
		return mType;
	}

	//-----------------------------------------------------------------------

	void cMaterial_FontNormal::EditVertexes(eMaterialRenderType aType, iCamera* apCam, iLight *pLight,
		tVertexVec *apVtxVec,cVector3f *apTransform,unsigned int alIndexAdd)
	{

	}

	//-----------------------------------------------------------------------
}
