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
#ifndef HPL_ANDROID_GAMESETUP_H
#define HPL_ANDROID_GAMESETUP_H

#include "game/LowLevelGameSetup.h"
#include "graphics/LowLevelGraphics.h"
#include "haptic/LowLevelHaptic.h"
#include "input/LowLevelInput.h"
#include "physics/LowLevelPhysics.h"
#include "resources/LowLevelResources.h"
#include "sound/LowLevelSound.h"
#include "system/LowLevelSystem.h"

namespace hpl
{
	class cAndroidGameSetup : public iLowLevelGameSetup
	{
	public:
		cAndroidGameSetup();
		~cAndroidGameSetup();

		cInput* CreateInput(cGraphics* apGraphics);
		cSystem* CreateSystem();
		cGraphics* CreateGraphics();
		cResources* CreateResources(cGraphics* apGraphics);
		cScene* CreateScene(cGraphics* apGraphics, cResources* apResources, cSound* apSound,
							cPhysics *apPhysics, cSystem *apSystem,cAI *apAI,cHaptic *apHaptic);
		cSound* CreateSound();
		cPhysics* CreatePhysics();
		cAI* CreateAI();
		cHaptic* CreateHaptic();

	private:
		iLowLevelSystem *mpLowLevelSystem;
		iLowLevelGraphics *mpLowLevelGraphics;
		iLowLevelInput *mpLowLevelInput;
		iLowLevelResources *mpLowLevelResources;
		iLowLevelSound*	mpLowLevelSound;
		iLowLevelPhysics* mpLowLevelPhysics;
		iLowLevelHaptic* mpLowLevelHaptic;
	};
};
#endif // HPL_ANDROID_GAMESETUP_H
