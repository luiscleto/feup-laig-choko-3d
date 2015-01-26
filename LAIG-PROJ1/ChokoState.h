#ifndef CHOKO_STATE_H
#define CHOKO_STATE_H

#define NUM_PIECES 12
#define HUD_CAMERA_HEIGHT 20
#define HUD_CAMERA_WIDTH 20
#define HUD_TOP_Y_POS 19
#define HUD_PLAYERS_Y_POS 5
#define HUD_PIECES_Y_POS 3
#define HUD_PLAYER_P1_X_POS 3
#define HUD_PLAYER_P2_X_POS 14
#define HUD_TURN_X_POS 7.2
#define HUD_P1_PIECES_X_POS 2
#define HUD_P2_PIECES_X_POS 13

#define P1_PANEL_X1 -90.0f
#define P1_PANEL_X2 -40.0f
#define P2_PANEL_X1 40.0f
#define P2_PANEL_X2 90.0f
#define P_PANEL_Y1 40.0f
#define P_PANEL_Y2 90.0f

#define PAUSE_PANEL_X1 -60.0f
#define PAUSE_PANEL_X2 60.0f
#define PAUSE_PANEL_Y1 -15.0f
#define PAUSE_PANEL_Y2 15.0f

#define TIMER_PANEL_X1 99.0f
#define TIMER_PANEL_X2 85.0f
#define TIMER_PANEL_Y1 -80.0f
#define TIMER_PANEL_Y2 -99.0f

#define TURN_PANEL_X1 -30.0f
#define TURN_PANEL_X2 30.0f
#define TURN_PANEL_Y1 70.0f
#define TURN_PANEL_Y2 110.0f

#define CAM_ROT_SPEED 2

#define DEFAULT_FONT GLUT_STROKE_ROMAN
#define SMALL_FONT GLUT_BITMAP_HELVETICA_12

#include <string>
#include "gl/glut.h"
#include "myCamera.h"
#include "primitive.h"
#include "myAppearance.h"
#include "Animation.h"
#include "SceneNode.h"
#include "ChokoCommunication.h"
#include <sstream>
#include <fstream>
#include <iomanip>

using std::string;

struct gamePiece {
	char color;
	LinearAnimation* curAnimation;
	float x, y, z;
	vector<int> displayListID;
	bool inBoard, needsUpdate, isVisible;

	gamePiece(char color, float x, float y, float z, int numScenes);
	void draw(sceneNode* model);
	void generateDisplayLists(vector<sceneNode*> model);
	void drawsubtree(sceneNode* model);
	void generateAnimation(float destX, float destY, float destZ);
};

struct player {
	char piece;
	int numPiecesInHand, numPieces;
	bool hasInitiative;
	int type;
	vector<gamePiece*> pieces;

	enum PlayerType{HUMAN, AI_EASY, AI_MEDIUM, AI_HARD};

	void reset(bool initiative);
};

class ChokoState {
	unsigned long timeOutCounter, curTime;
	bool resetTimer;
	gamePiece* board[BOARD_SIZE][BOARD_SIZE];
	player player1, player2;
	player* curPlayer;
	int state, nextState;
	int curReplayMove;
	bool playingAnimation, paused, playingReplay, tmpRemoved;
	player* winner;
	string input;
	enum States{WAITING_FOR_HUMAN_MOVE, WAITING_FOR_HUMAN_REMOVE, ROTATING_CAMERA, AI_MOVE, AI_REMOVE, REPLAY_MOVE, REPLAY_REMOVE};

	//HUD
	rectanglePrimitive* player1Panel;
	rectanglePrimitive* player2Panel;
	rectanglePrimitive* turnPanel;
	rectanglePrimitive* pausePanel; 
	rectanglePrimitive* timerPanel;

	myAppearance* panelAppearance;

	myCamera* cam;

	//Board
	boardPrimitive* boardPrim;
	bool isInDisplayList;

	int prevChosenX, prevChosenY;
	int chosenX, chosenY;
	vector <ChokoMove> moveStack;

	void printMoveStack();
	void displayString(string myString, void* font);
	void makeMove(ChokoMove move);
	void makeDropMove(ChokoMove move);
	void makeStandardMove(ChokoMove move);
	void makeCaptureMove(ChokoMove move);
	player* getOpponent();
	void passTurn();
	void updatePieceAnimations(player pl, unsigned long millis);
	bool validateAndMove();
	int convertToCoord(string coord);
public:
	int useVoice;
	int timeOutLimit;
	int useTimeOut;
	static const string savesDirectory; //dont forget to initialize in the cpp
	static const string savesFileExtention; //same as above
	string chokoFilename; //initialize in constructor as "save00."+savesFileExtention
	void saveMovesToFile() const; //saves the current moveStack to a file
	vector<ChokoMove> loadMovesFromFile() const; //loads a stack of moves from a file and returns it. If loading fails, returns an empty vector
	ChokoState(int numScenes, myCamera* cam);
	~ChokoState();
	void recogniveVoiceCommands();
	int getState() const;
	player* getPlayer(int num);
	void update(unsigned long millis);
	void displayHUD();
	void draw(vector<sceneNode*> blackPieceModel, vector<sceneNode*> whitePieceModel, myAppearance* boardAppearance);
	void firstDraw(vector<sceneNode*> blackPieceModel, vector<sceneNode*> whitePieceModel, vector<myAppearance*> boardAppearance);
	void handleInput(int x, int y);
	void resetSelection();
	void restartGame();
	void startReplay();
	void skipReplay();
	void undoMove();
	void togglePause();
	void loadGame();
	void toggleVoiceCommands();
};

#endif