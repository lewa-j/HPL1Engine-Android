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
#ifndef HPL_RENDEROBJECT2D_H
#define HPL_RENDEROBJECT2D_H

#include "system/SystemTypes.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/Material.h"

namespace hpl {

	class iRenderObject2DRenderer
	{
	public:
		virtual void RenderToBatch(eMaterialRenderType aRenderType,
									unsigned int &aIdxAdd)=0;
	};

	class cRenderObject2D
	{
	public:
		cRenderObject2D(iMaterial* apMaterial, tVertexVec* mpVtxVec,tUIntVec* mpIdxVec,
					float afZ,cRect2f& aRect,cMatrixf *apMtx=NULL,cVector3f* apTransform=NULL);

		cRenderObject2D(iMaterial* apMaterial, iRenderObject2DRenderer* apRenderer,
			float afZ);

		~cRenderObject2D();

		iMaterial* GetMaterial()const{ return mpMaterial;}
		tVertexVec* GetVertexVec()const{ return mpVtxVec;}
		tUIntVec* GetIndexVec()const{ return mpIdxVec;}
		cVector3f* GetTransform()const{ return mpTransform;	}
		float GetZ() const { return mfZ;}
		const cRect2f& GetRect() const { return mRect;}
		iRenderObject2DRenderer* GetCustomRenderer()const{ return mpCustomRenderer;}

	private:
		iMaterial* mpMaterial;
		tVertexVec* mpVtxVec;
		tUIntVec* mpIdxVec;
		cMatrixf *mpMtx;
		cVector3f *mpTransform;
		float mfZ;
		cRect2f mRect;
		iRenderObject2DRenderer *mpCustomRenderer;
	};

};
#endif // HPL_RENDEROBJECT2D_H
