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
#ifndef HPL_GPU_PROGRAM_H
#define HPL_GPU_PROGRAM_H

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"

namespace hpl {

	class iTexture;
	class cResources;

	class iGpuProgram
	{
	public:
		iGpuProgram(const tString &asName, eGpuProgramFormat aFormat);
		virtual ~iGpuProgram();

		const tString &GetName() { return msName; }

		void SetShader(eGpuShaderType aType, iGpuShader *apShader);
		iGpuShader* GetShader(eGpuShaderType aType) { return mpShader[static_cast<int>(aType)];}

		void SetResources(cResources *apResources){ mpResources = apResources;}

		static void SetLogDebugInformation(bool abX){mbDebugInfo = abX;}

		virtual bool Link() = 0;
		/**
		 * Bind the program to the GPU
		 */
		virtual void Bind()=0;
		/**
		 * Unbind the program to the GPU
		 */
		virtual void UnBind()=0;

		virtual bool SetFloat(const tString& asName, float afX)=0;

		bool SetVec2f(const tString& asName, const cVector2f avVec)
		{
			return SetVec2f(asName,avVec.x, avVec.y);
		}
		virtual bool SetVec2f(const tString& asName, float afX,float afY)=0;

		bool SetVec3f(const tString& asName, const cVector3f& avVec)
		{
			return SetVec3f(asName, avVec.x,avVec.y,avVec.z);
		}
		bool SetColor3f(const tString& asName, const cColor &aCol)
		{
			return SetVec3f(asName,aCol.r, aCol.g, aCol.b);
		}
		virtual bool SetVec3f(const tString& asName, float afX,float afY,float afZ)=0;

		bool SetColor4f(const tString& asName, const cColor &aCol)
		{
			return SetVec4f(asName,aCol.r, aCol.g, aCol.b, aCol.a);
		}
		virtual bool SetVec4f(const tString& asName, float afX,float afY,float afZ, float afW)=0;

		virtual bool SetMatrixf(const tString& asName, const cMatrixf& mMtx)=0;
		virtual bool SetMatrixf(const tString& asName, eGpuProgramMatrix mType,
			eGpuProgramMatrixOp mOp)=0;

		virtual bool SetTexture(const tString& asName,iTexture* apTexture, bool abAutoDisable=true)=0;
		virtual bool SetTextureToUnit(int alUnit, iTexture* apTexture)=0;

	protected:
		tString msName;
		cResources *mpResources = nullptr;
		eGpuProgramFormat mProgramFormat;
		iGpuShader *mpShader[2]{ nullptr,nullptr };
		bool mbAutoDestroyShaders = true;

		static bool mbDebugInfo;
	};
};
#endif // HPL_GPU_PROGRAM_H
