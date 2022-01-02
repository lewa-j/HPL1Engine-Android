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
#include "graphics/Material_DiffuseSpec.h"

#include "graphics/Material_Flat.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_DiffuseSpec::cMaterial_DiffuseSpec(const tString& asName, cGraphics *apGraphics, cResources *apResources, cMaterialType_BaseLight *apType, eMaterialPicture aPicture)
		: iMaterial_BaseLight(	"DiffuseSpec_Light.vert",
								"DiffuseSpec_Light.frag",
			asName, apGraphics, apResources, apType, aPicture)
	{
		mbUseSpecular = true;
		mbUseNormalMap = false;
	}

	//-----------------------------------------------------------------------

	cMaterial_DiffuseSpec::~cMaterial_DiffuseSpec()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterialType_DiffuseSpec::cMaterialType_DiffuseSpec(cGraphics *apGraphics)
		: cMaterialType_BaseLight("DiffuseSpec_Light.vert", "DiffuseSpec_Light.frag", apGraphics)
	{
	}

	iMaterial* cMaterialType_DiffuseSpec::Create(const tString& asName, cGraphics *apGraphics, cResources *apResources, eMaterialPicture aPicture)
	{
		if (iMaterial::GetQuality() >= eMaterialQuality_High)
		{
			return hplNew( cMaterial_DiffuseSpec, (asName, apGraphics, apResources, this, aPicture) );
		}
		else
		{
			return hplNew( cMaterial_Flat, (asName, apGraphics, apResources, this, aPicture) );
		}
	}

	//-----------------------------------------------------------------------
}
