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
#include "impl/LowLevelGraphicsGL.h"
#include "impl/platform/gl.h"

namespace hpl {

	class cGLSLProgram : public iGpuProgram
	{
	public:
		cGLSLProgram(const tString &asName, cLowLevelGraphicsGL *apLowLevelGfx);
		~cGLSLProgram() override;

		bool Link() override;
		void Bind() override;
		void UnBind() override;

		bool SetFloat(const tString &asName, float afX) override;
		bool SetVec2f(const tString &asName, float afX, float afY) override;
		bool SetVec3f(const tString &asName, float afX, float afY, float afZ) override;
		bool SetVec4f(const tString &asName, float afX, float afY, float afZ, float afW) override;

		bool SetMatrixf(const tString &asName, const cMatrixf &mMtx) override;
		bool SetMatrixf(const tString& asName, eGpuProgramMatrix mType,
						eGpuProgramMatrixOp mOp) override;

		bool SetTexture(const tString &asName, iTexture *apTexture, bool abAutoDisable = true) override;
		bool SetTextureToUnit(int alUnit, iTexture *apTexture) override;

		bool IsBound() { return mlHandle == mlCurrentProgram; }
	protected:
		void LogProgramInfoLog();

		GLuint mlHandle;
		static int mlCurrentProgram;
		cLowLevelGraphicsGL *mpLowLevelGfx = nullptr;
		friend class cLowLevelGraphicsGL;
	};
};
#endif // HPL_GLSLPROGRAM_H
