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
#pragma once

#include "graphics/LowLevelGraphics.h"

typedef unsigned int GLenum;

namespace hpl
{

	GLenum ColorFormatToGL(eColorDataFormat aFormat);
	GLenum GetGLTextureTargetEnum(eTextureTarget aType);
	GLenum GetGLWrapEnum(eTextureWrap aMode);

	//Depth helper
	GLenum GetGLDepthTestFuncEnum(eDepthTestFunc aType);

	//Alpha Helper
	GLenum GetGLAlphaTestFuncEnum(eAlphaTestFunc aType);

	//Stencil helper
	GLenum GetGLStencilFuncEnum(eStencilFunc aType);
	GLenum GetGLStencilOpEnum(eStencilOp aType);

	//Texture helper
	GLenum GetGLTextureParamEnum(eTextureParam aType);
	GLenum GetGLTextureOpEnum(eTextureOp aType);
	GLenum GetGLTextureFuncEnum(eTextureFunc aType);
	GLenum GetGLTextureSourceEnum(eTextureSource aType);

	//Blend helper
	GLenum GetGLBlendEnum(eBlendFunc aType);
}

