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
#ifndef HPL_MATERIAL_BASE_LIGHT_H
#define HPL_MATERIAL_BASE_LIGHT_H

#include "graphics/Material.h"
#include "graphics/MaterialType.h"
#include "scene/Light3D.h"

enum eBaseLightProgram
{
	eBaseLightProgram_Point1=0,
	eBaseLightProgram_Point2=1,
	eBaseLightProgram_Point3=2,
	eBaseLightProgram_Spot1=3,
	eBaseLightProgram_Spot2=4,
	eBaseLightProgram_Spot3=5,
	eBaseLightProgram_LastEnum = 6
};

namespace hpl {

	class cMaterialType_BaseLight;

	class iMaterial_BaseLight : public iMaterial
	{
	public:
		iMaterial_BaseLight(const tString& asLightVertexProgram,
							const tString& asLightFragmentProgram,
			const tString &asName, cGraphics *apGraphics, cResources *apResources, cMaterialType_BaseLight *apType, eMaterialPicture aPicture);
		virtual ~iMaterial_BaseLight();

		tTextureTypeList GetTextureTypes() override;

		bool UsesType(eMaterialRenderType aType) override;

		iGpuProgram* GetProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		bool VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		bool VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		iMaterialProgramSetup * GetFragmentProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;

		eMaterialAlphaMode GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		eMaterialBlendMode GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		eMaterialChannelMode GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;

		iTexture* GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		eMaterialBlendMode GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight) override;

		int GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight) override;

		//////////////////////////////////////////////////////////////////
		// Old and worthless stuff, only used by 2D renderer
		void Compile() override {}
		bool StartRendering(eMaterialRenderType aType,iCamera* apCam,iLight *pLight) override {return false;}
		void EndRendering(eMaterialRenderType aType) override {}
		tVtxBatchFlag GetBatchFlags(eMaterialRenderType aType) override {return 0;}
		bool NextPass(eMaterialRenderType aType) override {return false;}
		bool HasMultiplePasses(eMaterialRenderType aType) override {return false;}
		eMaterialType GetType(eMaterialRenderType aType) override { return eMaterialType_Diffuse;}
		void EditVertexes(eMaterialRenderType aType, iCamera* apCam, iLight *pLight,
			tVertexVec *apVtxVec,cVector3f *apTransform,unsigned int alIndexAdd) override {}

	protected:
		iTexture *mpNormalizationMap;
		iTexture *mpSpotNegativeRejectMap;

		bool mbUsesTwoPassSpot;

		//properties to set
		bool mbUseSpecular;
		bool mbUseNormalMap;
		bool mbUseColorSpecular;

		iGpuProgram* mpSimpleP;
		iGpuProgram* mpAmbientP;

		iGpuProgram* mvPrograms[eBaseLightProgram_LastEnum];
	};

	class cMaterialType_BaseLight : public iMaterialType
	{
	public:
		cMaterialType_BaseLight(const tString &asLightVertexProgram, const tString &asLightFragmentProgram, cGraphics *apGraphics);
		virtual ~cMaterialType_BaseLight();

		virtual void DestroyProgram(iMaterial *apMaterial, int i, iGpuProgram *apProgram) override;

		iGpuProgram *mpZPassP = nullptr;
		iGpuProgram *mpSimpleP = nullptr;
		iGpuProgram *mpAmbientP = nullptr;
	};
};
#endif // HPL_MATERIAL_BASE_LIGHT_H
