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

#include "impl/GLSLProgram.h"

namespace hpl
{
cGLSLProgram::cGLSLProgram(tString asName,eGpuProgramType aType)
 : iGpuProgram(asName,aType)
{
	
}
cGLSLProgram::~cGLSLProgram()
{
	
}

bool cGLSLProgram::Reload()
{
	return false;
}
void cGLSLProgram::Unload()
{
	
}
void cGLSLProgram::Destroy()
{
	
}

tString cGLSLProgram::GetProgramName()
{
	return msName;
}

bool cGLSLProgram::CreateFromFile(const tString& asFile, const tString& asEntry)
{
	return false;
}

void cGLSLProgram::Bind()
{
	
}
void cGLSLProgram::UnBind()
{
	
}

bool cGLSLProgram::SetFloat(const tString& asName, float afX)
{
	return true;
}
bool cGLSLProgram::SetVec2f(const tString& asName, float afX,float afY)
{
	return true;
}
bool cGLSLProgram::SetVec3f(const tString& asName, float afX,float afY,float afZ)
{
	return true;
}
bool cGLSLProgram::SetVec4f(const tString& asName, float afX,float afY,float afZ, float afW)
{
	return true;
}

bool cGLSLProgram::SetMatrixf(const tString& asName, const cMatrixf& mMtx)
{
	return true;
}
bool cGLSLProgram::SetMatrixf(const tString& asName, eGpuProgramMatrix mType,
							eGpuProgramMatrixOp mOp)
{
	return true;
}

bool cGLSLProgram::SetTexture(const tString& asName,iTexture* apTexture, bool abAutoDisable)
{
	return true;
}
bool cGLSLProgram::SetTextureToUnit(int alUnit, iTexture* apTexture)
{
	return true;
}

static void cGLSLProgram::SetVProfile(tString asProfile)
{
	msForceVP = asProfile;
}
static void cGLSLProgram::SetFProfile(tString asProfile)
{
	msForceFP = asProfile;
}
static tString &cGLSLProgram::GetVProfile()
{
	return msForceVP;
}
static tString &cGLSLProgram::GetFProfile()
{
	return msForceFP;
}
}
