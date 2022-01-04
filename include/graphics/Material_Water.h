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
#ifndef HPL_MATERIAL_WATER_H
#define HPL_MATERIAL_WATER_H

#include "graphics/Material.h"
#include "graphics/MaterialType.h"

namespace hpl {

	class cMaterial_Water : public iMaterial
	{
	public:
		cMaterial_Water(const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture);
		virtual ~cMaterial_Water();

		tTextureTypeList GetTextureTypes() override;

		bool UsesType(eMaterialRenderType aType) override;

		void Update(float afTimeStep) override;

		iGpuProgram* GetProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		iMaterialProgramSetup* GetVertexProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		bool VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		bool VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;

		eMaterialAlphaMode GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		eMaterialBlendMode GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		eMaterialChannelMode GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;

		iTexture* GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		eMaterialBlendMode GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight) override;

		int GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight) override { return 1; }

		iGpuProgram *GetRefractionProgam() override { return mpRefractProg; }
		bool GetRefractionUsesDiffuse() override { return true; }
		eMaterialTexture GetRefractionDiffuseTexture() override { return eMaterialTexture_Specular;}
		bool GetRefractionUsesEye() override { return true; }
		bool GetRefractionSkipsStandardTrans() override { return true; }
		bool GetRefractionUsesTime() override { return true; }

		//////////////////////////////////////////////////////////////////
		// Old and worthless stuff, only used by 2D renderer
		void Compile() override {}
		bool StartRendering(eMaterialRenderType aType, iCamera *apCam, iLight *pLight) override { return false; }
		void EndRendering(eMaterialRenderType aType) override {}
		tVtxBatchFlag GetBatchFlags(eMaterialRenderType aType) override { return 0; }
		bool NextPass(eMaterialRenderType aType) override { return false; }
		bool HasMultiplePasses(eMaterialRenderType aType) override { return false; }
		eMaterialType GetType(eMaterialRenderType aType) override { return eMaterialType_Diffuse; }
		void EditVertexes(eMaterialRenderType aType, iCamera* apCam, iLight *pLight,
			tVertexVec *apVtxVec,cVector3f *apTransform,unsigned int alIndexAdd) override {}

	private:
		iGpuProgram *mpFogProg;
		iGpuProgram *mpRefractProg;

		float mfTime;
	};

	class cMaterialType_Water : public iMaterialType
	{
	public:
		cMaterialType_Water(cGraphics *apGraphics)
			: iMaterialType(apGraphics) {}

		bool IsCorrect(tString asName) override{
			return cString::ToLowerCase(asName)=="water";
		}

		iMaterial* Create(const tString& asName, cGraphics *apGraphics, cResources *apResources, eMaterialPicture aPicture) override
		{
			return hplNew( cMaterial_Water, (asName, apGraphics, apResources, this, aPicture) );
		}
	};

};
#endif // HPL_MATERIAL_WATER_H
