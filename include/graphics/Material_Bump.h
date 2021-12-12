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
#ifndef HPL_MATERIAL_BUMP_H
#define HPL_MATERIAL_BUMP_H

#include "graphics/Material_BaseLight.h"
#include "graphics/MaterialType.h"

namespace hpl {

	class cMaterialType_Bump;

	class cMaterial_Bump : public iMaterial_BaseLight
	{
	public:
		cMaterial_Bump(const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture);
		virtual ~cMaterial_Bump();

	private:
	};

	class cMaterialType_Bump : public iMaterialType
	{
	public:
		cMaterialType_Bump(cGraphics *apGraphics);
		virtual ~cMaterialType_Bump();

		bool IsCorrect(tString asName) override{
			return cString::ToLowerCase(asName)=="bump";
		}

		iMaterial* Create(const tString& asName, cGraphics *apGraphics, cResources *apResources, eMaterialPicture aPicture) override;

		friend cMaterial_Bump;
	};

};
#endif // HPL_MATERIAL_BUMPE_H
