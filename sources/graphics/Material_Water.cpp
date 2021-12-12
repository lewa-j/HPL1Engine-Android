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
#include "graphics/Material_Water.h"
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

	class cFogWaterProgramSetup : public iMaterialProgramSetup
	{
	public:
		void Setup(iGpuProgram *apProgram,cRenderSettings* apRenderSettings)
		{
			apProgram->SetFloat("fogStart",apRenderSettings->mfFogStart);
			apProgram->SetFloat("fogEnd",apRenderSettings->mfFogEnd);
			//apProgram->SetFloat("timeCount",mfTime);
			//Log("Setting %f\n",mfTime);
		}
		float mfTime;
	};

	static cFogWaterProgramSetup gFogWaterProgramSetup;

	//-----------------------------------------------------------------------

	class cWaterProgramSetup : public iMaterialProgramSetup
	{
	public:
		void Setup(iGpuProgram *apProgram,cRenderSettings* apRenderSettings)
		{
			apProgram->SetFloat("timeCount",mfTime);
		}

		float mfTime;
	};

	static cWaterProgramSetup gWaterProgramSetup;

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_Water::cMaterial_Water(	const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture)
		: iMaterial(asName, apGraphics, apResources, apType, aPicture)
	{
		mbIsTransperant = true;
		mbIsGlowing= false;
		mbUsesLights = false;

		mpFogProg = apGraphics->CreateGpuProgramFromShaders("Water_Fog", "Water_Fog.vert", "Diffuse_Color.frag");

		mpRefractProg = apGraphics->CreateGpuProgramFromShaders("refract_water", "refract_water.vert", "refract_water.frag");

		iGpuProgram *pProg = apGraphics->CreateGpuProgramFromShaders("Water_Diffuse", "Water_Diffuse.vert", "Diffuse_Color.frag");
		SetProgram(pProg, 0);

		mfTime = 0;
	}

	//-----------------------------------------------------------------------

	cMaterial_Water::~cMaterial_Water()
	{
		if (mpFogProg) mpType->DestroyProgram(this, 0, mpFogProg);
		if (mpRefractProg) mpType->DestroyProgram(this, 0, mpRefractProg);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cMaterial_Water::Update(float afTimeStep)
	{
		mfTime += afTimeStep;
		gWaterProgramSetup.mfTime = mfTime;
		gFogWaterProgramSetup.mfTime = mfTime;
	}

	//-----------------------------------------------------------------------

	iGpuProgram* cMaterial_Water::GetProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(mpRenderSettings->mbFogActive)
			return mpFogProg;
		else
			return mpProgram[0];
	}

	iMaterialProgramSetup* cMaterial_Water::GetVertexProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(mpRenderSettings->mbFogActive)
			return &gFogWaterProgramSetup;
		else
			return &gWaterProgramSetup;
	}

	bool cMaterial_Water::VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return false;
	}

	bool cMaterial_Water::VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return false;
	}

	eMaterialAlphaMode cMaterial_Water::GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialAlphaMode_Solid;
	}

	eMaterialBlendMode cMaterial_Water::GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialBlendMode_Mul;//Add;
	}

	eMaterialChannelMode cMaterial_Water::GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialChannelMode_RGBA;
	}

	//-----------------------------------------------------------------------

	iTexture* cMaterial_Water::GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(mpRenderSettings->mbFogActive)
		{
			if(alUnit == 0)
				return mvTexture[eMaterialTexture_Diffuse];
			else if(alUnit == 1)
				return mpRenderer3D->GetFogAddTexture();
		}
		else
		{
			if(alUnit == 0)
				return mvTexture[eMaterialTexture_Diffuse];
		}

		return NULL;
	}

	eMaterialBlendMode cMaterial_Water::GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialBlendMode_Mul;
	}

	//-----------------------------------------------------------------------

	bool cMaterial_Water::UsesType(eMaterialRenderType aType)
	{
		if(aType == eMaterialRenderType_Diffuse) return true;
		return false;
	}

	//-----------------------------------------------------------------------

	tTextureTypeList cMaterial_Water::GetTextureTypes()
	{
		tTextureTypeList vTypes;
		vTypes.push_back(cTextureType("",eMaterialTexture_Diffuse));
		vTypes.push_back(cTextureType("",eMaterialTexture_Specular));

		if(mpRefractProg)
			vTypes.push_back(cTextureType("",eMaterialTexture_Refraction));

		return vTypes;
	}

	//-----------------------------------------------------------------------
}
