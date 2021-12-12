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
#include "graphics/Material_Diffuse.h"

#include "graphics/Material_Flat.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_Diffuse::cMaterial_Diffuse(const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture)
		: iMaterial_BaseLight(	"Diffuse_Light.vert",
								"Diffuse_Light.frag",
			asName, apGraphics, apResources, apType, aPicture)
	{
		mbUseSpecular = false;
		mbUseNormalMap = false;
	}

	//-----------------------------------------------------------------------

	cMaterial_Diffuse::~cMaterial_Diffuse()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	cMaterialType_Diffuse::cMaterialType_Diffuse(cGraphics *apGraphics)
		: iMaterialType(apGraphics)
	{
		mlTechLevel = 0;
	}

	//-----------------------------------------------------------------------

	iMaterial* cMaterialType_Diffuse::Create(const tString& asName, cGraphics *apGraphics, cResources *apResources, eMaterialPicture aPicture)
	{
		if (iMaterial::GetQuality() >= eMaterialQuality_High)
		{
			return hplNew( cMaterial_Diffuse, (asName, apGraphics, apResources, this, aPicture) );
		}
		else
		{
			return hplNew( cMaterial_Flat, (asName, apGraphics, apResources, this, aPicture) );
		}
	}

	//-----------------------------------------------------------------------
}
