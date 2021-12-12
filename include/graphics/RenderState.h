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
#ifndef HPL_RENDER_SET_H
#define HPL_RENDER_SET_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"
#include "graphics/Material.h"

namespace hpl {

	class cRenderSettings;
	class iRenderable;
	class iLight3D;
	class cSector;

	enum eRenderStateType
	{
		eRenderStateType_Sector = 0,
		eRenderStateType_Pass = 1,
		eRenderStateType_DepthTest = 2,
		eRenderStateType_Depth = 3,
		eRenderStateType_AlphaMode = 4,
		eRenderStateType_BlendMode = 5,
		eRenderStateType_Program = 6,
		eRenderStateType_Texture = 7,
		eRenderStateType_VertexBuffer = 8,
		eRenderStateType_Matrix = 9,
		eRenderStateType_Render = 10,
		eRenderStateType_LastEnum  = 11
	};
	///////////// INTERFACE ////////////////////

	class iRenderState
	{
	public:
		iRenderState(){}

		int Compare(const iRenderState* apState) const;
		void SetMode(cRenderSettings* apSettings);

		void Set(const iRenderState* apState);

		eRenderStateType mType;

		//Properties:
		//Sector
		cSector *mpSector;

		//Pass
		int mlPass;

		//DepthTest
		bool mbDepthTest;

		//Depth:
		float mfZ;

		//Alpha
		eMaterialAlphaMode mAlphaMode;

		//Blend
		eMaterialBlendMode mBlendMode;
		eMaterialChannelMode mChannelMode;

		//Program
		iGpuProgram *mpProgram;
		iMaterialProgramSetup* mpVtxProgramSetup;
		iMaterialProgramSetup *mpFragProgramSetup;
		bool mbUsesLight;
		bool mbUsesEye;
		iLight3D* mpLight;

		//Texture
		iTexture* mpTexture[MAX_TEXTUREUNITS];
		eMaterialBlendMode mTextureBlend[MAX_TEXTUREUNITS];

		//Vertex buffer
		iVertexBuffer *mpVtxBuffer;

		//Matrix
		cMatrixf *mpModelMatrix;
		cMatrixf *mpInvModelMatrix;

		//Scale
		cVector3f mvScale;

		//Render
		iRenderable *mpObject;


	private:
		//Compare
		int CompareSector(const iRenderState* apState)const;
		int ComparePass(const iRenderState* apState)const;
		int CompareDepthTest(const iRenderState* apState)const;
		int CompareDepth(const iRenderState* apState)const;
		int CompareAlpha(const iRenderState* apState)const;
		int CompareBlend(const iRenderState* apState)const;
		int CompareProg(const iRenderState* apState)const;
		int CompareTexture(const iRenderState* apState)const;
		int CompareVtxBuff(const iRenderState* apState)const;
		int CompareMatrix(const iRenderState* apState)const;
		int CompareRender(const iRenderState* apState)const;
		//Set mode
		void SetSectorMode(cRenderSettings* apSettings);
		void SetPassMode(cRenderSettings* apSettings);
		void SetDepthTestMode(cRenderSettings* apSettings);
		void SetDepthMode(cRenderSettings* apSettings);
		void SetBlendMode(cRenderSettings* apSettings);
		void SetAlphaMode(cRenderSettings* apSettings);
		void SetProgMode(cRenderSettings* apSettings);
		void SetTextureMode(cRenderSettings* apSettings);
		void SetVtxBuffMode(cRenderSettings* apSettings);
		void SetMatrixMode(cRenderSettings* apSettings);
		void SetRenderMode(cRenderSettings* apSettings);
	};

	////////////// DEPTH ///////////////////

	/*class cRenderState_Depth : public iRenderState
	{
	public:
		cRenderState_Depth() : iRenderState(eRenderStateType_Depth){}

		int Compare(iRenderState* apState);
		void SetMode(cRenderSettings* apSettings);

		//Properties:
		float mfZ;
	};

	////////////// ALPHA MODE ///////////////////

	class cRenderState_AlphaMode : public iRenderState
	{
	public:
		cRenderState_AlphaMode() : iRenderState(eRenderStateType_AlphaMode){}

		int Compare(iRenderState* apState);
		void SetMode(cRenderSettings* apSettings);

		//Properties:
		eMaterialAlphaMode mMode;
	};

	////////////// BLEND MODE ///////////////////

	class cRenderState_BlendMode : public iRenderState
	{
	public:
		cRenderState_BlendMode() : iRenderState(eRenderStateType_BlendMode){}

		int Compare(iRenderState* apState);
		void SetMode(cRenderSettings* apSettings);

		//Properties:
		eMaterialBlendMode mBlendMode;
		eMaterialChannelMode mChannelMode;
	};

	/////////////// PROGRAM //////////////////

	class cRenderState_Program : public iRenderState
	{
	public:
		cRenderState_Program() : iRenderState(eRenderStateType_Program){}

		int Compare(iRenderState* apState);
		void SetMode(cRenderSettings* apSettings);

		//Properties:
		iGpuProgram *mpProgram;
		bool mbUsesLight;
		bool mbUsesEye;
	};

	///////////////// TEXTURE ////////////////

	class cRenderState_Texture : public iRenderState
	{
	public:
		cRenderState_Texture() : iRenderState(eRenderStateType_Texture){}

		int Compare(iRenderState* apState);
		void SetMode(cRenderSettings* apSettings);

		//Properties:
		iTexture* mpTexture[MAX_TEXTUREUNITS];
	};

	///////////////// VERTEX BUFFER ////////////////

	class cRenderState_VertexBuffer : public iRenderState
	{
	public:
		cRenderState_VertexBuffer() : iRenderState(eRenderStateType_VertexBuffer){}

		int Compare(iRenderState* apState);
		void SetMode(cRenderSettings* apSettings);

		//Properties:
		iVertexBuffer *mpVtxBuffer;
	};

	///////////////// MATRIX //////////////////////

	class cRenderState_Matrix : public iRenderState
	{
	public:
		cRenderState_Matrix() : iRenderState(eRenderStateType_Matrix){}

		int Compare(iRenderState* apState);
		void SetMode(cRenderSettings* apSettings);

		//Properties:
		cMatrixf *mpModelMatrix;
		cMatrixf *mpInvModelMatrix;
	};

	///////////////// RENDER //////////////////////

	class cRenderState_Render : public iRenderState
	{
	public:
		cRenderState_Render() : iRenderState(eRenderStateType_Render){}

		int Compare(iRenderState* apState);
		void SetMode(cRenderSettings* apSettings);

		//Properties:
		iRenderable *mpObject;
	};*/
};
#endif // HPL_RENDER_SET_H
