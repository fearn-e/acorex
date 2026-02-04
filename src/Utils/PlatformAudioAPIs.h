#pragma once

#include "ofSoundBaseTypes.h"

namespace Acorex {

#ifdef TARGET_LINUX
    #define ACOREX_PLATFORM_AUDIO_APIS ofSoundDevice::ALSA, ofSoundDevice::PULSE, ofSoundDevice::OSS, ofSoundDevice::JACK
#elif defined(TARGET_OSX)
    #define ACOREX_PLATFORM_AUDIO_APIS ofSoundDevice::OSX_CORE
#elif defined(TARGET_WIN32)
    #define ACOREX_PLATFORM_AUDIO_APIS ofSoundDevice::MS_WASAPI, ofSoundDevice::MS_ASIO, ofSoundDevice::MS_DS
#endif

} // namespace Acorex