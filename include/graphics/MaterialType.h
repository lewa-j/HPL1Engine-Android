#pragma once

#include "graphics/Material.h"

namespace hpl {
	class iMaterialType
	{
	public:
		iMaterialType(cGraphics *apGraphics);
		virtual ~iMaterialType() {}
		virtual bool IsCorrect(tString asName) = 0;
		virtual iMaterial *Create(const tString &asName, cGraphics *apGraphics, cResources *apResources, eMaterialPicture aPicture) = 0;

		virtual void DestroyProgram(iMaterial *apMaterial, int i, iGpuProgram *apProgram);

	protected:
		cGraphics *mpGraphics = nullptr;
	};

	typedef std::list<iMaterialType *> tMaterialTypeList;
	typedef tMaterialTypeList::iterator tMaterialTypeListIt;
}
