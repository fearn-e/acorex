/*
The MIT License (MIT)

Copyright (c) 2026-2026 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Utilities/TemporaryDefaults.h"

#include <ofConstants.h>
#include <ofSoundBaseTypes.h>
#include <ofSoundStream.h>
#include <array>
#include <string_view>

// TODO - function to find a device in the list of devices marked as "default"

#define CHECK_ENUM_VALUE(ENUM, VALUE, EXPECTED ) \
    static_assert(static_cast<int>(ENUM::VALUE) == EXPECTED, #ENUM "::" #VALUE " has changed, expected " #EXPECTED);

#if 0
// set to 1 to find places where enum code needs to be updated, then set back to 0
#define ACOREX_OF_API_ENUM_CHANGED
#endif
// if any of these fail, check if ofSoundDevice::Api implementation has changed, and set 0 above to 1
CHECK_ENUM_VALUE(ofSoundDevice::Api, UNSPECIFIED, 0)
CHECK_ENUM_VALUE(ofSoundDevice::Api, DEFAULT, 1)
CHECK_ENUM_VALUE(ofSoundDevice::Api, ALSA, 2)
CHECK_ENUM_VALUE(ofSoundDevice::Api, PULSE, 3)
CHECK_ENUM_VALUE(ofSoundDevice::Api, OSS, 4)
CHECK_ENUM_VALUE(ofSoundDevice::Api, JACK, 5)
CHECK_ENUM_VALUE(ofSoundDevice::Api, OSX_CORE, 6)
CHECK_ENUM_VALUE(ofSoundDevice::Api, MS_WASAPI, 7)
CHECK_ENUM_VALUE(ofSoundDevice::Api, MS_ASIO, 8)
CHECK_ENUM_VALUE(ofSoundDevice::Api, MS_DS, 9)
CHECK_ENUM_VALUE(ofSoundDevice::Api, NUM_APIS, 10)

namespace Acorex {
namespace Utilities {

class AudioSettingsManager {
public:

    AudioSettingsManager ( );
    ~AudioSettingsManager ( ) { }

    //void Update ( );

    //bool IsDeviceChangeDetected ( ) const { return bDeviceChangeDetected; }

    /// <summary>Rescans device list.</summary>
    /// <returns>False if no changes detected. True if device list has changed.</returns>
    bool RefreshDeviceListChanged ( );
    void ResetCurrentSettings ( ) { currentApiIndex = DEFAULT_API; currentAudioDeviceIndex = DEFAULT_OUT_DEVICE_INDEX; currentBufferSize = DEFAULT_BUFFER_SIZE; }

    bool ChangeSelectedApi ( size_t newApiIndex );
    bool ChangeSelectedDevice ( size_t newDeviceIndex );

    void SetBufferSize ( int bufferSize ) { currentBufferSize = bufferSize; }

    bool HasValidOutput ( ) const { return audioDevicesOut[currentApiIndex].size ( ) > 1; }
    std::vector<ofSoundDevice> GetCurrentApiDevicesOut ( ) const { return audioDevicesOut[currentApiIndex]; }
    std::pair<ofSoundDevice, int> GetCurrentAudioSettings ( ) const { return { audioDevicesOut[currentApiIndex][currentAudioDeviceIndex], currentBufferSize }; }
    size_t GetCurrentApiIndex ( ) const { return currentApiIndex; }
    size_t GetCurrentDeviceIndex ( ) const { return currentAudioDeviceIndex; }
    int GetCurrentBufferSize ( ) const { return currentBufferSize; }

    std::string_view GetCurrentApiName ( ) const { return availablePlatformApiNames[currentApiIndex]; }
    std::string_view GetApiName ( size_t apiIndex ) const { return availablePlatformApiNames[apiIndex]; }
    std::vector<size_t> GetApis ( ) const { return availablePlatformApis; }
    size_t GetApiCount ( ) const { return availablePlatformApis.size ( ); }
    std::vector<ofSoundDevice> GetOutDevices ( size_t apiIndex ) const { return audioDevicesOut[apiIndex]; }
    size_t GetOutDeviceCount ( size_t apiIndex ) const { return audioDevicesOut[apiIndex].size ( ); }
    std::vector<int> GetBufferSizes ( ) const { return availableBufferSizes; }
    size_t GetBufferSizeCount ( ) const { return availableBufferSizes.size ( ); }

private:
    /// <summary>
    /// Scans and repopulates list of all output audio devices for platform specific APIs.
    /// <para>&#160;</para>
    /// The 0th index of every API vector is "No output selected."
    /// </summary>
    void ScanAndPopulateAllDevicesOut ( );

    bool FindMatchingDevice ( const ofSoundDevice& referenceDevice, size_t apiIndex, size_t& matchingDeviceIndex ) const;
    bool FindDefaultDeviceIndex ( size_t apiIndex, size_t& defaultDeviceIndex ) const;

    //void CheckAudioDevices ( );

    //bool bDeviceChangeDetected;

    //std::vector<ofSoundDevice> mLastDeviceCheck;
    //float mLastDeviceCheckTime;
    //const float mDeviceCheckInterval;

    ofSoundStream mDeviceSoundStream;

    std::vector<std::vector<ofSoundDevice>> audioDevicesOut;

    size_t currentApiIndex; size_t currentAudioDeviceIndex; int currentBufferSize;

#ifndef ACOREX_OF_API_ENUM_CHANGED
    const std::array<ofSoundDevice::Api, ofSoundDevice::NUM_APIS> ofApiReferenceIndices;
    const std::array<std::string_view, ofSoundDevice::NUM_APIS> ofApiReferenceNames;
    const std::vector<size_t> availablePlatformApis;
    const std::vector<std::string_view> availablePlatformApiNames;
#endif
    const std::vector<int> availableBufferSizes;
};

} // namespace Utilities
} // namespace Acorex

// TODO - find default device in list

/*
    bool deviceSelected = false; size_t deviceIndex = 0;
    {
        for ( size_t i = 0; i < outDevices.size ( ); i++ )
        {
            if ( outDevices[i].name == currentOutDevice.name )
            {
                deviceSelected = true;
                deviceIndex = i;
                break;
            }
        }

        size_t defaultDeviceIndex;
        if ( !deviceSelected && FindDefaultDeviceIndexInList ( outDevices, defaultDeviceIndex ) )
        {
            deviceSelected = true;
            deviceIndex = defaultDeviceIndex;
        }
    }
*/

