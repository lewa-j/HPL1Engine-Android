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
#include "sound/SoundData.h"
#include "sound/SoundChannel.h"

namespace hpl
{

	class cSoundChannelNull : public iSoundChannel
	{
	public:
		cSoundChannelNull(iSoundData* apData, cSoundManager* apSoundManger) : iSoundChannel(apData, apSoundManger)
		{}
		~cSoundChannelNull(){}

		void Play(){}
		void Stop(){}

		void SetPaused(bool abX){}
		void SetSpeed(float afSpeed){}
		void SetVolume (float afVolume){}
		void SetLooping (bool abLoop){}
		void SetPan (float afPan){}
		void Set3D(bool ab3D){}

		void SetPriority(int alX){}
		int GetPriority(){return 0;}

		void SetPositionRelative(bool abRelative){}
		void SetPosition(const cVector3f &avPos){}
		void SetVelocity(const cVector3f &avVel){}

		void SetMinDistance(float fMin){}
		void SetMaxDistance(float fMax){}

		bool IsPlaying(){return false;}

		bool IsBufferUnderrun() { return false;}
		double GetElapsedTime()	{ return 0;}
		double GetTotalTime()	{ return 1;}

		void SetAffectedByEnv(bool abAffected){}
		void SetFiltering ( bool abEnabled, int alFlags){}
		void SetFilterGain(float afGain){}
		void SetFilterGainHF(float afGainHF){}
	};

	class cSoundDataNull : public iSoundData
	{
	public:
		cSoundDataNull(tString asName, bool abStream) : iSoundData(asName, abStream)
		{}
		~cSoundDataNull(){}
		bool CreateFromFile(const tString &asFile){return true;}
		iSoundChannel* CreateChannel(int alPriority){return hplNew(cSoundChannelNull,(this, mpSoundManger));}
		bool IsStereo(){return false;}
	};

	class cLowLevelSoundNull : public iLowLevelSound
	{
	public:
		cLowLevelSoundNull(){
			mvFormats[0] = "WAV";
			mvFormats[1] = "OGG";
			mvFormats[2] = "";
		}
		~cLowLevelSoundNull(){}

		void GetSupportedFormats(tStringList &alstFormats){
			int lPos = 0;

			while(mvFormats[lPos][0] != '\0')
			{
				alstFormats.push_back(mvFormats[lPos]);
				lPos++;
			}
		}

		iSoundData* LoadSoundData(const tString& asName,const tString& asFilePath,
									const tString& asType, bool abStream,bool abLoopStream){return hplNew(cSoundDataNull,(asName,abStream));}

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
		tString mvFormats[3];
	};
};
#endif // HPL_LOWLEVELSOUND_NULL_H

