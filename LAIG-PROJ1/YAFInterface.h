#ifndef _YAFINTERFACE_H
#define _YAFINTERFACE_H

#include "CGFinterface.h"
#include "YAFScene.h"

#define MOUSE_ROTATE_FACTOR 0.5
#define MOUSE_PAN_FACTOR 0.05
#define MOUSE_ZOOM_FACTOR 0.5

/*#define LIGHT_IDS_BEGIN 0
#define CAMERAS_ID 10
#define RESET_CAMERA_ID 11
#define CAMERA_MODE_ID 12
#define RENDER_MODE_ID 13
#define USE_DL_ID 14
#define LOOP_ANIMATIONS_ID 15
#define RESET_ANIMATIONS_ID 16
#define TOGGLE_ANIMATIONS_ID 17*/

#define RESTART_BUTTON_ID 0
#define PLAYER_1_ID 1
#define PLAYER_2_ID 2
#define CAMERA_ROTATION_TOGGLE_ID 3
#define PIECE_ANIMATION_TOGGLE_ID 4
#define APPEARANCES_ID 5
#define UNDO_BUTTON_ID 6
#define WATCH_REPLAY_ID 7
#define SAVE_BUTTON_ID 8
#define LOAD_BUTTON_ID 9
#define PAUSE_BUTTON_ID 10
#define GAME_SCENES_ID 11
#define CAMERA_ROTATION_ID 12
#define SKIP_REPLAY_ID 13
#define FILE_SLOTS_ID 14
#define ENABLE_TIMEOUTS_ID 15
#define TIMEOUT_VALUE 16
#define VOICE_COMMANDS_ID 17

class YAFInterface: public CGFinterface {
	GLUI_Rotation * rot;
	void processMouseMoved(int x, int y);
	void performPicking(int x, int y);
	void processHits (GLint hits, GLuint buffer[]);
	vector<pair<string, YAFReader> > yafFiles;
	int selectedSaveSlot;
public:
	YAFInterface(vector<pair<string, YAFReader> > &yafFiles);
	void initGUI();
	void processGUI(GLUI_Control *ctrl);
	void processMouse(int button, int state, int x, int y);
	void processKeyboard(unsigned char key, int x, int y);
};


#endif