/*
ofSoundDevice GetDefaultDeviceForApi ( ofSoundDevice::Api api ) { return outDevices[static_cast<size_t> (api)][0]; } // TODO - later, save/load last used

bool Acorex::ExplorerMenu::FindDefaultDeviceIndexInList ( const std::vector<ofSoundDevice>& devices, size_t& outIndex )
{
    for ( size_t i = 0; i < devices.size ( ); i++ )
    {
        if ( devices[i].isDefaultOutput )
        {
            outIndex = i;
            return true;
        }
    }
    outIndex = 0;
    return false;
}


bool Acorex::ExplorerMenu::FindDefaultDeviceIndexInList ( const std::vector<ofSoundDevice>& devices, size_t& outIndex );


    // Find APIs and Audio Devices -------------------------- // TODO - move these into their own function calls
if ( bFirstTimeAPIDeviceLoad )
{
    // APIs

    apiNames.clear ( );
    currentAPI = ofSoundDevice::Api ( );

    ofSoundStream temp;
    std::vector<ofSoundDevice::Api> platformAPIs = { ACOREX_PLATFORM_AUDIO_APIS };

    for ( int i = 0; i < platformAPIs.size ( ); i++ )
    {
        apiNames.push_back ( std::string ( Utilities::AudioAPINames[static_cast<size_t> ( platformAPIs[i] )] ) );
    }

    currentAPI = defaultApi;
    // TODO - might be overkill, but error if defaultApi doesn't exist on apiNames (shouldn't really ever happen)

    // Devices

    outDevices.clear ( );
    currentOutDevice = ofSoundDevice ( );

    outDevices = GetOutDevicesForApi ( currentAPI );

    if ( outDevices.empty ( ) )
    {
        //TODO - KILL AUDIO? audioplayback probably will need to have a bool state for this case
    }
    else
    {
        size_t tempDeviceIndex;
        if ( FindDefaultDeviceIndexInList ( outDevices, tempDeviceIndex ) )
        { defaultOutDeviceIndex = tempDeviceIndex; }
    }

    bFirstTimeAPIDeviceLoad = false;
}

*/