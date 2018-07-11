#pragma once
#include <cstdint>
#include "basetypes.h"

namespace Fx
{
	typedef int32_t HEFFECT;
	typedef int32_t HCHANNEL;

	class SoundManager
	{
	public:
		void free(HEFFECT effect);
		HEFFECT load(const char* path);
		HCHANNEL play(HEFFECT sound);
		void stop(HCHANNEL channel);
		Time_t getLength(HCHANNEL channel);
		void setVolume(HCHANNEL channel, float volume);
	};
}
