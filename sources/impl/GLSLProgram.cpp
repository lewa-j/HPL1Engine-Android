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
#include "impl/Platform.h"

namespace hpl
{
cGLSLProgram::cGLSLProgram(tString asName,eGpuProgramType aType)
 : iGpuProgram(asName,aType), mId(0)
{
	
}
cGLSLProgram::~cGLSLProgram()
{
	if(mId)
		glDeleteProgram(mId);
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

char* LoadCharBuffer(const tString& asFileName, int& alLength)
{
	FILE *pFile = fopen(asFileName.c_str(), "rb");
	if(pFile == nullptr){
		alLength = 0;
		return nullptr;
	}

	int lLength = (int)Platform::FileLength(pFile);
	alLength = lLength;

	char *pBuffer = hplNewArray(char, lLength+1);
	fread(pBuffer, lLength, 1, pFile);
	pBuffer[lLength] = 0;

	fclose(pFile);

	return pBuffer;
}

bool cGLSLProgram::CreateFromFiles(const tString& asFileVertex, const tString& asFileFragment)
{
	if(mProgramType != eGpuProgramType_Linked)
		return false;

	int vsl,fsl;
	char *vs = LoadCharBuffer(asFileVertex, vsl);
	char *fs = LoadCharBuffer(asFileFragment, fsl);
	bool cr = Create(vs, fs);
	hplDeleteArray(vs);
	hplDeleteArray(fs);
	if(!cr){
		return false;
	}

	return true;
}

void cGLSLProgram::Bind()
{
	glUseProgram(mId);
}

void cGLSLProgram::UnBind()
{
	
}

bool cGLSLProgram::SetFloat(const tString& asName, float afX)
{
	int loc = glGetUniformLocation(mId, asName.c_str());
	glUniform1f(loc, afX);
	return loc != -1;
}

bool cGLSLProgram::SetVec2f(const tString& asName, float afX,float afY)
{
	int loc = glGetUniformLocation(mId, asName.c_str());
	glUniform2f(loc, afX, afY);
	return loc != -1;
}

bool cGLSLProgram::SetVec3f(const tString& asName, float afX,float afY,float afZ)
{
	int loc = glGetUniformLocation(mId, asName.c_str());
	glUniform3f(loc, afX, afY, afZ);
	return loc != -1;
}

bool cGLSLProgram::SetVec4f(const tString& asName, float afX,float afY,float afZ, float afW)
{
	int loc = glGetUniformLocation(mId, asName.c_str());
	glUniform4f(loc, afX, afY, afZ, afW);
	return loc != -1;
}

bool cGLSLProgram::SetMatrixf(const tString& asName, const cMatrixf& mMtx)
{
	int loc = glGetUniformLocation(mId, asName.c_str());
	glUniformMatrix4fv(loc, 1, true, &mMtx.m[0][0]);
	return loc != -1;
}

bool cGLSLProgram::SetMatrixf(const tString& asName, eGpuProgramMatrix mType,
							eGpuProgramMatrixOp mOp)
{
	Warning("cGLSLProgram::SetMatrixf unimplemented type %d op %d\n",mType, mOp);
	return false;
}

bool cGLSLProgram::SetTexture(const tString& asName,iTexture* apTexture, bool abAutoDisable)
{
	Warning("cGLSLProgram::SetTexture unimplemented\n");
	return false;
}

bool cGLSLProgram::SetTextureToUnit(int alUnit, iTexture* apTexture)
{
	Warning("cGLSLProgram::SetTextureToUnit unimplemented\n");
	return false;
}

int CreateShader(int type, const char *src)
{
	int s = glCreateShader(type);
	glShaderSource(s, 1, &src, 0);
	glCompileShader(s);
	int status = 0;
	glGetShaderiv(s, GL_COMPILE_STATUS, &status);
	if(!status){
		Log("%s shader compile status %d\n",type==GL_VERTEX_SHADER ? "Vertex" : "Fragment", status);
		
		int size = 0;
		glGetShaderiv(s, GL_INFO_LOG_LENGTH, &size);
		char data[size];
		glGetShaderInfoLog(s, size, &size, data);
		
		Log("Compile log:\n%s\n===========\n",data);
	}
	return s;
}

bool cGLSLProgram::Create(const char *vt, const char *ft)
{
	int vs = CreateShader(GL_VERTEX_SHADER,vt);
	int fs = CreateShader(GL_FRAGMENT_SHADER,ft);

	mId = glCreateProgram();
	glBindAttribLocation(mId,eVtxAttr_Position,"a_position");
	glBindAttribLocation(mId,eVtxAttr_Color0,"a_color");
	glBindAttribLocation(mId,eVtxAttr_Normal,"a_normal");
	glBindAttribLocation(mId,eVtxAttr_Texture0,"a_uv");
	glBindAttribLocation(mId,eVtxAttr_Tangent,"a_tangent");

	glAttachShader(mId,vs);
	glAttachShader(mId,fs);
	glLinkProgram(mId);

	glDeleteShader(vs);
	glDeleteShader(fs);

	int status = 0;
	glGetProgramiv(mId,GL_LINK_STATUS,&status);
	if(!status)
		Log("link status %d\n",status);

	int size=0;
	glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &size);
	if(size){
		char data[size];
		glGetProgramInfoLog(mId, size, &size, data);
		Log("Shader program log: %s\n",data);
	}

	if(!status)
		return false;

	glUseProgram(mId);
	glUniform1i(glGetUniformLocation(mId,"diffuseMap"),0);
	glUniform1i(glGetUniformLocation(mId,"normalMap"),1);
	glUniform1i(glGetUniformLocation(mId,"normalCubeMap"),2);
	glUniform1i(glGetUniformLocation(mId,"falloffMap"),3);
	glUniform1i(glGetUniformLocation(mId,"spotlightMap"),4);
	glUniform1i(glGetUniformLocation(mId,"spotNegRejectMap"),5);
	glUniform1i(glGetUniformLocation(mId,"specularMap"),6);
	glUseProgram(0);

	return status;
}

}
