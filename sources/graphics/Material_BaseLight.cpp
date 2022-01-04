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
#include "graphics/Material_BaseLight.h"
#include "graphics/MaterialType.h"

#include "graphics/Graphics.h"
#include "graphics/Renderer2D.h"
#include "scene/Light.h"
#include "scene/Camera.h"
#include "resources/GpuShaderManager.h"
#include "resources/TextureManager.h"
#include "graphics/GPUProgram.h"
#include "math/Math.h"
#include "system/String.h"
#include "scene/Light3DSpot.h"
#include "graphics/Renderer3D.h"
#include "scene/PortalContainer.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// FRAGMENT PRORGAM SETUP
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	class cAmbProgramSetup : public iMaterialProgramSetup
	{
	public:
		void Setup(iGpuProgram *apProgram, cRenderSettings *apRenderSettings)
		{
			if (apRenderSettings->mpSector)
				apProgram->SetColor3f("ambientColor", apRenderSettings->mAmbientColor * apRenderSettings->mpSector->GetAmbientColor());
			else
				apProgram->SetColor3f("ambientColor", apRenderSettings->mAmbientColor);
		}
	};

	static cAmbProgramSetup gAmbProgramSetup;

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iMaterial_BaseLight::iMaterial_BaseLight(const tString& asLightVertexProgram,
										const tString& asLightFragmentProgram,
		const tString &asName, cGraphics *apGraphics, cResources *apResources, cMaterialType_BaseLight *apType, eMaterialPicture aPicture)
		: iMaterial(asName, apGraphics, apResources, apType, aPicture)
	{
		mbIsTransperant = false;
		mbIsGlowing = false;
		mbUsesLights = true;
		mbUseColorSpecular = false;

		for (int i = 0; i < eBaseLightProgram_LastEnum; i++) {
			mvPrograms[i] = NULL;
		}

		////////////////////////////////////////
		//Get names for other light programs
		tString sSpotVtxShader = cString::Sub(asLightVertexProgram,0, (int)asLightVertexProgram.size() - 5) +
									"_Spot.vert";

		///////////////////////////////////////////
		//Load the light pass program
		//Point
		mvPrograms[eBaseLightProgram_Point1] = apGraphics->CreateGpuProgramFromShaders(asName + "_Point1", asLightVertexProgram, asLightFragmentProgram);

		//////////////////////////////////////////////////////
		//Check if there is enough texture units for 1 pass spot
		if(mpLowLevelGraphics->GetCaps(eGraphicCaps_MaxTextureImageUnits) > 4)
		{
			mbUsesTwoPassSpot = false;

			tString sSpotFragProgram = cString::Sub(asLightFragmentProgram,0, (int)asLightFragmentProgram.size() - 5) +
													"_Spot.frag";

			mvPrograms[eBaseLightProgram_Spot1] = apGraphics->CreateGpuProgramFromShaders(asName + "_Spot1", sSpotVtxShader, sSpotFragProgram);
		}
		else
		{
			mbUsesTwoPassSpot = true;

			tString sSpotFragProgram1 = "Diffuse_Light_Spot_pass1.frag";//cString::Sub(asLightFragmentProgram,0, (int)asLightFragmentProgram.size() - 5) +
										//			"_Spot_pass1.frag";
			tString sSpotFragProgram2 = cString::Sub(asLightFragmentProgram,0, (int)asLightFragmentProgram.size() - 5) +
													"_Spot_pass2.frag";

			mvPrograms[eBaseLightProgram_Spot1] = apGraphics->CreateGpuProgramFromShaders(asName + "_Spot1", sSpotVtxShader, sSpotFragProgram1);
			mvPrograms[eBaseLightProgram_Spot2] = apGraphics->CreateGpuProgramFromShaders(asName + "_Spot2", sSpotVtxShader, sSpotFragProgram2);
		}


		///////////////////////////////////////////
		//Load the Z pass vertex program
		iGpuProgram *pProg = apType->mpZPassP;
		SetProgram(pProg, 1);


		///////////////////////////////////////////
		//More fragment programs
		mpSimpleP = apType->mpSimpleP;
		mpAmbientP = apType->mpAmbientP;

		///////////////////////////////////////////
		//Normalization map
		mpNormalizationMap = mpTextureManager->CreateCubeMap("Normalization", false);
		mpNormalizationMap->SetWrapR(eTextureWrap_ClampToEdge);
		mpNormalizationMap->SetWrapS(eTextureWrap_ClampToEdge);
		mpNormalizationMap->SetWrapT(eTextureWrap_ClampToEdge);

		///////////////////////////////////////////
		//Negative rejection
		mpSpotNegativeRejectMap = mpTextureManager->Create2D("core_spot_negative_reject.tga",false);
		if(mpSpotNegativeRejectMap)
		{
			mpSpotNegativeRejectMap->SetWrapS(eTextureWrap_ClampToEdge);
			mpSpotNegativeRejectMap->SetWrapT(eTextureWrap_ClampToEdge);
		}

		mbUseSpecular = false;
		mbUseNormalMap = false;
	}

	//-----------------------------------------------------------------------

	iMaterial_BaseLight::~iMaterial_BaseLight()
	{
		if(mpNormalizationMap) mpTextureManager->Destroy(mpNormalizationMap);
		if(mpSpotNegativeRejectMap) mpTextureManager->Destroy(mpSpotNegativeRejectMap);

		if (mbDestroyTypeSpecifics && mpType)
			for (int i = 0; i < eBaseLightProgram_LastEnum; i++)
			{
				if(mvPrograms[i]) mpType->DestroyProgram(this, i, mvPrograms[i]);
			}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iGpuProgram* iMaterial_BaseLight::GetProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(apLight && aType == eMaterialRenderType_Light)
		{
			eBaseLightProgram program;

			///////////////
			//Spot two pass
			if (apLight->GetLightType() == eLight3DType_Spot
				&& mbUsesTwoPassSpot)
			{
				if (alPass == 0)	program = eBaseLightProgram_Spot1;
				else			program = eBaseLightProgram_Spot2;
			}
			//////////////////
			//Other
			else
			{
				if (apLight->GetLightType() == eLight3DType_Point)		program = eBaseLightProgram_Point1;
				else if (apLight->GetLightType() == eLight3DType_Spot)	program = eBaseLightProgram_Spot1;
			}

			return mvPrograms[program];
		}

		else if (aType == eMaterialRenderType_Diffuse)
		{
			return mpSimpleP;
		}
		else if (aType == eMaterialRenderType_Z)
		{
			return mpAmbientP;
		}

		return NULL;
	}

	//------------------------------------------------------------------------------------

	bool iMaterial_BaseLight::VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Light) return true;
		return false;
	}

	//------------------------------------------------------------------------------------

	bool iMaterial_BaseLight::VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Light && mbUseSpecular) return true;
		return false;
	}

	//------------------------------------------------------------------------------------

	iMaterialProgramSetup * iMaterial_BaseLight::GetFragmentProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Z)
		{
			return &gAmbProgramSetup;
		}

		return NULL;
	}

	//------------------------------------------------------------------------------------

	eMaterialAlphaMode iMaterial_BaseLight::GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Z && mbHasAlpha) return eMaterialAlphaMode_Trans;

		return eMaterialAlphaMode_Solid;
	}

	//------------------------------------------------------------------------------------

	eMaterialBlendMode iMaterial_BaseLight::GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		//////////////////////////////
		// Spot two pass
		if(aType == eMaterialRenderType_Light && apLight->GetLightType() == eLight3DType_Spot
			&& mbUsesTwoPassSpot)
		{
			if(alPass == 0)			return eMaterialBlendMode_Replace;
			else if(alPass == 1)	return eMaterialBlendMode_DestAlphaAdd;
		}

		//////////////////////////////
		// Other
		if(aType == eMaterialRenderType_Z) return eMaterialBlendMode_Replace;

		return eMaterialBlendMode_Add;
	}

	//------------------------------------------------------------------------------------

	eMaterialChannelMode iMaterial_BaseLight::GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		////////////////////////////
		// Spot two pass:
		if(aType == eMaterialRenderType_Light && apLight->GetLightType() == eLight3DType_Spot
			&& mbUsesTwoPassSpot)
		{
			if(alPass == 0) return eMaterialChannelMode_A;
		}
		//////////////////////////////
		// Other
		else if(aType == eMaterialRenderType_Z)
		{
			//return eMaterialChannelMode_Z;
			return eMaterialChannelMode_RGBA;
		}

		return eMaterialChannelMode_RGBA;
	}

	//-----------------------------------------------------------------------

	iTexture* iMaterial_BaseLight::GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Z)
		{
			//if(alUnit==0 && mbHasAlpha) return mvTexture[eMaterialTexture_Diffuse];
			if(alUnit==0) return mvTexture[eMaterialTexture_Diffuse];
			return NULL;
		}
		else if(aType == eMaterialRenderType_Diffuse)
		{
			if(alUnit==0) return mvTexture[eMaterialTexture_Illumination];
			return NULL;
		}
		else if(aType == eMaterialRenderType_Light)
		{
			//////////////////////////////////////////////
			// Two Pass Spot LIght
			if(mbUsesTwoPassSpot && apLight->GetLightType() == eLight3DType_Spot)
			{
				if(alPass == 0)
				{
					switch(alUnit)
					{
					//Falloff map
					case 0: return apLight->GetFalloffMap();

					//Negative rejection:
					case 1: return mpSpotNegativeRejectMap;
					}
				}
				else
				{
					switch(alUnit)
					{
					//Diffuse texture
					case 0: return mvTexture[eMaterialTexture_Diffuse];

					//Normalmap
					case 1: if(mbUseNormalMap) return mvTexture[eMaterialTexture_NMap];break;

					//Normalization map
					case 2: return mpNormalizationMap; break;

					//Spotlight texture
					case 3:
						{
							cLight3DSpot *pSpotLight = static_cast<cLight3DSpot*>(apLight);
							return pSpotLight->GetTexture();
						}
						break;
					}
				}
			}
			//////////////////////////////////////////////
			// All Other Lighting
			else
			{
				switch(alUnit)
				{
					//Diffuse texture
				case 0: return mvTexture[eMaterialTexture_Diffuse];

					//Normalmap
				case 1: if(mbUseNormalMap) return mvTexture[eMaterialTexture_NMap];break;

					//Normalization map
				case 2: return mpNormalizationMap; break;

					//Falloff map
				case 3: return apLight->GetFalloffMap();

					//Spotlight texture
				case 4: if(apLight->GetLightType() == eLight3DType_Spot)
						{
							cLight3DSpot *pSpotLight = static_cast<cLight3DSpot*>(apLight);
							return pSpotLight->GetTexture();
						};
					break;
					//Negative rejection
				case 5: if(apLight->GetLightType() == eLight3DType_Spot)
						{
							return mpSpotNegativeRejectMap;
						}
						break;
					//Color specular
				case 6:	if(mbUseColorSpecular)
						{
							return mvTexture[eMaterialTexture_Specular];
						}
						break;
				}

			}
		}
		return NULL;
	}

	//-----------------------------------------------------------------------

	eMaterialBlendMode iMaterial_BaseLight::GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight)
	{
		return eMaterialBlendMode_None;
	}

	//-----------------------------------------------------------------------

	int iMaterial_BaseLight::GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight)
	{
		if(aType == eMaterialRenderType_Light && apLight->GetLightType() == eLight3DType_Spot
			&& mbUsesTwoPassSpot)
		{
			return 2;
		}

		return 1;
	}

	//-----------------------------------------------------------------------

	bool iMaterial_BaseLight::UsesType(eMaterialRenderType aType)
	{
		if(aType == eMaterialRenderType_Diffuse)
		{
			if(mvTexture[eMaterialTexture_Illumination])
				return true;
			else
				return false;
		}
		return true;
	}

	//-----------------------------------------------------------------------

	tTextureTypeList iMaterial_BaseLight::GetTextureTypes()
	{
		tTextureTypeList vTypes;
		vTypes.push_back(cTextureType("",eMaterialTexture_Diffuse));
		if(mbUseNormalMap)
			vTypes.push_back(cTextureType("_bump",eMaterialTexture_NMap));

		if(mbUseColorSpecular)
			vTypes.push_back(cTextureType("_spec",eMaterialTexture_Specular));

		vTypes.push_back(cTextureType("_illum",eMaterialTexture_Illumination));

		return vTypes;
	}

	//-----------------------------------------------------------------------

	cMaterialType_BaseLight::cMaterialType_BaseLight(const tString &asLightVertexProgram, const tString &asLightFragmentProgram, cGraphics *apGraphics)
		: iMaterialType(apGraphics)
	{
		///////////////////////////////////////////
		//Load the Z pass program
		mpZPassP = apGraphics->CreateGpuProgramFromShaders("ZPass", "Diffuse_Color.vert", "Diffuse_Color.frag");

		///////////////////////////////////////////
		//More programs
		mpSimpleP = apGraphics->CreateGpuProgramFromShaders("BaseLight_Simple", "Diffuse_Color.vert", "Diffuse_Color.frag");
		mpAmbientP = apGraphics->CreateGpuProgramFromShaders("BaseLight_Ambient", "Diffuse_Color.vert", "Ambient_Color.frag");
	}

	cMaterialType_BaseLight::~cMaterialType_BaseLight()
	{
		if (mpZPassP) mpGraphics->DestroyGpuProgram(mpZPassP);
		if (mpSimpleP) mpGraphics->DestroyGpuProgram(mpSimpleP);
		if (mpAmbientP) mpGraphics->DestroyGpuProgram(mpAmbientP);
	}

	void cMaterialType_BaseLight::DestroyProgram(iMaterial *apMaterial, int i, iGpuProgram *apProgram)
	{
		if (apProgram != mpZPassP)
			mpGraphics->DestroyGpuProgram(apProgram);
	}
}
