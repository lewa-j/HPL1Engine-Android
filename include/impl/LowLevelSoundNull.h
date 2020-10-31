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
#ifndef HPL_LOWLEVELSOUND_NULL_H
#define HPL_LOWLEVELSOUND_NULL_H

#include "sound/LowLevelSound.h"

namespace hpl
{
	class cLowLevelSoundNull : public iLowLevelSound
	{
	public:
		cLowLevelSoundNull(){}
		~cLowLevelSoundNull(){}

		void GetSupportedFormats(tStringList &alstFormats){}

		iSoundData* LoadSoundData(const tString& asName,const tString& asFilePath,
									const tString& asType, bool abStream,bool abLoopStream){return nullptr;}

		void UpdateSound(float afTimeStep){}

		void SetListenerAttributes (const cVector3f &avPos,const cVector3f &avVel,
								const cVector3f &avForward,const cVector3f &avUp){}
		void SetListenerPosition(const cVector3f &avPos){}

		void SetSetRolloffFactor(float afFactor){}

		void SetListenerAttenuation (bool abEnabled){}

//		void LogSoundStatus();

		void Init ( bool abUseHardware, bool abForceGeneric, bool abUseEnvAudio, int alMaxChannels,
					int alStreamUpdateFreq, bool abUseThreading, bool abUseVoiceManagement,
					int alMaxMonoSourceHint, int alMaxStereoSourceHint,
					int alStreamingBufferSize, int alStreamingBufferCount, bool abEnableLowLevelLog,
					tString asDeviceName
					){}

		void SetVolume(float afVolume){}

		void SetEnvVolume( float afEnvVolume ){}

		iSoundEnvironment* LoadSoundEnvironment (const tString& asFilePath){return nullptr;}
		void SetSoundEnvironment ( iSoundEnvironment* apSoundEnv ){}
		void FadeSoundEnvironment( iSoundEnvironment* apSourceSoundEnv, iSoundEnvironment* apDestSoundEnv, float afT ){}

	private:

	};
};
#endif // HPL_LOWLEVELSOUND_NULL_H

