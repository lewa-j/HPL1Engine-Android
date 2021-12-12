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
#include "graphics/GPUProgram.h"
#include "resources/Resources.h"
#include "resources/GpuShaderManager.h"

namespace hpl{

	bool iGpuProgram::mbDebugInfo = false;

	iGpuProgram::iGpuProgram(const tString &asName, eGpuProgramFormat aFormat)
	{
		msName = asName;
		mProgramFormat = aFormat;
	}

	iGpuProgram::~iGpuProgram() 
	{
		if (mbAutoDestroyShaders && mpResources)
		{
			for (int i = 0; i < 2; ++i)
			{
				if (mpShader[i]) mpResources->GetGpuShaderManager()->Destroy(mpShader[i]);
			}
		}
	}

	void iGpuProgram::SetShader(eGpuShaderType aType, iGpuShader *apShader)
	{
		mpShader[static_cast<int>(aType)] = apShader;
	}
}
