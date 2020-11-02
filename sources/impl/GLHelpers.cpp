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

#include "impl/GLHelpers.h"
#include "impl/platform/gl.h"

namespace hpl
{
	
	//////////////////////////////////////////////////////////////////////////
	// GLOBAL FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	GLenum ColorFormatToGL(eColorDataFormat aFormat)
	{
		switch(aFormat)
		{
		case eColorDataFormat_RGB:		return GL_RGB;
		case eColorDataFormat_RGBA:		return GL_RGBA;
		case eColorDataFormat_ALPHA:	return GL_ALPHA;
		case eColorDataFormat_BGR:		return GL_BGR;
		case eColorDataFormat_BGRA:		return GL_BGRA;
		};

		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLTextureTargetEnum(eTextureTarget aType)
	{
		switch (aType)
		{
		case eTextureTarget_1D:		return GL_TEXTURE_1D;
		case eTextureTarget_2D:		return GL_TEXTURE_2D;
		case eTextureTarget_Rect:	return GL_TEXTURE_RECTANGLE;
		case eTextureTarget_CubeMap:	return GL_TEXTURE_CUBE_MAP;
		case eTextureTarget_3D:		return GL_TEXTURE_3D;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLWrapEnum(eTextureWrap aMode)
	{
		switch (aMode)
		{
		case eTextureWrap_Clamp: return GL_CLAMP;
		case eTextureWrap_Repeat: return GL_REPEAT;
		case eTextureWrap_ClampToEdge: return GL_CLAMP_TO_EDGE;
		case eTextureWrap_ClampToBorder: return GL_CLAMP_TO_BORDER;
		}

		return GL_REPEAT;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLBlendEnum(eBlendFunc aType)
	{
		switch(aType)
		{
		case eBlendFunc_Zero:					return GL_ZERO;
		case eBlendFunc_One:					return GL_ONE;
		case eBlendFunc_SrcColor:				return GL_SRC_COLOR;
		case eBlendFunc_OneMinusSrcColor:		return GL_ONE_MINUS_SRC_COLOR;
		case eBlendFunc_DestColor:				return GL_DST_COLOR;
		case eBlendFunc_OneMinusDestColor:		return GL_ONE_MINUS_DST_COLOR;
		case eBlendFunc_SrcAlpha:				return GL_SRC_ALPHA;
		case eBlendFunc_OneMinusSrcAlpha:		return GL_ONE_MINUS_SRC_ALPHA;
		case eBlendFunc_DestAlpha:				return GL_DST_ALPHA;
		case eBlendFunc_OneMinusDestAlpha:		return GL_ONE_MINUS_DST_ALPHA;
		case eBlendFunc_SrcAlphaSaturate:		return GL_SRC_ALPHA_SATURATE;
		}
		return 0;
	}

	//-----------------------------------------------------------------------
#ifndef ANDROID
	GLenum GetGLTextureParamEnum(eTextureParam aType)
	{
		switch(aType)
		{
		case eTextureParam_ColorFunc:		return GL_COMBINE_RGB;
		case eTextureParam_AlphaFunc:		return GL_COMBINE_ALPHA;
		case eTextureParam_ColorSource0:	return GL_SOURCE0_RGB;
		case eTextureParam_ColorSource1:	return GL_SOURCE1_RGB;
		case eTextureParam_ColorSource2:	return GL_SOURCE2_RGB;
		case eTextureParam_AlphaSource0:	return GL_SOURCE0_ALPHA;
		case eTextureParam_AlphaSource1:	return GL_SOURCE1_ALPHA;
		case eTextureParam_AlphaSource2:	return GL_SOURCE2_ALPHA;
		case eTextureParam_ColorOp0:		return GL_OPERAND0_RGB;
		case eTextureParam_ColorOp1:		return GL_OPERAND1_RGB;
		case eTextureParam_ColorOp2:		return GL_OPERAND2_RGB;
		case eTextureParam_AlphaOp0:		return GL_OPERAND0_ALPHA;
		case eTextureParam_AlphaOp1:		return GL_OPERAND1_ALPHA;
		case eTextureParam_AlphaOp2:		return GL_OPERAND2_ALPHA;
		case eTextureParam_ColorScale:		return GL_RGB_SCALE;
		case eTextureParam_AlphaScale:		return GL_ALPHA_SCALE;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLTextureOpEnum(eTextureOp aType)
	{
		switch(aType)
		{
		case eTextureOp_Color:			return GL_SRC_COLOR;
		case eTextureOp_OneMinusColor:	return GL_ONE_MINUS_SRC_COLOR;
		case eTextureOp_Alpha:			return GL_SRC_ALPHA;
		case eTextureOp_OneMinusAlpha:	return GL_ONE_MINUS_SRC_ALPHA;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLTextureSourceEnum(eTextureSource aType)
	{
		switch(aType)
		{
		case eTextureSource_Texture:	return GL_TEXTURE;
		case eTextureSource_Constant:	return GL_CONSTANT;
		case eTextureSource_Primary:	return GL_PRIMARY_COLOR;
		case eTextureSource_Previous:	return GL_PREVIOUS;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLTextureFuncEnum(eTextureFunc aType)
	{
		switch(aType)
		{
		case eTextureFunc_Modulate:		return GL_MODULATE;
		case eTextureFunc_Replace:		return GL_REPLACE;
		case eTextureFunc_Add:			return GL_ADD;
		case eTextureFunc_Substract:	return GL_SUBTRACT;
		case eTextureFunc_AddSigned:	return GL_ADD_SIGNED;
		case eTextureFunc_Interpolate:	return GL_INTERPOLATE;
		case eTextureFunc_Dot3RGB:		return GL_DOT3_RGB;
		case eTextureFunc_Dot3RGBA:		return GL_DOT3_RGBA;
		}
		return 0;
	}
#endif
	//-----------------------------------------------------------------------
	
	GLenum GetGLDepthTestFuncEnum(eDepthTestFunc aType)
	{
		switch(aType)
		{
		case eDepthTestFunc_Never:			return GL_NEVER;
		case eDepthTestFunc_Less:				return GL_LESS;
		case eDepthTestFunc_LessOrEqual:		return GL_LEQUAL;
		case eDepthTestFunc_Greater:			return GL_GREATER;
		case eDepthTestFunc_GreaterOrEqual:	return GL_GEQUAL;
		case eDepthTestFunc_Equal:			return GL_EQUAL;
		case eDepthTestFunc_NotEqual:			return GL_NOTEQUAL;
		case eDepthTestFunc_Always:			return GL_ALWAYS;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLAlphaTestFuncEnum(eAlphaTestFunc aType)
	{
		switch(aType)
		{
		case eAlphaTestFunc_Never:			return GL_NEVER;
		case eAlphaTestFunc_Less:				return GL_LESS;
		case eAlphaTestFunc_LessOrEqual:		return GL_LEQUAL;
		case eAlphaTestFunc_Greater:			return GL_GREATER;
		case eAlphaTestFunc_GreaterOrEqual:	return GL_GEQUAL;
		case eAlphaTestFunc_Equal:			return GL_EQUAL;
		case eAlphaTestFunc_NotEqual:			return GL_NOTEQUAL;
		case eAlphaTestFunc_Always:			return GL_ALWAYS;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLStencilFuncEnum(eStencilFunc aType)
	{
		switch(aType)
		{
		case eStencilFunc_Never:			return GL_NEVER;
		case eStencilFunc_Less:				return GL_LESS;
		case eStencilFunc_LessOrEqual:		return GL_LEQUAL;
		case eStencilFunc_Greater:			return GL_GREATER;
		case eStencilFunc_GreaterOrEqual:	return GL_GEQUAL;
		case eStencilFunc_Equal:			return GL_EQUAL;
		case eStencilFunc_NotEqual:			return GL_NOTEQUAL;
		case eStencilFunc_Always:			return GL_ALWAYS;
		}
		return 0;
	}

	//-----------------------------------------------------------------------

	GLenum GetGLStencilOpEnum(eStencilOp aType)
	{
		switch(aType)
		{
		case eStencilOp_Keep:			return GL_KEEP;
		case eStencilOp_Zero:			return GL_ZERO;
		case eStencilOp_Replace:		return GL_REPLACE;
		case eStencilOp_Increment:		return GL_INCR;
		case eStencilOp_Decrement:		return GL_DECR;
		case eStencilOp_Invert:			return GL_INVERT;
		case eStencilOp_IncrementWrap:	return GL_INCR_WRAP;
		case eStencilOp_DecrementWrap:	return GL_DECR_WRAP;
		}
		return 0;
	}
}

