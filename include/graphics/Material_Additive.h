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
#ifndef HPL_MATERIAL_ADDITIVE_H
#define HPL_MATERIAL_ADDITIVE_H

#include "graphics/Material.h"
#include "graphics/MaterialType.h"

namespace hpl {
	class cMaterialType_Additive;

	class cMaterial_Additive : public iMaterial
	{
	public:
		cMaterial_Additive(const tString& asName, cGraphics *apGraphics, cResources *apResources, cMaterialType_Additive *apType, eMaterialPicture aPicture);
		virtual ~cMaterial_Additive();

		tTextureTypeList GetTextureTypes();

		bool UsesType(eMaterialRenderType aType);

		iGpuProgram* GetProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight) override;
		iMaterialProgramSetup *GetVertexProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		bool VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		bool VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		eMaterialAlphaMode GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialBlendMode GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialChannelMode GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight);

		iTexture* GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight);
		eMaterialBlendMode GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight);

		int GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight){ return 1;}

		//////////////////////////////////////////////////////////////////
		// Old and worthless stuff, only used by 2D renderer
		void Compile(){}
		bool StartRendering(eMaterialRenderType aType,iCamera* apCam,iLight *pLight){return false;}
		void EndRendering(eMaterialRenderType aType){}
		tVtxBatchFlag GetBatchFlags(eMaterialRenderType aType){return 0;}
		bool NextPass(eMaterialRenderType aType){return false;}
		bool HasMultiplePasses(eMaterialRenderType aType){return false;}
		eMaterialType GetType(eMaterialRenderType aType){ return eMaterialType_Diffuse;}
		void EditVertexes(eMaterialRenderType aType, iCamera* apCam, iLight *pLight,
			tVertexVec *apVtxVec,cVector3f *apTransform,unsigned int alIndexAdd){}

	private:
		iGpuProgram *mpFogProg = nullptr;
	};

	class cMaterialType_Additive : public iMaterialType
	{
	public:
		cMaterialType_Additive(cGraphics *apGraphics, cResources *apResources);
		virtual ~cMaterialType_Additive();

		bool IsCorrect(tString asName) override{
			return cString::ToLowerCase(asName)=="additive";
		}

		iMaterial *Create(const tString &asName, cGraphics *apGraphics, cResources *apResources, eMaterialPicture aPicture) override;

	private:
		iGpuProgram *mpFogProg = nullptr;

		friend cMaterial_Additive;
	};

};
#endif // HPL_MATERIAL_ADDITIVE_H
