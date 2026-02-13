#pragma once

#include "./AudioSettingsManager.h"
#include <ofUtils.h>
#include <ofAppRunner.h>
#include <ofLog.h>

using namespace Acorex;

Utils::AudioSettingsManager::AudioSettingsManager ( )
    : ofApiReferenceIndices {
        ofSoundDevice::UNSPECIFIED, // 0
        ofSoundDevice::DEFAULT,     // 1
        ofSoundDevice::ALSA,        // 2
        ofSoundDevice::PULSE,       // 3
        ofSoundDevice::OSS,         // 4
        ofSoundDevice::JACK,        // 5
        ofSoundDevice::OSX_CORE,    // 6
        ofSoundDevice::MS_WASAPI,   // 7
        ofSoundDevice::MS_ASIO,     // 8
        ofSoundDevice::MS_DS        // 9
    }, ofApiReferenceNames {
        "Unspecified",
        "Default",
        "ALSA",
        "PulseAudio",
        "Linux OSS",
        "Jack Audio",
        "OS-X Core Audio",
        "WASAPI",
        "ASIO",
        "DirectSound"
    }, availablePlatformApis {
    #ifdef TARGET_LINUX
        2, 3, 4, 5 // ALSA, PULSE, OSS, JACK
    #elif defined(TARGET_OSX)
        6 // OSX_CORE
    #elif defined(TARGET_WIN32)
        7, 8, 9 // MS_WASAPI, MS_ASIO, MS_DS
    #endif
    }, availablePlatformApiNames {
    #ifdef TARGET_LINUX
        "ALSA", "PulseAudio", "Linux OSS", "Jack Audio"
    #elif defined(TARGET_OSX)
        "OS-X Core Audio"
    #elif defined(TARGET_WIN32)
        "WASAPI", "ASIO", "DirectSound"
    #endif
    }, availableBufferSizes {
        64, 128, 256, 512, 1024, 2048, 4096, 8192
    }
{
    audioDevicesOut.clear ( );

    currentApiIndex = DEFAULT_API;
    currentAudioDeviceIndex = DEFAULT_OUT_DEVICE_INDEX;
    currentBufferSize = DEFAULT_BUFFER_SIZE;

    ScanAndPopulateAllDevicesOut ( );

    for ( size_t i = 0; i < availablePlatformApis.size ( ); i++ )
    {
        if ( audioDevicesOut[i].size ( ) > 1 )
        {
            currentApiIndex = i;
            break;
        }
    }

    bool defaultDeviceFound = FindDefaultDeviceIndex ( currentApiIndex, currentAudioDeviceIndex );
    if ( !defaultDeviceFound ) { currentAudioDeviceIndex = DEFAULT_OUT_DEVICE_INDEX; }
};

bool Utils::AudioSettingsManager::RefreshDeviceListChanged ( )
{
    std::vector<std::vector<ofSoundDevice>> oldAudioDevicesOut = audioDevicesOut;

    ScanAndPopulateAllDevicesOut ( );

    bool deviceListChanged = false;

    for ( size_t i = 0; i < audioDevicesOut.size ( ); i++ )
    {
        if ( audioDevicesOut[i].size ( ) != oldAudioDevicesOut[i].size ( ) )
        {
            deviceListChanged = true;
            break;
        }

        for ( size_t j = 0; j < audioDevicesOut[i].size ( ); j++ )
        {
            // TODDO - try matching via ID instead of name?
            if ( audioDevicesOut[i][j].name == oldAudioDevicesOut[i][j].name ) { continue; }

            deviceListChanged = true;
            break;
        }

        if ( deviceListChanged ) { break; }
    }

    if ( !deviceListChanged ) { return false; }

    ofLogNotice ( ) << "AudioSettingsManager::RefreshDeviceListChanged - device list changed, devices found: " << audioDevicesOut[currentApiIndex].size ( ) - 1;

    bool deviceSet = false;
    deviceSet = FindMatchingDevice ( oldAudioDevicesOut[currentApiIndex][currentAudioDeviceIndex], currentApiIndex, currentAudioDeviceIndex );
    if ( !deviceSet ) { deviceSet = FindDefaultDeviceIndex ( currentApiIndex, currentAudioDeviceIndex ); }
    if ( !deviceSet ) { currentAudioDeviceIndex = 0; }

    return true;
}

