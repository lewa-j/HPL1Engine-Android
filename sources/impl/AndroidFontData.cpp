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
#include "impl/AndroidFontData.h"
#include "impl/AndroidBitmap2D.h"
#include "graphics/LowLevelGraphics.h"
#include "impl/tinyXML/tinyxml.h"

namespace hpl
{

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------


	cAndroidFontData::cAndroidFontData(const tString &asName, iLowLevelGraphics* apLowLevelGraphics)
		: iFontData(asName,apLowLevelGraphics)
	{
		
	}

	//-----------------------------------------------------------------------

	
	cAndroidFontData::~cAndroidFontData()
	{
		
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cAndroidFontData::CreateFromBitmapFile(const tString &asFileName)
	{
		tString sPath = cString::GetFilePath(asFileName);

		////////////////////////////////////////////
		// Load xml file
		TiXmlDocument *pXmlDoc = hplNew( TiXmlDocument,(asFileName.c_str()) );

		if(pXmlDoc->LoadFile()==false)
		{
			Error("Couldn't load angel code font file '%s'\n",asFileName.c_str());
			hplDelete(pXmlDoc);
			return false;
		}

		TiXmlElement *pRootElem = pXmlDoc->RootElement();

		////////////////////////////////////////////
		// Load Common info
		TiXmlElement *pCommonElem = pRootElem->FirstChildElement("common");

		int lLineHeight = cString::ToInt(pCommonElem->Attribute("lineHeight"),0);
		int lBase = cString::ToInt(pCommonElem->Attribute("base"),0);

		mlFirstChar = 0;
		mlLastChar = 3000; //Get this num from something.

		mfHeight = (float)lLineHeight;

		mvGlyphs.resize(3000, NULL);

		mvSizeRatio.x = (float)lBase / (float)lLineHeight;
		mvSizeRatio.y = 1;

		////////////////////////////////////////////
		// Load bitmaps
		std::vector<cAndroidBitmap2D *> vBitmaps;

		TiXmlElement *pPagesRootElem = pRootElem->FirstChildElement("pages");

		TiXmlElement *pPageElem = pPagesRootElem->FirstChildElement("page");
		for(; pPageElem != NULL; pPageElem = pPageElem->NextSiblingElement("page"))
		{
			tString sFileName = pPageElem->Attribute("file");
			tString sFilePath = cString::SetFilePath(sFileName,sPath);

			cAndroidBitmap2D *pBitmap = static_cast<cAndroidBitmap2D*>(mpLowLevelResources->LoadBitmap2D(sFilePath));
			if(pBitmap==NULL)
			{
				Error("Couldn't load bitmap %s for FNT file '%s'\n",sFilePath.c_str(),asFileName.c_str());
				hplDelete(pXmlDoc);
				return false;
			}

			vBitmaps.push_back(pBitmap);
		}

		////////////////////////////////////////////
		// Load glyphs
		TiXmlElement *pCharsRootElem = pRootElem->FirstChildElement("chars");

		TiXmlElement *pCharElem = pCharsRootElem->FirstChildElement("char");
		for(; pCharElem != NULL; pCharElem = pCharElem->NextSiblingElement("char"))
		{
			//Get the info on the character
			int lId = cString::ToInt(pCharElem->Attribute("id"),0);
			int lX = cString::ToInt(pCharElem->Attribute("x"),0);
			int lY = cString::ToInt(pCharElem->Attribute("y"),0);

			int lW = cString::ToInt(pCharElem->Attribute("width"),0);
			int lH = cString::ToInt(pCharElem->Attribute("height"),0);

			int lXOffset = cString::ToInt(pCharElem->Attribute("xoffset"),0);
			int lYOffset = cString::ToInt(pCharElem->Attribute("yoffset"),0);

			int lAdvance = cString::ToInt(pCharElem->Attribute("xadvance"),0);

			int lPage = cString::ToInt(pCharElem->Attribute("page"),0);

			//Get the bitmap where the character graphics is
			cAndroidBitmap2D *pSourceBitmap = vBitmaps[lPage];

			//Create a bitmap for the character.
			cVector2l vSize(lW, lH);
			cAndroidBitmap2D *pBmp = static_cast<cAndroidBitmap2D*>(mpLowLevelGraphics->CreateBitmap2D(vSize,32));

			//Copy from source to character bitmap
			int srcSize = pSourceBitmap->GetNumChannels();
			unsigned char* srcBuffer = (unsigned char*)pSourceBitmap->GetRawData();
			int lBmpSize = pBmp->GetNumChannels();
			unsigned char* PixBuffer = (unsigned char*)pBmp->GetRawData();

			for(unsigned int y=0;y<lH;y++)
				for(unsigned int x=0;x<lW;x++) {
					uint8_t *Pix = &PixBuffer[(y*lW + x)*lBmpSize];
					uint8_t *src = &srcBuffer[((lY+y)*pSourceBitmap->GetWidth()+lX+x)*srcSize];
					Pix[0] = src[0];
					Pix[1] = src[1];
					Pix[2] = src[2];
					//Set proper alpha (dunno if this is needed)
					Pix[3] = Pix[0];
				}

			//Create glyph and place it correctly.
			cGlyph *pGlyph = CreateGlyph(pBmp,cVector2l(lXOffset,lYOffset),cVector2l(lW,lH),
										cVector2l(lBase,lLineHeight),lAdvance);

			mvGlyphs[lId] = pGlyph;

			hplDelete(pBmp);
		}

		//Destroy bitmaps
		STLDeleteAll(vBitmaps);

		//Destroy XML
		hplDelete(pXmlDoc);
		return true;
	}

	//-----------------------------------------------------------------------

	bool cAndroidFontData::CreateFromFontFile(const tString &asFileName, int alSize,unsigned short alFirstChar,
								unsigned short alLastChar)
	{
		return false;
	}

	//-----------------------------------------------------------------------

}

