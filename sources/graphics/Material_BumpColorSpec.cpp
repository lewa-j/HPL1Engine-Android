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
#include "graphics/Material_BumpColorSpec.h"

#include "graphics/Material_Bump.h"
#include "graphics/Material_Flat.h"
#include "graphics/Material_Alpha.h"
#include "graphics/Graphics.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cMaterial_BumpColorSpec::cMaterial_BumpColorSpec(const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture)
		: iMaterial_BaseLight(	"DiffuseSpec_Light.vert",
								"BumpColorSpec_Light.frag",
			asName, apGraphics, apResources, apType, aPicture)
	{
		mbUseSpecular = true;
		mbUseNormalMap = true;
		mbUseColorSpecular = true;
	}

	//-----------------------------------------------------------------------

	cMaterial_BumpColorSpec::~cMaterial_BumpColorSpec()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iMaterial* cMaterialType_BumpColorSpec::Create(const tString& asName, cGraphics *apGraphics, cResources *apResources, eMaterialPicture aPicture)
	{
		if (iMaterial::GetQuality() >= eMaterialQuality_High)
		{
			if (apGraphics->GetLowLevel()->GetCaps(eGraphicCaps_MaxTextureImageUnits) >= 7)
			{
				return hplNew( cMaterial_BumpColorSpec, (asName, apGraphics, apResources, this, aPicture) );
			}
			else
			{
				return hplNew( cMaterial_Bump,(asName, apGraphics, apResources, this, aPicture) );
			}
		}
		else
		{
			return hplNew( cMaterial_Flat, (asName, apGraphics, apResources, this, aPicture) );
		}
	}

	//-----------------------------------------------------------------------
}
