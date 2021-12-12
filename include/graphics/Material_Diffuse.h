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
#ifndef HPL_MATERIAL_DIFFUSE_H
#define HPL_MATERIAL_DIFFUSE_H

#include "graphics/Material_BaseLight.h"
#include "graphics/MaterialType.h"

namespace hpl {

	class cMaterial_Diffuse : public iMaterial_BaseLight
	{
	public:
		cMaterial_Diffuse(const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture);
		virtual ~cMaterial_Diffuse();

	private:
	};

	class cMaterialType_Diffuse : public iMaterialType
	{
	public:
		cMaterialType_Diffuse(cGraphics *apGraphics);

		bool IsCorrect(tString asName) override{
			return cString::ToLowerCase(asName)=="diffuse";
		}

		iMaterial* Create(const tString& asName, cGraphics *apGraphics, cResources *apResources, eMaterialPicture aPicture) override;

	private:
		int mlTechLevel;
	};

};
#endif // HPL_MATERIAL_DIFFUSE_H
