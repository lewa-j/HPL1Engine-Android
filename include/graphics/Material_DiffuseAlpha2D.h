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
#ifndef HPL_MATERIAL_DIFFUSE_ALPHA2D_H
#define HPL_MATERIAL_DIFFUSE_ALPHA2D_H

#include "graphics/Material.h"
#include "graphics/MaterialType.h"

namespace hpl {

	class cMaterial_DiffuseAlpha2D : public iMaterial
	{
	public:
		cMaterial_DiffuseAlpha2D(const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture);
		virtual ~cMaterial_DiffuseAlpha2D();

		void Compile();
		bool StartRendering(eMaterialRenderType mType,iCamera* apCam,iLight *pLight);
		void EndRendering(eMaterialRenderType mType);
		tVtxBatchFlag GetBatchFlags(eMaterialRenderType mType);
		bool NextPass(eMaterialRenderType mType);
		bool HasMultiplePasses(eMaterialRenderType mType);

		eMaterialType GetType(eMaterialRenderType mType);
		void EditVertexes(eMaterialRenderType mType, iCamera* apCam, iLight *pLight,
			tVertexVec *apVtxVec,cVector3f *apTransform,unsigned int alIndexAdd);

	private:
	};

	class cMaterialType_DiffuseAlpha2D : public iMaterialType
	{
	public:
		cMaterialType_DiffuseAlpha2D(cGraphics *apGraphics)
			: iMaterialType(apGraphics) {}

		bool IsCorrect(tString asName) override {
			return cString::ToLowerCase(asName)=="diffalpha2d";
		}

		iMaterial* Create(const tString& asName, cGraphics *apGraphics, cResources *apResources, eMaterialPicture aPicture) override
		{
			return hplNew( cMaterial_DiffuseAlpha2D, (asName, apGraphics, apResources, this, aPicture) );
		}
	};

};
#endif // HPL_MATERIAL_DIFFUSE_ALPHA2D_H
