/*
 * Copyright (C) 2020 - lewa_j
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
#include "impl/AndroidGameSetup.h"
#include "impl/LowLevelSystemAndroid.h"
#include "impl/LowLevelGraphicsAndroid.h"
#include "impl/LowLevelInputAndroid.h"
#include "impl/LowLevelResourcesAndroid.h"
//#include "impl/LowLevelSoundOpenAL.h"
#include "impl/LowLevelSoundNull.h"
#include "impl/LowLevelPhysicsNewton.h"

namespace hpl
{
	cAndroidGameSetup::cAndroidGameSetup()
	{
		mpLowLevelSystem = hplNew( cLowLevelSystemAndroid, () );
		mpLowLevelGraphics = hplNew( cLowLevelGraphicsAndroid,() );
		mpLowLevelInput = hplNew( cLowLevelInputAndroid,(mpLowLevelGraphics) );
		mpLowLevelResources = hplNew( cLowLevelResourcesAndroid,((cLowLevelGraphicsAndroid *)mpLowLevelGraphics) );
		mpLowLevelSound	= hplNew( cLowLevelSoundNull,() );
		mpLowLevelPhysics = nullptr;//hplNew( cLowLevelPhysicsNewton,() );

#ifdef INCLUDE_HAPTIC
		mpLowLevelHaptic = hplNew( cLowLevelHapticHaptX,() );
#else
		mpLowLevelHaptic = NULL;
#endif
	}
	
	cAndroidGameSetup::~cAndroidGameSetup()
	{
		Log("- Deleting lowlevel stuff.\n");

		Log("  Physics\n");
		hplDelete(mpLowLevelPhysics);
		Log("  Sound\n");
		hplDelete(mpLowLevelSound);
		Log("  Input\n");
		hplDelete(mpLowLevelInput);
		Log("  Resources\n");
		hplDelete(mpLowLevelResources);
		Log("  System\n");
		hplDelete(mpLowLevelSystem);
		Log("  Graphics\n");
		hplDelete(mpLowLevelGraphics);
		Log("  Haptic\n");
		if(mpLowLevelHaptic) hplDelete(mpLowLevelHaptic);
	}
	
	cScene* cAndroidGameSetup::CreateScene(cGraphics* apGraphics, cResources *apResources, cSound* apSound,
										cPhysics *apPhysics, cSystem *apSystem,cAI *apAI,
										cHaptic *apHaptic)
	{
		cScene *pScene = hplNew( cScene, (apGraphics,apResources, apSound,apPhysics, apSystem,apAI,apHaptic) );
		return pScene;
	}
	
	cResources* cAndroidGameSetup::CreateResources(cGraphics* apGraphics)
	{
		cResources *pResources = hplNew( cResources, (mpLowLevelResources,mpLowLevelGraphics) );
		return pResources;
	}

	//-----------------------------------------------------------------------

	cInput* cAndroidGameSetup::CreateInput(cGraphics* apGraphics)
	{
		cInput *pInput = hplNew( cInput, (mpLowLevelInput) );
		return pInput;
	}

	//-----------------------------------------------------------------------

	cSystem* cAndroidGameSetup::CreateSystem()
	{
		cSystem *pSystem = hplNew( cSystem, (mpLowLevelSystem) );
		return pSystem;
	}

	//-----------------------------------------------------------------------

	cGraphics* cAndroidGameSetup::CreateGraphics()
	{
		cGraphics *pGraphics = hplNew( cGraphics, (mpLowLevelGraphics,mpLowLevelResources) );
		return pGraphics;
	}
	//-----------------------------------------------------------------------

	cSound* cAndroidGameSetup::CreateSound()
	{
		cSound *pSound = hplNew( cSound, (mpLowLevelSound) );
		return pSound;
	}

	//-----------------------------------------------------------------------

	cPhysics* cAndroidGameSetup::CreatePhysics()
	{
		cPhysics *pPhysics = hplNew( cPhysics, (mpLowLevelPhysics) );
		return pPhysics;
	}

	//-----------------------------------------------------------------------

	cAI* cAndroidGameSetup::CreateAI()
	{
		cAI *pAI = hplNew( cAI,() );
		return pAI;
	}

	//-----------------------------------------------------------------------

	cHaptic* cAndroidGameSetup::CreateHaptic()
	{
		cHaptic *pHaptic = hplNew( cHaptic, (mpLowLevelHaptic) );
		return pHaptic;
	}
}