bool Utils::AudioSettingsManager::ChangeSelectedApi ( size_t newApiIndex )
{
    if ( newApiIndex >= availablePlatformApis.size ( ) || newApiIndex < 0 )
    {
        ofLogError ( ) << "AudioSettingsManager::ChangeSelectedApi - apiIndex out of range: " << newApiIndex;
        currentApiIndex = 0;
        currentAudioDeviceIndex = 0;
        return false;
    }

    bool deviceSet = false;
    deviceSet = FindMatchingDevice ( audioDevicesOut[currentApiIndex][currentAudioDeviceIndex], newApiIndex, currentAudioDeviceIndex );
    if ( !deviceSet ) { deviceSet = FindDefaultDeviceIndex ( newApiIndex, currentAudioDeviceIndex ); }
    if ( !deviceSet ) { currentAudioDeviceIndex = 0; }

    currentApiIndex = newApiIndex;
    return true;
}

bool Utils::AudioSettingsManager::ChangeSelectedDevice ( size_t newDeviceIndex )
{
    if ( newDeviceIndex >= audioDevicesOut[currentApiIndex].size ( ) || newDeviceIndex < 0 )
    {
        ofLogError ( ) << "AudioSettingsManager::ChangeSelectedDevice - deviceIndex out of range: " << newDeviceIndex;
        currentAudioDeviceIndex = 0;
        return false;
    }

    currentAudioDeviceIndex = newDeviceIndex;
    return true;
}

void Utils::AudioSettingsManager::ScanAndPopulateAllDevicesOut ( )
{
    audioDevicesOut.clear ( );

    audioDevicesOut.resize ( availablePlatformApis.size ( ) );

    for ( size_t i = 0; i < availablePlatformApis.size ( ); i++ )
    {
        ofSoundDevice nullDevice { };
        nullDevice.api = ofApiReferenceIndices[availablePlatformApis[i]];
        nullDevice.name = "No output selected.";
        nullDevice.deviceID = -1;
        audioDevicesOut[i].push_back ( nullDevice );

        std::vector<ofSoundDevice> tempDevices = mDeviceSoundStream.getDeviceList ( ofApiReferenceIndices[availablePlatformApis[i]] );

        if ( tempDevices.size ( ) == 0 ) { continue; }

        for ( auto& device : tempDevices )
        {
            if ( device.outputChannels == 0 ) { continue; }

            audioDevicesOut[i].push_back ( device );
        }
    }
}

bool Utils::AudioSettingsManager::FindMatchingDevice ( const ofSoundDevice& referenceDevice, size_t apiIndex, size_t& matchingDeviceIndex ) const
{
    for ( size_t i = 0; i < audioDevicesOut[apiIndex].size ( ); i++ )
    {
        // TODO - replace with ID matching instead?
        if ( audioDevicesOut[apiIndex][i].name == referenceDevice.name )
        {
            ofLogNotice ( ) << "AudioSettingsManager::FindMatchingDevice - found matching device: " << referenceDevice.name;
            matchingDeviceIndex = i;
            return true;
        }
    }

    return false;
}

bool Utils::AudioSettingsManager::FindDefaultDeviceIndex ( size_t apiIndex, size_t& defaultDeviceIndex ) const
{
    for ( size_t i = 0; i < audioDevicesOut[apiIndex].size ( ); i++ )
    {
        if ( audioDevicesOut[apiIndex][i].isDefaultOutput )
        {
            ofLogNotice ( ) << "AudioSettingsManager::FindDefaultDeviceIndex - found default device: " << audioDevicesOut[apiIndex][i].name;
            defaultDeviceIndex = i;
            return true;
        }
    }
    return false;
}