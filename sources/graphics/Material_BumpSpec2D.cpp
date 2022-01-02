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
#include "graphics/Material_BumpSpec2D.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer2D.h"
#include "scene/Light.h"
#include "scene/Camera.h"
#include "graphics/GPUProgram.h"


namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_BumpSpec2D::cMaterial_BumpSpec2D(const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture)
	: iMaterial(asName, apGraphics, apResources, apType, aPicture)
	{
		mbIsTransperant = false;
		mbIsGlowing= false;
		mbUsesLights = true;

		mbHasSpecular = true;

		mType = eMaterialType_BumpSpec;

		if(mbHasSpecular)
		{
			//load the program
			iGpuProgram* pProg = apGraphics->CreateGpuProgramFromShaders("BumpSpec2D_Light", "BumpSpec2D_Light.vert", "BumpSpec2D_Light.frag");
			SetProgram(pProg,0);
		}
		else //Just use normal bump without specular
		{
			//load the program
			iGpuProgram* pProg = apGraphics->CreateGpuProgramFromShaders("Bump2D_Light", "Bump2D_Light.vert", "Bump2D_Light.frag");
			SetProgram(pProg,0);
		}

	}

	//-----------------------------------------------------------------------

	cMaterial_BumpSpec2D::~cMaterial_BumpSpec2D()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cMaterial_BumpSpec2D::Compile()
	{

	}

	//-----------------------------------------------------------------------

	bool cMaterial_BumpSpec2D::StartRendering(eMaterialRenderType aType,iCamera* apCam,iLight *apLight)
	{
		if(aType == eMaterialRenderType_Z)
		{
			mpLowLevelGraphics->SetBlendActive(false);
			mpLowLevelGraphics->SetTexture(0, GetTexture(eMaterialTexture_Diffuse));
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_AlphaFunc,eTextureFunc_Replace);
			mpLowLevelGraphics->SetAlphaTestActive(true);
			mpLowLevelGraphics->SetAlphaTestFunc(eAlphaTestFunc::GreaterOrEqual, 0.6f);
		}
		else if(aType == eMaterialRenderType_Light)
		{
			cVector3f vLightPos = apLight->GetLightPosition();

			cVector3f vEyePos;
			if(apCam!=NULL)
			{
				vEyePos = apCam->GetEyePosition();
			}

			mpLowLevelGraphics->SetBlendActive(true);
			mpLowLevelGraphics->SetBlendFunc(eBlendFunc::One,eBlendFunc::One);

			mpLowLevelGraphics->SetTexture(0, GetTexture(eMaterialTexture_NMap));
			mpLowLevelGraphics->SetTexture(1, mpRenderer->GetLightMap(0));

			mpProgram[0]->Bind();
			mpProgram[0]->SetMatrixf("worldViewProj",
				eGpuProgramMatrix::ViewProjection,
				eGpuProgramMatrixOp::Identity);

			mpProgram[0]->SetVec3f("LightPos",vLightPos.x, vLightPos.y,vLightPos.z);

			if(mbHasSpecular)
				mpProgram[0]->SetVec3f("EyePos",vEyePos.x, vEyePos.y,vEyePos.z);

			mpProgram[0]->SetFloat("LightRadius",apLight->GetFarAttenuation());
			mpProgram[0]->SetVec4f("LightColor",apLight->GetDiffuseColor().r,
									apLight->GetDiffuseColor().g, apLight->GetDiffuseColor().b,
									apLight->GetDiffuseColor().a);
		}
		else if(aType == eMaterialRenderType_Diffuse)
		{
			mpLowLevelGraphics->SetBlendActive(true);
			mpLowLevelGraphics->SetBlendFunc(eBlendFunc::DestColor,eBlendFunc::DestAlpha);

			mpLowLevelGraphics->SetTexture(0, GetTexture(eMaterialTexture_Diffuse));
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc,eTextureFunc_Add);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorOp1, eTextureOp_OneMinusAlpha);
		}

		return true;
	}
	//-----------------------------------------------------------------------

	void cMaterial_BumpSpec2D::EndRendering(eMaterialRenderType aType)
	{
		if(aType == eMaterialRenderType_Z)
		{
			mpLowLevelGraphics->SetAlphaTestActive(false);
			mpLowLevelGraphics->SetTexture(0, NULL);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_AlphaFunc,eTextureFunc_Modulate);
		}
		if(aType == eMaterialRenderType_Light)
		{
			mpLowLevelGraphics->SetTexture(0, NULL);
			mpLowLevelGraphics->SetTexture(1, NULL);

			mpProgram[0]->UnBind();
		}
		else if(aType == eMaterialRenderType_Diffuse)
		{
			mpLowLevelGraphics->SetTexture(0, NULL);
			mpLowLevelGraphics->SetBlendActive(false);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorFunc,eTextureFunc_Modulate);
			mpLowLevelGraphics->SetTextureEnv(eTextureParam_ColorOp1, eTextureOp_Color);
		}

		mlPassCount=0;
	}

	//-----------------------------------------------------------------------

	tVtxBatchFlag cMaterial_BumpSpec2D::GetBatchFlags(eMaterialRenderType aType)
	{
		if(aType == eMaterialRenderType_Light)
		{
			return eVtxBatchFlag_Position |	eVtxBatchFlag_Texture0 |
				eVtxBatchFlag_Normal | eVtxBatchFlag_Color0;
		}

		return eVtxBatchFlag_Position |	eVtxBatchFlag_Texture0 	| eVtxBatchFlag_Color0;
	}
	//-----------------------------------------------------------------------

	bool cMaterial_BumpSpec2D::NextPass(eMaterialRenderType aType)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	bool cMaterial_BumpSpec2D::HasMultiplePasses(eMaterialRenderType aType)
	{
		return false;
	}

	//-----------------------------------------------------------------------

	eMaterialType cMaterial_BumpSpec2D::GetType(eMaterialRenderType aType)
	{
		if(aType == eMaterialRenderType_Z) return eMaterialType_DiffuseAlpha;
		return mType;
	}

	//-----------------------------------------------------------------------

	void cMaterial_BumpSpec2D::EditVertexes(eMaterialRenderType aType, iCamera* apCam, iLight *pLight,
		tVertexVec *apVtxVec,cVector3f *apTransform, unsigned int alIndexAdd)
	{

	}

	//-----------------------------------------------------------------------

	tTextureTypeList cMaterial_BumpSpec2D::GetTextureTypes()
	{
		tTextureTypeList lstTypes;
		lstTypes.push_back(cTextureType("", eMaterialTexture_Diffuse));
		lstTypes.push_back(cTextureType("_bump", eMaterialTexture_NMap));
		return lstTypes;
	}

	//-----------------------------------------------------------------------
}
