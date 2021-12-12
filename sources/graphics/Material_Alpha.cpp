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
#include "graphics/Material_Alpha.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer2D.h"
#include "graphics/Renderer3D.h"
#include "scene/Light.h"
#include "scene/Camera.h"
#include "resources/TextureManager.h"
#include "graphics/GPUProgram.h"
#include "math/Math.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// VERTEX PRORGAM SETUP
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	class cFogProgramSetup : public iMaterialProgramSetup
	{
	public:
		void Setup(iGpuProgram *apProgram,cRenderSettings* apRenderSettings)
		{
			apProgram->SetFloat("fogStart",apRenderSettings->mfFogStart);
			apProgram->SetFloat("fogEnd",apRenderSettings->mfFogEnd);
		}
	};

	static cFogProgramSetup gFogProgramSetup;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_Alpha::cMaterial_Alpha(	const tString& asName, cGraphics *apGraphics, cResources *apResources, cMaterialType_Alpha *apType, eMaterialPicture aPicture)
		: iMaterial(asName, apGraphics, apResources, apType, aPicture)
	{
		mbIsTransperant = true;
		mbIsGlowing= false;
		mbUsesLights = false;

		mpFogProg = apType->mpFogProg;
	}

	//-----------------------------------------------------------------------

	cMaterial_Alpha::~cMaterial_Alpha()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iGpuProgram* cMaterial_Alpha::GetProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(mpRenderSettings->mbFogActive)
			return mpFogProg;
		else
			return NULL;
	}

	iMaterialProgramSetup* cMaterial_Alpha::GetVertexProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(mpRenderSettings->mbFogActive)
			return &gFogProgramSetup;
		else
			return NULL;
	}

	bool cMaterial_Alpha::VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return false;
	}

	bool cMaterial_Alpha::VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return false;
	}

	eMaterialAlphaMode cMaterial_Alpha::GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialAlphaMode_Solid;
	}

	eMaterialBlendMode cMaterial_Alpha::GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialBlendMode_Alpha;
	}

	eMaterialChannelMode cMaterial_Alpha::GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialChannelMode_RGBA;
	}

	//-----------------------------------------------------------------------

	iTexture* cMaterial_Alpha::GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(alUnit == 0)
			return mvTexture[eMaterialTexture_Diffuse];

		if(alUnit == 1 && mpRenderSettings->mbFogActive)
		{
			return mpRenderer3D->GetFogAlphaTexture();
		}

		return NULL;
	}

	eMaterialBlendMode cMaterial_Alpha::GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialBlendMode_Mul;
	}

	//-----------------------------------------------------------------------

	bool cMaterial_Alpha::UsesType(eMaterialRenderType aType)
	{
		if(aType == eMaterialRenderType_Diffuse) return true;
		return false;
	}

	//-----------------------------------------------------------------------

	tTextureTypeList cMaterial_Alpha::GetTextureTypes()
	{
		tTextureTypeList vTypes;
		vTypes.push_back(cTextureType("",eMaterialTexture_Diffuse));
		vTypes.push_back(cTextureType("_ref",eMaterialTexture_Refraction));
		vTypes.push_back(cTextureType("_spec",eMaterialTexture_Specular));

		return vTypes;
	}

	//-----------------------------------------------------------------------

    cMaterialType_Alpha::cMaterialType_Alpha(cGraphics *apGraphics)
		: iMaterialType(apGraphics)
    {
		mpFogProg = apGraphics->CreateGpuProgramFromShaders("cMaterial_Alpha", "Fog_Trans.vert", "Fog_Trans_Alpha.frag");
    }

	cMaterialType_Alpha::~cMaterialType_Alpha()
	{
		mpGraphics->DestroyGpuProgram(mpFogProg);
	}

	inline iMaterial *cMaterialType_Alpha::Create(const tString &asName, cGraphics *apGraphics, cResources *apResources, eMaterialPicture aPicture)
	{
		return hplNew(cMaterial_Alpha, (asName, apGraphics, apResources, this, aPicture));
	}
}
