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
#ifdef _DEBUG
#pragma comment(lib, "newton_d.lib")
#else
#pragma comment(lib, "newton.lib")
#endif

#include "impl/LowLevelPhysicsNewton.h"

#include "impl/PhysicsWorldNewton.h"
#include "system/LowLevelSystem.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cLowLevelPhysicsNewton::cLowLevelPhysicsNewton()
	{

	}

	//-----------------------------------------------------------------------

	cLowLevelPhysicsNewton::~cLowLevelPhysicsNewton()
	{

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHOD
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	iPhysicsWorld* cLowLevelPhysicsNewton::CreateWorld()
	{
		cPhysicsWorldNewton* pWorld = hplNew( cPhysicsWorldNewton, () );
		return pWorld;
	}

	//-----------------------------------------------------------------------

}
