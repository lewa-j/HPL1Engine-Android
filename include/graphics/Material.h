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
#ifndef HPL_MATERIAL_H
#define HPL_MATERIAL_H

#include <vector>
#include "graphics/Texture.h"
#include "system/SystemTypes.h"
#include "resources/ResourceImage.h"
#include "resources/ResourceBase.h"
#include "graphics/LowLevelGraphics.h"
#include "graphics/GPUProgram.h"

class TiXmlElement;

namespace hpl {


#define kMaxProgramNum (5)

	enum eMaterialQuality
	{
		eMaterialQuality_VeryLow=0,
		eMaterialQuality_Low=1,
		eMaterialQuality_Medium=2,
		eMaterialQuality_High=3,
		eMaterialQuality_VeryHigh=4,
		eMaterialQuality_LastEnum=5
	};

	enum eMaterialTexture
	{
		eMaterialTexture_Diffuse,
		eMaterialTexture_NMap,
		eMaterialTexture_Specular,
		eMaterialTexture_Alpha,
		eMaterialTexture_Illumination,
		eMaterialTexture_CubeMap,
		eMaterialTexture_Refraction,
		eMaterialTexture_LastEnum
	};

	enum eMaterialType
	{
		eMaterialType_Null,

		eMaterialType_Diffuse,
		eMaterialType_DiffuseAlpha,
		eMaterialType_DiffuseAdditive,
		eMaterialType_DiffuseNMap,
		eMaterialType_DiffuseSpecular,
		eMaterialType_BumpSpec,
		eMaterialType_Smoke,

		eMaterialType_FontNormal,

		eMaterialType_LastEnum
	};

	enum eMaterialPicture
	{
		eMaterialPicture_Image,
		eMaterialPicture_Texture,
		eMaterialPicture_LastEnum
	};

	enum eMaterialRenderType
	{
		eMaterialRenderType_Z,
		eMaterialRenderType_Light,
		eMaterialRenderType_Diffuse,
		eMaterialRenderType_LastEnum
	};

	enum eMaterialBlendMode
	{
		eMaterialBlendMode_None,
		eMaterialBlendMode_Add,
		eMaterialBlendMode_Mul,
		eMaterialBlendMode_MulX2,
		eMaterialBlendMode_Replace,
		eMaterialBlendMode_Alpha,
		eMaterialBlendMode_DestAlphaAdd,
		eMaterialBlendMode_LastEnum
	};

	enum eMaterialAlphaMode
	{
		eMaterialAlphaMode_Solid,
		eMaterialAlphaMode_Trans,
		eMaterialAlphaMode_LastEnum,
	};

	//! Determines what color channels are going to be affected
	enum eMaterialChannelMode
	{
		eMaterialChannelMode_RGBA,
		eMaterialChannelMode_RGB,
		eMaterialChannelMode_A,
		eMaterialChannelMode_Z,
		eMaterialChannelMode_LastEnum,
	};

	//---------------------------------------------------

	class cGraphics;
	class cRenderer2D;
	class cRenderer3D;
	class cRenderSettings;
	class cTextureManager;
	class cImageManager;
	class iLight;
	class iCamera;
	class iLight3D;
	class iMaterialType;
	class cGpuShaderManager;
	//---------------------------------------------------

	class iGLStateProgram : public iGpuProgram
	{
	public:
		iGLStateProgram(tString asName)
			: iGpuProgram(asName, eGpuProgramFormat::State)
		{
			mbSetUpDone = false;
		}
		virtual ~iGLStateProgram(){}

		void SetUp(iLowLevelGraphics *apLowLevelGraphics)
		{
			if(mbSetUpDone==false)
			{
				mpLowGfx = apLowLevelGraphics;
				mbSetUpDone = true;
				InitData();
			}
		}

		virtual void Bind()=0;
		virtual void UnBind()=0;

		bool CreateFromFile(const tString& asFile, const tString& asEntry){return false;}
		bool SetFloat(const tString& asName, float afX){return false;}

		bool SetVec2f(const tString& asName, float afX,float afY){return false;}
		bool SetVec3f(const tString& asName, float afX,float afY,float afZ){return false;}
		bool SetVec4f(const tString& asName, float afX,float afY,float afZ, float afW){return false;}
		bool SetMatrixf(const tString& asName, const cMatrixf& mMtx){return false;}
		bool SetMatrixf(const tString& asName, eGpuProgramMatrix mType, eGpuProgramMatrixOp mOp){return false;}
		bool SetTexture(const tString& asName,iTexture* apTexture, bool abAutoDisable=true){return false;}
		bool SetTextureToUnit(int alUnit, iTexture* apTexture){return false;}

