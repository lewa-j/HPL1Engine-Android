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
#include "resources/ParticleManager.h"

#include "system/LowLevelSystem.h"
#include "resources/Resources.h"
#include "resources/FileSearcher.h"
#include "graphics/Graphics.h"
#include "graphics/ParticleSystem3D.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cParticleManager::cParticleManager(cGraphics* apGraphics,cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
							apResources->GetLowLevelSystem())
	{
		mpGraphics = apGraphics;
		mpResources = apResources;
	}

	cParticleManager::~cParticleManager()
	{
		tResourceHandleMapIt it = m_mapHandleResources.begin();
		for(;it != m_mapHandleResources.end(); ++it)
		{
			iResourceBase* pResource = it->second;
			while(pResource->HasUsers()) pResource->DecUserCount();
		}

		DestroyUnused(0);

		Log(" Done with particles\n");


	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iParticleSystem2D* cParticleManager::CreatePS2D(const tString& asName, cVector3f avSize)
	{
		/*tParticleSystemDataMapIt it = m_mapData.find(asName);
		if(it == m_mapData.end()) return NULL;

		iParticleSystemData *pData = it->second;

		//Is name needed?..nahh
		iParticleSystem* pPart = pData->Create("",avSize);
		pPart->SetDataName(asName);
		pPart->SetDataSize(avSize);

		if(pPart->GetType() == eParticleSystemType_2D)
			return static_cast<iParticleSystem2D*>(pPart);
		else
		{
			hplDelete(pPart);
			return NULL;
		}*/

		return NULL;
	}

	//-----------------------------------------------------------------------

	cParticleSystem3D* cParticleManager::CreatePS3D(const tString& asName,const tString& asType,
												cVector3f avSize,const cMatrixf& a_mtxTransform)
	{
		cParticleSystemData3D *pData = NULL;

		tString sTypeName = cString::SetFileExt(cString::ToLowerCase(asType),"");


		//tParticleSystemData3DMapIt it = m_mapData3D.find(sTypeName);
		//if(it == m_mapData3D.end())
		pData = static_cast<cParticleSystemData3D*>(GetByName(sTypeName));
		if(pData == NULL)
		{
			tString sFile = cString::SetFileExt(asType,"ps");

			tString sPath = mpFileSearcher->GetFilePath(sFile);

			if(sPath == "")
			{
				Error("Couldn't find particle system file '%s'\n",sFile.c_str());
				return NULL;
			}

			cParticleSystemData3D *pPSData = hplNew( cParticleSystemData3D, (sTypeName,
																		mpResources,mpGraphics) );

			if(pPSData->LoadFromFile(sPath)==false)
			{
				Error("Can't load data from particle system file '%s'\n",sTypeName.c_str());
				hplDelete(pPSData);
				return NULL;
			}

			AddData3D(pPSData);

			pData = pPSData;
		}


		pData->IncUserCount();
		cParticleSystem3D* pPS = pData->Create(asName,avSize,a_mtxTransform);
		pPS->SetDataName(asType);
		pPS->SetDataSize(avSize);
		pPS->SetParticleManager(this);

		return pPS;
	}

	//-----------------------------------------------------------------------

	void cParticleManager::AddData3D(cParticleSystemData3D *apData)
	{
		AddResource(apData);
		//m.insert(tParticleSystemData3DMap::value_type(cString::ToLowerCase(apData->GetName()),
		//														apData));
	}

	//-----------------------------------------------------------------------

	void cParticleManager::Preload(const tString& asFile)
	{
		tString sTypeName = cString::SetFileExt(cString::ToLowerCase(asFile),"");

		//tParticleSystemData3DMapIt it = m_mapData3D.find(sTypeName);
		//if(it == m_mapData3D.end())
		cParticleSystemData3D *pData = static_cast<cParticleSystemData3D*>(GetByName(sTypeName));
		if(pData == NULL)
		{
			tString sFile = cString::SetFileExt(asFile,"ps");
			tString sPath = mpFileSearcher->GetFilePath(sFile);
			if(sPath == "")
			{
				Error("Couldn't find particle system file '%s'\n",sFile.c_str());
				return;
			}

			cParticleSystemData3D *pPSData = hplNew( cParticleSystemData3D, (sTypeName,
																	mpResources,mpGraphics) );

			if(pPSData->LoadFromFile(sPath)==false)
			{
				Error("Can't load data from particle system file '%s'\n",sTypeName.c_str());
				hplDelete(pPSData);
				return;
			}

			AddData3D(pPSData);
		}
	}

	//-----------------------------------------------------------------------

	void cParticleManager::Unload(iResourceBase* apResource)
	{

	}
	//-----------------------------------------------------------------------

	void cParticleManager::Destroy(iResourceBase* apResource)
	{
		if(apResource->HasUsers())
		{
			apResource->DecUserCount();
		}
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------
}
