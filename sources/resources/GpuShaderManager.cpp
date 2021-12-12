/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "resources/GpuShaderManager.h"

#include "system/String.h"
#include "system/LowLevelSystem.h"
#include "system/Platform.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/GPUShader.h"

#include "resources/FileSearcher.h"

#ifdef WIN32
#include <io.h>
#endif

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cGpuShaderManager::cGpuShaderManager(cFileSearcher *apFileSearcher,iLowLevelGraphics *apLowLevelGraphics, 
		iLowLevelResources *apLowLevelResources,iLowLevelSystem *apLowLevelSystem)
		: iResourceManager(apFileSearcher, apLowLevelResources,apLowLevelSystem)
	{
		mpLowLevelGraphics = apLowLevelGraphics;
	}

	cGpuShaderManager::~cGpuShaderManager()
	{
		DestroyAll();

		Log(" Done with Gpu programs\n");
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	void cGpuShaderManager::CheckFeatureSupport()
	{
	}

	//-----------------------------------------------------------------------

	iGpuShader* cGpuShaderManager::CreateShader(const tString& asName, eGpuShaderType aType)
	{
		iGpuShader* pShader;

		BeginLoad(asName);

		/////////////////////////////////////////
		// Normal resource load
		tString sPath;
		pShader = static_cast<iGpuShader*>(FindLoadedResource(asName, sPath));

		if (pShader == NULL && sPath != "")
		{
			pShader = mpLowLevelGraphics->CreateGpuShader(asName, aType);

			if(pShader->CreateFromFile(cString::To16Char(sPath))==false)
			{
				Error("Couldn't create program '%s'\n",asName.c_str());
				hplDelete(pShader);
				EndLoad();
				return NULL;
			}

			AddResource(pShader);
		}

		if(pShader)pShader->IncUserCount();
		else Error("Couldn't load program '%s'\n",asName.c_str());

		EndLoad();
		return pShader;
     }

	//-----------------------------------------------------------------------

	void cGpuShaderManager::Unload(iResourceBase* apResource)
	{

	}
	//-----------------------------------------------------------------------

	void cGpuShaderManager::Destroy(iResourceBase* apResource)
	{
		apResource->DecUserCount();

		if(apResource->HasUsers()==false){
			RemoveResource(apResource);
			hplDelete(apResource);
		}
	}

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	
	bool cGpuShaderManager::IsShaderSupported(const tString& asName, eGpuShaderType aType)
	{
		/////////////////////////////////
		//Get file from file searcher
		tString sPath = mpFileSearcher->GetFilePath(asName);
		if(sPath==""){
			Error("Couldn't find test file '%s' in resources\n",asName.c_str());
			return false;
		}

		/////////////////////////////////
		//Compile
		iGpuShader* pShader = mpLowLevelGraphics->CreateGpuShader(asName, aType);
		
		bool bRet = pShader->CreateFromFile(cString::To16Char(sPath), "main", false);
		hplDelete(pShader);
		
		return bRet;
	}
	
	//-----------------------------------------------------------------------
}
