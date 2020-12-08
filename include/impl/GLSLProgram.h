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
#ifndef HPL_GLSLPROGRAM_H
#define HPL_GLSLPROGRAM_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GPUProgram.h"
#include "graphics/LowLevelGraphics.h"
#include "impl/platform/gl.h"

namespace hpl
{
	class cGLSLProgram : public iGpuProgram
	{
	public:
		cGLSLProgram(tString asName,eGpuProgramType aType);
		~cGLSLProgram();

		bool Reload();
		void Unload();
		void Destroy();

		tString GetProgramName();

		bool CreateFromFile(const tString& asFile, const tString& asEntry);
		bool CreateFromFiles(const tString& asFileVertex, const tString& asFileFragment);

		void Bind();
		void UnBind();

		bool SetFloat(const tString& asName, float afX);
		bool SetVec2f(const tString& asName, float afX,float afY);
		bool SetVec3f(const tString& asName, float afX,float afY,float afZ);
		bool SetVec4f(const tString& asName, float afX,float afY,float afZ, float afW);

		bool SetMatrixf(const tString& asName, const cMatrixf& mMtx);
		bool SetMatrixf(const tString& asName, eGpuProgramMatrix mType,
									eGpuProgramMatrixOp mOp);

		bool SetTexture(const tString& asName,iTexture* apTexture, bool abAutoDisable=true);
		bool SetTextureToUnit(int alUnit, iTexture* apTexture);


		/// GLSL SPECIFIC //////////////////////

	protected:
		bool Create(const char *vt, const char *ft);

		tString msName;
		tString msFile;
		int mId;
	};
};
#endif // HPL_GLSLPROGRAM_H
