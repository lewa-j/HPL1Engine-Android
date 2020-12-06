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
#ifndef HPL_MESH_LOADER_H
#define HPL_MESH_LOADER_H

#include "math/MathTypes.h"
#include "graphics/GraphicsTypes.h"
#include "system/SystemTypes.h"

namespace hpl {

	typedef tFlag tWorldLoadFlag;

	#define eWorldLoadFlag_NoLights			(0x00000001)
	#define eWorldLoadFlag_NoEntities		(0x00000002)
	#define eWorldLoadFlag_NoGameEntities	(0x00000004)

	typedef tFlag tMeshLoadFlag;

	#define eMeshLoadFlag_NoGeometry		(0x00000001)

	class cMesh;

	class cResources;
	class cMaterialManager;
	class cMeshManager;
	class cAnimationManager;
	class cMeshLoaderHandler;
	class iLowLevelGraphics;
	class cWorld3D;
	class cScene;
	class cAnimation;
	class cSystem;

	class iMeshLoader
	{
	friend class cMeshLoaderHandler;
	public:
		iMeshLoader(iLowLevelGraphics *apLowLevelGraphics)
					: mpLowLevelGraphics(apLowLevelGraphics){}
		virtual ~iMeshLoader(){}

		virtual cMesh* LoadMesh(const tString& asFile, tMeshLoadFlag aFlags)=0;
		virtual bool SaveMesh(cMesh* apMesh,const tString& asFile)=0;

		virtual cWorld3D* LoadWorld(const tString& asFile, cScene* apScene,tWorldLoadFlag aFlags)=0;

		virtual cAnimation* LoadAnimation(const tString& asFile)=0;

		virtual bool IsSupported(const tString asFileType)=0;

		virtual void AddSupportedTypes(tStringVec* avFileTypes)=0;

		static void SetRestricStaticLightToSector(bool abX){ mbRestricStaticLightToSector = abX;}
		static void SetUseFastMaterial(bool abX) {mbUseFastMaterial = abX;}
		static void SetFastMaterialFile(const tString& asFile) { msFastMaterialFile = asFile;}
		static void SetCacheDir(const tString& asDir) { msCacheDir = asDir;}
	protected:
		cMaterialManager *mpMaterialManager;
		cMeshManager *mpMeshManager;
		cAnimationManager *mpAnimationManager;
		iLowLevelGraphics *mpLowLevelGraphics;
		cSystem *mpSystem;

		static bool mbRestricStaticLightToSector;
		static bool mbUseFastMaterial;
		static tString msFastMaterialFile;
		static tString msCacheDir;

	};

};
#endif // HPL_MESH_LOADER_H
