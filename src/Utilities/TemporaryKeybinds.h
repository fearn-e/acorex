#pragma once

// TODO - store properly in xml or json, add rebinding menu

//#include <ofConstants.h>
#include <ofEvents.h>


//LiveView key binds
#define ACOREX_KEYBIND_CAMERA_MOVE_FORWARD  'w'
#define ACOREX_KEYBIND_CAMERA_MOVE_LEFT     'a'
#define ACOREX_KEYBIND_CAMERA_MOVE_BACKWARD 's'
#define ACOREX_KEYBIND_CAMERA_MOVE_RIGHT    'd'
#define ACOREX_KEYBIND_CAMERA_MOVE_UP       'r'
#define ACOREX_KEYBIND_CAMERA_MOVE_DOWN     'f' 
// TODO - double check these 4 lol
#define ACOREX_KEYBIND_CAMERA_ROTATE_LEFT   'q' //? LEFT OR RIGHT?
#define ACOREX_KEYBIND_CAMERA_ROTATE_RIGHT  'e' //? LEFT OR RIGHT?
#define ACOREX_KEYBIND_CAMERA_ZOOM_IN       'z' //? IN OR OUT?
#define ACOREX_KEYBIND_CAMERA_ZOOM_OUT      'x' //? IN OR OUT?

#define ACOREX_KEYBIND_CREATE_PLAYHEAD_ZERO_ZERO         '1'
#define ACOREX_KEYBIND_CREATE_PLAYHEAD_RANDOM_POINT      '2'
#define ACOREX_KEYBIND_CREATE_PLAYHEAD_PICKER_POINT      '3'

#define ACOREX_KEYBIND_AUDIO_PAUSE          ' '

//PointPicker key binds
#define ACOREX_KEYBIND_TOGGLE_DEBUG_VIEW OF_KEY_F3

#define ACOREX_KEYBIND_PICK_RANDOM_POINT '4'

#define ACOREX_KEYBIND_TOGGLE_POINT_PICKER OF_KEY_TAB



// Mouse Bind Reference:
// PointPicker - OF_MOUSE_BUTTON_RIGHT, MouseReleased - select point