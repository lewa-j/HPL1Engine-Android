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
#ifndef HPL_PARTICLE_SYSTEM_MANAGER_H
#define HPL_PARTICLE_SYSTEM_MANAGER_H

#include "resources/ResourceManager.h"

#include "math/MathTypes.h"

namespace hpl {

	class cGraphics;
	class cResources;
	class iParticleSystem2D;
	class cParticleSystem3D;
	class cParticleSystemData3D;

	//----------------------------------------------------

	typedef std::map<tString, cParticleSystemData3D*> tParticleSystemData3DMap;
	typedef tParticleSystemData3DMap::iterator tParticleSystemData3DMapIt;


	//----------------------------------------------------

	class cParticleManager : public iResourceManager
	{
	public:
		cParticleManager(cGraphics* apGraphics,cResources *apResources);
		~cParticleManager();

		iParticleSystem2D* CreatePS2D(const tString& asName, cVector3f avSize);

		cParticleSystem3D* CreatePS3D(const tString& asName,const tString& asType, cVector3f avSize,
										const cMatrixf& a_mtxTransform);

		void AddData3D(cParticleSystemData3D *apData);

		void Preload(const tString& asFile);

		void Destroy(iResourceBase* apResource);
		void Unload(iResourceBase* apResource);
	private:
		cGraphics* mpGraphics;
		cResources *mpResources;

		std::list<cParticleSystem3D*> mlstSystems;

		//tParticleSystemData3DMap m_mapData3D;
	};

};
#endif // HPL_PARTICLE_SYSTEM_MANAGER_H
