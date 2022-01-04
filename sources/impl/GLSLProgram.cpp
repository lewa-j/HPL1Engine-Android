/*
 * Copyright © 2009-2020 Frictional Games
 * Copyright (C) 2020-2021 - lewa_j
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
#include "impl/GLSLShader.h"
#include "impl/Platform.h"
#include "math/Math.h"

namespace hpl {

	int cGLSLProgram::mlCurrentProgram = 0;

	cGLSLProgram::cGLSLProgram(const tString &asName, cLowLevelGraphicsGL *apLowLevelGfx) : iGpuProgram(asName, eGpuProgramFormat::GLSL)
	{
		mpLowLevelGfx = apLowLevelGfx;
		mlHandle = glCreateProgram();
	}

	cGLSLProgram::~cGLSLProgram()
	{
		for (int i = 0; i < 2; i++)
		{
			cGLSLShader *pGLSLShader = static_cast<cGLSLShader *>(mpShader[i]);
			if (pGLSLShader)
			{
				glDetachShader(mlHandle, pGLSLShader->GetHandle());
			}
		}
		glDeleteProgram(mlHandle);
	}

	bool cGLSLProgram::Link()
	{
		glBindAttribLocation(mlHandle, eVtxAttr_Position, "a_position");
		glBindAttribLocation(mlHandle, eVtxAttr_Color0, "a_color");
		glBindAttribLocation(mlHandle, eVtxAttr_Normal, "a_normal");
		glBindAttribLocation(mlHandle, eVtxAttr_Texture0, "a_uv");
		glBindAttribLocation(mlHandle, eVtxAttr_Tangent, "a_tangent");

		///////////////////////////////////////
		//Attach shaders
		for (int i = 0; i < 2; ++i)
		{
			cGLSLShader *pGLSLShader = static_cast<cGLSLShader *>(mpShader[i]);
			if (pGLSLShader)
			{
				glAttachShader(mlHandle, pGLSLShader->GetHandle());
			}
		}

		///////////////////////////////////////
		//Link
		glLinkProgram(mlHandle);

		///////////////////////////////////////
		//Check for errors
		GLint lStatus = 0;
		glGetProgramiv(mlHandle, GL_LINK_STATUS, &lStatus);
		if (lStatus == GL_FALSE)
		{
			Error("Failed to link GLSL program %s (%s, %s)\n", msName.c_str(), mpShader[0]->GetName().c_str(), mpShader[1]->GetName().c_str());
			LogProgramInfoLog();
			return false;
		}

		///////////////////////////////////////
		//Set up sampler units
		glUseProgram(mlHandle);
		glUniform1i(glGetUniformLocation(mlHandle, "diffuseMap"), 0);
		glUniform1i(glGetUniformLocation(mlHandle, "normalMap"), 1);
		glUniform1i(glGetUniformLocation(mlHandle, "normalCubeMap"), 2);
		glUniform1i(glGetUniformLocation(mlHandle, "falloffMap"), 3);
		glUniform1i(glGetUniformLocation(mlHandle, "spotlightMap"), 4);
		glUniform1i(glGetUniformLocation(mlHandle, "spotNegRejectMap"), 5);
		glUniform1i(glGetUniformLocation(mlHandle, "specularMap"), 6);
		glUseProgram(mlCurrentProgram);

		return true;
	}

	void cGLSLProgram::Bind()
	{
		if (mlCurrentProgram == mlHandle)
			return;

		mlCurrentProgram = mlHandle;
		glUseProgram(mlHandle);
	}

	void cGLSLProgram::UnBind()
	{
		if (mlCurrentProgram == 0)
			return;

		mlCurrentProgram = 0;
		if (this != mpLowLevelGfx->mSimpleShader)
		{
			mpLowLevelGfx->mSimpleShader->Bind();
			mpLowLevelGfx->mSimpleShader->SetMatrixf("worldViewProj",
				eGpuProgramMatrix::ViewProjection,
				eGpuProgramMatrixOp::Identity);
		}
		else
		{
			glUseProgram(0);
		}
	}

	bool cGLSLProgram::SetFloat(const tString& asName, float afX)
	{
		int loc = glGetUniformLocation(mlHandle, asName.c_str());
		glUniform1f(loc, afX);
		return loc != -1;
	}

	bool cGLSLProgram::SetVec2f(const tString &asName, float afX, float afY)
	{
		int loc = glGetUniformLocation(mlHandle, asName.c_str());
		glUniform2f(loc, afX, afY);
		return loc != -1;
	}

	bool cGLSLProgram::SetVec3f(const tString &asName, float afX, float afY, float afZ)
	{
		int loc = glGetUniformLocation(mlHandle, asName.c_str());
		glUniform3f(loc, afX, afY, afZ);
		return loc != -1;
	}

	bool cGLSLProgram::SetVec4f(const tString &asName, float afX, float afY, float afZ, float afW)
	{
		int loc = glGetUniformLocation(mlHandle, asName.c_str());
		glUniform4f(loc, afX, afY, afZ, afW);
		return loc != -1;
	}

	bool cGLSLProgram::SetMatrixf(const tString& asName, const cMatrixf& mMtx)
	{
		int loc = glGetUniformLocation(mlHandle, asName.c_str());
		glUniformMatrix4fv(loc, 1, true, mMtx.v);
		return loc != -1;
	}

	bool cGLSLProgram::SetMatrixf(const tString& asName, eGpuProgramMatrix mType,
								eGpuProgramMatrixOp mOp)
	{
		if (mType == eGpuProgramMatrix::ViewProjection){
			cMatrixf mtx = cMath::MatrixMul(mpLowLevelGfx->GetMatrix(eMatrix::Projection),
					mpLowLevelGfx->GetMatrix(eMatrix::ModelView));
			return SetMatrixf(asName, mtx);
		}else{
			Warning("cGLSLProgram::SetMatrixf unimplemented type %d\n", mType);
		}
		return false;
	}

	bool cGLSLProgram::SetTexture(const tString &asName, iTexture *apTexture, bool abAutoDisable)
	{
		Warning("cGLSLProgram::SetTexture unimplemented\n");
		return false;
	}

	bool cGLSLProgram::SetTextureToUnit(int alUnit, iTexture* apTexture)
	{
		Warning("cGLSLProgram::SetTextureToUnit unimplemented\n");
		return false;
	}

	void cGLSLProgram::LogProgramInfoLog()
	{
		GLint infologLength = 0;
		GLsizei charsWritten = 0;
		char *infoLog;

		glGetProgramiv(mlHandle, GL_INFO_LOG_LENGTH, &infologLength);

		if (infologLength > 0)
		{
			infoLog = (char *)hplMalloc(infologLength);
			glGetProgramInfoLog(mlHandle, infologLength, &charsWritten, infoLog);
			Log("-------------\n");
			Log("%s\n", infoLog);
			Log("-------------\n");
			hplFree(infoLog);
		}
	}
}
