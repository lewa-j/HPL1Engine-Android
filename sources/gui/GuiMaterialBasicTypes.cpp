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
#include "gui/GuiMaterialBasicTypes.h"

#include "graphics/LowLevelGraphics.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// DIFFUSE
	//////////////////////////////////////////////////////////////////////////


	//-----------------------------------------------------------------------

	void cGuiMaterial_Diffuse::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc::One, eBlendFunc::Zero);
	}

	//-----------------------------------------------------------------------

	void cGuiMaterial_Diffuse::AfterRender()
	{
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// ALPHA
	//////////////////////////////////////////////////////////////////////////


	//-----------------------------------------------------------------------

	void cGuiMaterial_Alpha::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc::SrcAlpha, eBlendFunc::OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	void cGuiMaterial_Alpha::AfterRender()
	{
		//Not needed right?
		//mpLowLevelGraphics->SetBlendFunc(eBlendFunc::One, eBlendFunc::OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// FONT NORMAL
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGuiMaterial_FontNormal::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc::SrcAlpha, eBlendFunc::OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	void cGuiMaterial_FontNormal::AfterRender()
	{
		//Not needed right?
		//mpLowLevelGraphics->SetBlendFunc(eBlendFunc::One, eBlendFunc::OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// ADDITIVE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGuiMaterial_Additive::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc::One, eBlendFunc::One);
	}

	//-----------------------------------------------------------------------

	void cGuiMaterial_Additive::AfterRender()
	{
		//Not needed right?
		//mpLowLevelGraphics->SetBlendFunc(eBlendFunc::One, eBlendFunc::OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// MODULATIVE
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGuiMaterial_Modulative::BeforeRender()
	{
		mpLowLevelGraphics->SetBlendActive(true);
		mpLowLevelGraphics->SetBlendFunc(eBlendFunc::DestColor, eBlendFunc::Zero);
	}

	//-----------------------------------------------------------------------

	void cGuiMaterial_Modulative::AfterRender()
	{
		//Not needed right?
		//mpLowLevelGraphics->SetBlendFunc(eBlendFunc::One, eBlendFunc::OneMinusSrcAlpha);
	}

	//-----------------------------------------------------------------------

}
