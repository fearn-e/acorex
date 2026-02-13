#pragma once
// TODO -   store global defaults in xml?
//          and per-corpus in the corpus JSONs?
//          then no longer need this file or these defines, as everything can be loaded in place

#include <ofConstants.h>

//default global settings
#define DEFAULT_ANALYSE_OPEN false
#define DEFAULT_EXPLORE_OPEN true
#define DEFAULT_HI_DPI false

//default explorer settings values (could have both default globals an per-corpus "last used")
#define DEFAULT_DIMENSION_X 0 // these could be set by name in per-corpus settings
#define DEFAULT_DIMENSION_Y 0
#define DEFAULT_DIMENSION_Z 0
#define DEFAULT_DIMENSION_COLOR 0
#define DEFAULT_DIMENSION_DYNAMIC_PAN 0
#define DEFAULT_COLOR_SPECTRUM false
#define DEFAULT_LOOP_PLAYHEADS true
#define DEFAULT_JUMP_SAME_FILE_ALLOWED true
#define DEFAULT_JUMP_SAME_FILE_MIN_DIFF 2
#define DEFAULT_CROSSOVER_JUMP_CHANCE_X1000 950 // out of 1000
#define DEFAULT_CROSSFADE_MAX_SAMPLE_LENGTH 16384
#define DEFAULT_MAX_JUMP_DISTANCE_SPACE_X1000 50 // out of 1000
#define DEFAULT_MAX_JUMP_TARGETS 5
#define DEFAULT_VOLUME_X1000 500 // out of 1000
#define DEFAULT_PANNING_STRENGTH_X1000 1000 // out of 1000

// default explorer audio device settings - store globally (xml?)
//#ifdef TARGET_LINUX
//    #define DEFAULT_API ofSoundDevice::ALSA
//#elif defined(TARGET_OSX)
//    #define DEFAULT_API ofSoundDevice::OSX_CORE
//#elif defined(TARGET_WIN32)
//    #define DEFAULT_API ofSoundDevice::MS_WASAPI
//#endif
#define DEFAULT_API 0
//#define DEFAULT_OUT_DEVICE_NAME ""
#define DEFAULT_OUT_DEVICE_INDEX 0
#define DEFAULT_BUFFER_SIZE 512


// default analysis settings - store globally (xml?)
// already kind of exists in Data.h in struct AnalysisSettings
#define DEFAULT_ANALYSE_PITCH false
#define DEFAULT_ANALYSE_LOUDNESS false
#define DEFAULT_ANALYSE_SPEC_SHAPE true
#define DEFAULT_ANALYSE_MFCC true
#define DEFAULT_ANALYSE_SAMPLE_RATE 44100
#define DEFAULT_ANALYSE_WINDOW_SIZE 4096
#define DEFAULT_ANALYSE_HOP_SIZE_FRACTION 2 //means default is 512 (1024/2)
#define DEFAULT_ANALYSE_MFCC_BANDS 40
#define DEFAULT_ANALYSE_MFCC_COEFS 13
#define DEFAULT_ANALYSE_MIN_FREQ 20
#define DEFAULT_ANALYSE_MAX_FREQ 5000

#define DEFAULT_ANALYSE_INSERT_FILES_REPLACE false

#define DEFAULT_REDUCE_DIMENSIONS 4
#define DEFAULT_MAX_TRAINING_ITERATIONS 200