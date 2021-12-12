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
#include "resources/VideoManager.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "resources/Resources.h"
#include "resources/FileSearcher.h"
#include "graphics/VideoStream.h"

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cVideoManager::cVideoManager(cGraphics* apGraphics,cResources *apResources)
		: iResourceManager(apResources->GetFileSearcher(), apResources->GetLowLevel(),
							apResources->GetLowLevelSystem())
	{
		mpGraphics = apGraphics;
		mpResources = apResources;
	}

	cVideoManager::~cVideoManager()
	{
		STLDeleteAll(mlstVideoLoaders);
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iVideoStream* cVideoManager::CreateVideo(const tString& asName)
	{
		BeginLoad(asName);

		tString sPath = mpFileSearcher->GetFilePath(asName);
		if(sPath == "")
		{
			EndLoad();
			Error("Video file '%s' could not be found!\n",asName.c_str());
			return NULL;
		}

		iVideoStreamLoader *pLoader = GetLoader(asName);
		if(pLoader==NULL)
		{
			Error("Could not find a loader for '%s'\n",asName.c_str());
			return NULL;
		}

		iVideoStream *pVideo = pLoader->Create(asName);

		if(pVideo->LoadFromFile(sPath)==false)
		{
			EndLoad();
			hplDelete(pVideo);
			Error("Could not load video '%s'\n",asName.c_str());
			return NULL;
		}

		AddResource(pVideo);

		EndLoad();
		return pVideo;
	}

	//-----------------------------------------------------------------------

	void cVideoManager::AddVideoLoader(iVideoStreamLoader *apLoader)
	{
		mlstVideoLoaders.push_back(apLoader);
	}

	//-----------------------------------------------------------------------

	void cVideoManager::Unload(iResourceBase* apResource)
	{

	}
	//-----------------------------------------------------------------------

	void cVideoManager::Destroy(iResourceBase* apResource)
	{
		if(apResource)
		{
			RemoveResource(apResource);
			hplDelete(apResource);
		}
	}

	//-----------------------------------------------------------------------

	void cVideoManager::Update(float afTimeStep)
	{
		tResourceHandleMapIt it = m_mapHandleResources.begin();
		for(; it != m_mapHandleResources.end(); ++it)
		{
			iResourceBase *pBase = it->second;
			iVideoStream *pVideo = static_cast<iVideoStream*>(pBase);

			pVideo->Update(afTimeStep);
		}

	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	iVideoStreamLoader * cVideoManager::GetLoader(const tString& asFileName)
	{
		tString sExt = cString::ToLowerCase(cString::GetFileExt(asFileName));

		tVideoStreamLoaderListIt it = mlstVideoLoaders.begin();
		for(; it != mlstVideoLoaders.end(); ++it)
		{
			iVideoStreamLoader *pLoader = *it;

			tStringVec& vExt = pLoader->GetExtensions();
			for(size_t i=0; i< vExt.size(); ++i)
			{
				if(vExt[i] == sExt){
					return pLoader;
				}
			}
		}

		return NULL;
	}


	//-----------------------------------------------------------------------
}