		eGpuProgramFormat GetType() { return mProgramFormat;}

		bool Reload(){return false;}
		void Unload(){}
		void Destroy(){}
	protected:

		iLowLevelGraphics *mpLowGfx = nullptr;

		bool mbSetUpDone;

		virtual void InitData()=0;
	};

	//---------------------------------------------------------------

	class iMaterialProgramSetup
	{
	public:
		virtual void Setup(iGpuProgram *apProgram,cRenderSettings* apRenderSettings)=0;
		virtual void SetupMatrix(cMatrixf *apModelMatrix, cRenderSettings* apRenderSettings){}
	};

	//---------------------------------------------------

	class cTextureType
	{
	public:
		cTextureType(tString asSuffix, eMaterialTexture aType): msSuffix(asSuffix), mType(aType){}

		tString msSuffix;
		eMaterialTexture mType;
	};

	typedef std::list<cTextureType> tTextureTypeList;
	typedef tTextureTypeList::iterator tTextureTypeListIt;

	//---------------------------------------------------

	class iMaterial : public iResourceBase
	{
	public:
		iMaterial(const tString& asName, cGraphics *apGraphics, cResources *apResources, iMaterialType *apType, eMaterialPicture aPicture);
		virtual ~iMaterial();

		/**
		 * Get type
		 * \param mType
		 * \return
		 */
		virtual eMaterialType GetType(eMaterialRenderType mType)=0;
		virtual void EditVertexes(eMaterialRenderType mType, iCamera* apCam, iLight *pLight,
								tVertexVec *apVtxVec,cVector3f *apTransform,unsigned int alIndexAdd)=0;

		iTexture* GetTexture(eMaterialTexture aType);
		cRect2f GetTextureOffset(eMaterialTexture aType);

		void SetTexture(iTexture* apTex,eMaterialTexture aType){ mvTexture[aType] = apTex; }
		void SetImage(cResourceImage* apImg,eMaterialTexture aType){
			mvImage[aType] = apImg;	}
		cResourceImage* GetImage(eMaterialTexture aType){ return mvImage[aType];}

		virtual void Compile()=0;

		virtual void Update(float afTimeStep){}

		//The new render system stuff
		virtual iGpuProgram *GetProgram(eMaterialRenderType aType, int alPass, iLight3D *apLight) { return NULL; }
		virtual bool VertexProgramUsesLight(eMaterialRenderType aType, int alPass, iLight3D *apLight){return false;}
		virtual bool VertexProgramUsesEye(eMaterialRenderType aType, int alPass, iLight3D *apLight){return false;}
		virtual iMaterialProgramSetup * GetVertexProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight){return NULL;}
		virtual iMaterialProgramSetup * GetFragmentProgramSetup(eMaterialRenderType aType, int alPass, iLight3D *apLight){return NULL;}

		virtual eMaterialAlphaMode GetAlphaMode(eMaterialRenderType aType, int alPass, iLight3D *apLight){return eMaterialAlphaMode_Solid;}
		virtual eMaterialBlendMode GetBlendMode(eMaterialRenderType aType, int alPass, iLight3D *apLight){return eMaterialBlendMode_Replace;}
		virtual eMaterialChannelMode GetChannelMode(eMaterialRenderType aType, int alPass, iLight3D *apLight){return eMaterialChannelMode_RGBA;}

		virtual iTexture* GetTexture(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight){ return NULL;}
		virtual eMaterialBlendMode GetTextureBlend(int alUnit,eMaterialRenderType aType, int alPass, iLight3D *apLight){ return eMaterialBlendMode_None;}

		virtual int GetNumOfPasses(eMaterialRenderType aType, iLight3D *apLight){ return 0;}

		virtual bool UsesType(eMaterialRenderType aType){return false;}

		bool HasAlpha(){ return mbHasAlpha;}
		void SetHasAlpha(bool abX){ mbHasAlpha= abX; }

		bool GetDepthTest(){ return mbDepthTest;}
		void SetDepthTest(bool abX){ mbDepthTest = abX;}

		float GetValue() { return mfValue;}
		void SetValue(float afX) { mfValue = afX;}

