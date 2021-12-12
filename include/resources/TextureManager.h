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
#ifndef HPL_TEXTURE_MANAGER_H
#define HPL_TEXTURE_MANAGER_H

#include "resources/ResourceManager.h"
#include "graphics/Texture.h"

namespace hpl {

	class cGraphics;
	class cResources;
	class iTexture;

	//------------------------------------------------------

	typedef std::map<tString, iTexture*> tTextureAttenuationMap;
	typedef std::map<tString, iTexture*>::iterator tTextureAttenuationMapIt;

	//------------------------------------------------------

	class cTextureManager : public iResourceManager
	{
	public:
		cTextureManager(cGraphics* apGraphics,cResources *apResources);
		~cTextureManager();

		iTexture* Create1D(const tString& asName,bool abUseMipMaps, bool abCompress=false, eTextureType aType=eTextureType_Normal,
							unsigned int alTextureSizeLevel=0);

		iTexture* Create2D(const tString& asName,bool abUseMipMaps, bool abCompress=false, eTextureType aType=eTextureType_Normal,
							unsigned int alTextureSizeLevel=0, eTextureTarget aTarget= eTextureTarget_2D);

		/**
		 * Creates an animated texture. The name must be [name].[ext]. And then the textures in the animtion must
		 * be named [name]01.[ext], [name]02.[ext], etc
		 * \param asName
		 * \param abUseMipMaps
		 * \param abCompress
		 * \param aType
		 * \return
		 */
		iTexture* CreateAnim2D(const tString& asName,bool abUseMipMaps, bool abCompress=false, eTextureType aType=eTextureType_Normal,
								unsigned int alTextureSizeLevel=0);

		iTexture* CreateCubeMap(const tString& asName,bool abUseMipMaps, bool abCompress=false, eTextureType aType=eTextureType_Normal,
								unsigned int alTextureSizeLevel=0);


		iTexture* CreateAttenuation(const tString& asFallOffName);

		void Destroy(iResourceBase* apResource);
		void Unload(iResourceBase* apResource);

		void Update(float afTimeStep);

	private:
		iTexture* CreateFlatTexture(const tString& asName,bool abUseMipMaps,
									bool abCompress, eTextureType aType, eTextureTarget aTarget,
									unsigned int alTextureSizeLevel);

		iTexture* FindTexture2D(const tString &asName, tString &asFilePath);

		tTextureAttenuationMap m_mapAttenuationTextures;

		tStringList mlstFileFormats;

		tStringVec mvCubeSideSuffixes;

		cGraphics* mpGraphics;
		cResources* mpResources;
	};

};
#endif // HPL_TEXTURE_MANAGER_H
