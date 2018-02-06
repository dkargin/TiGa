#pragma once

namespace Fx
{
	typedef int32_t HEFFECT;
	typedef int32_t HCHANNEL;

	class SoundManager
	{
	public:
		void Effect_Free(HEFFECT effect);
		HEFFECT Effect_Load(const char* path);
		HCHANNEL Effect_Play(HEFFECT sound);
		void Channel_Stop(HCHANNEL channel);
		Time_t Channel_GetLength(HCHANNEL channel);
		void Channel_SetVolume(HCHANNEL channel, float volume);
	};
}