		virtual bool LoadData(TiXmlElement* apRootElem){ return true; }

		/**
		 * Here the set up should be done like setting texture units, blend mode, etc
		 * \param mType
		 * \param apCam
		 * \param *pLight
		 * \return
		 */
		virtual bool StartRendering(eMaterialRenderType aType,iCamera* apCam,iLight *pLight)=0;
		/**
		 * Here all stuff should be set back to normal, like unbinding gpu programs
		 * \param mType
		 */
		virtual void EndRendering(eMaterialRenderType mType)=0;
		/**
		 * Return the data types needed for rendering.
		 * \param mType
		 * \return
		 */
		virtual tVtxBatchFlag GetBatchFlags(eMaterialRenderType mType)=0;

		/**
		 * Set new states and return true for another pass
		 * \param mType
		 * \return
		 */
		virtual bool NextPass(eMaterialRenderType mType)=0;
		/**
		 * return true if the program has multple passes
		 * \param mType
		 * \return
		 */
		virtual bool HasMultiplePasses(eMaterialRenderType mType)=0;



		void SetProgram(iGpuProgram* apProgram, unsigned int alNum){
						mpProgram[alNum] = apProgram;}
		iGpuProgram* GetProgram(unsigned int alNum){
						return mpProgram[alNum];}

		/**
		 * return true if the material is transparent
		 * \return
		 */
		virtual bool IsTransperant() {return mbIsTransperant;}
		virtual bool IsGlowing() { return mbIsGlowing; }
		/**
		 * return true if the material has a light pass
		 * \return
		 */
		virtual bool UsesLights(){ return mbUsesLights; }

		virtual tTextureTypeList GetTextureTypes(){
			tTextureTypeList vTypes;
			vTypes.push_back(cTextureType("",eMaterialTexture_Diffuse));
			return vTypes;
		}

		/**
		 * The type of the material
		 * \param alId
		 */
		void SetId(int alId){ mlId = alId;}
		int GetId(){ return mlId;}

		virtual iGpuProgram *GetRefractionProgam() { return NULL; }

		virtual bool GetRefractionUsesDiffuse(){ return false;}
		virtual eMaterialTexture GetRefractionDiffuseTexture(){ return eMaterialTexture_Diffuse;}

		virtual bool GetRefractionUsesEye(){ return false;}
		virtual bool GetRefractionUsesTime(){ return false;}
		virtual bool GetRefractionSkipsStandardTrans(){ return false;}

		const tString& GetPhysicsMaterial(){ return msPhysicsMaterial;}
		void SetPhysicsMaterial(const tString& asName){ msPhysicsMaterial = asName;}

		static void SetQuality(eMaterialQuality aQuality){ mQuality = aQuality;}
		static eMaterialQuality GetQuality(){ return mQuality;}

		/**
		 * This is used so that materials do not call type specific things after types have been destroyed!
		 * Shall only be set by graphics!
		 */
		static void SetDestroyTypeSpecifics(bool abX) { mbDestroyTypeSpecifics = abX; }
		static bool GetDestroyTypeSpecifics() { return mbDestroyTypeSpecifics; }

		//resources stuff.
		bool Reload() { return false; }
		void Unload() {}
		void Destroy() {}

	protected:
		iLowLevelGraphics* mpLowLevelGraphics;
		cImageManager* mpImageManager;
		cTextureManager* mpTextureManager;
		cRenderer2D* mpRenderer;
		cRenderer3D* mpRenderer3D;
		cRenderSettings *mpRenderSettings;
		cGpuShaderManager* mpShaderManager;

		iMaterialType *mpType = nullptr;

		static eMaterialQuality	mQuality;

		bool mbIsTransperant;
		bool mbIsGlowing;
		bool mbUsesLights;
		bool mbHasAlpha;
		bool mbDepthTest;
		float mfValue;

		int mlId;

		tString msPhysicsMaterial;

		tTextureVec mvTexture;
		tResourceImageVec mvImage;

		eMaterialType mType;
		eMaterialPicture mPicture;

		iGpuProgram *mpProgram[kMaxProgramNum];

		int mlPassCount;

		//void Destroy();

		static bool mbDestroyTypeSpecifics;
	};

	typedef std::vector<iMaterial*> tMaterialVec;
	typedef tMaterialVec::iterator tMaterialVecIt;
};
#endif // HPL_MATERIAL_H
