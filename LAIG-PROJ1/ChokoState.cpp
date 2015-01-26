#include "ChokoState.h"
#include "YAFScene.h"
#include "voce.h"

const string ChokoState::savesDirectory = "choko_saves";
const string ChokoState::savesFileExtention = "chokosav";

float hud_emissive[4] = {0.0,0.0,0.0,0.0};
float hud_diffuse[4] = {0.0,0.0,0.0,0.0};
float hud_specular[4] = {0.0,0.0,0.0,0.0};

ChokoState::ChokoState(int numScenes, myCamera* cam)
{
#ifdef linux
	mkdir(savesDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else 
#if _MSC_VER >= 1700
	wchar_t wtext[20];
	mbstowcs(wtext, savesDirectory.c_str(), savesDirectory.size()+1);//Plus null
	LPWSTR dirname_lpstr = wtext;
	CreateDirectory(dirname_lpstr,NULL);
#else
	LPCSTR dirname_lpcstr = savesDirectory.c_str();
	CreateDirectory(dirname_lpcstr,NULL);
#endif
#endif
	voce::init("lib", true, true, "./grammar", "moves");
	voce::setRecognizerEnabled(false);
	useVoice = 0;
	this->chokoFilename = "save00."+savesFileExtention;
	playingAnimation = false;
	playingReplay = true;
	paused = false;
	this->cam = cam;
	curReplayMove = 0;
	tmpRemoved = false;
	resetTimer = true;
	useTimeOut = 0;
	timeOutLimit = 60;
	timeOutCounter = 0;
	curTime = 0;
	input = "";

	for(unsigned int i = 0; i < BOARD_SIZE; i++)
	{
		for(unsigned int j = 0; j < BOARD_SIZE; j++)
		{
			board[i][j] = NULL;
		}
	}

	this->player1.piece = 'w';
	this->player1.numPiecesInHand = NUM_PIECES;
	this->player1.numPieces = NUM_PIECES;
	this->player1.hasInitiative = true;
	this->player1.type = player::HUMAN;

	this->player2.piece = 'b';
	this->player2.numPiecesInHand = NUM_PIECES;
	this->player2.numPieces = NUM_PIECES;
	this->player2.hasInitiative = false;
	this->player2.type = player::AI_HARD;

	for(int i = 0; i < 12; i++)
	{
		float curX = -16 - 2*(i % 2);
		float curZ = -18 + (i*3); 
		player2.pieces.push_back(new gamePiece('b',curX,0,curZ, numScenes));
	}

	for(int i = 0; i < 12; i++)
	{
		float curX = 16 + 2*(i % 2);
		float curZ = -18 + (i*3); 
		player1.pieces.push_back(new gamePiece('w',curX,0,curZ, numScenes));
	}

	curPlayer = &player1;

	turnPanel = new rectanglePrimitive(TURN_PANEL_X1,TURN_PANEL_Y1,TURN_PANEL_X2,TURN_PANEL_Y2,TURN_PANEL_X2-TURN_PANEL_X1,TURN_PANEL_Y2-TURN_PANEL_Y1);
	player1Panel = new rectanglePrimitive(P1_PANEL_X1, P_PANEL_Y1, P1_PANEL_X2, P_PANEL_Y2, P1_PANEL_X2-P1_PANEL_X1, P_PANEL_Y2-P_PANEL_Y1);
	player2Panel = new rectanglePrimitive(P2_PANEL_X1, P_PANEL_Y1, P2_PANEL_X2, P_PANEL_Y2, P2_PANEL_X2-P2_PANEL_X1, P_PANEL_Y2-P_PANEL_Y1);
	pausePanel = new rectanglePrimitive(PAUSE_PANEL_X1, PAUSE_PANEL_Y1,PAUSE_PANEL_X2, PAUSE_PANEL_Y2, PAUSE_PANEL_X2-PAUSE_PANEL_X1, PAUSE_PANEL_Y2-PAUSE_PANEL_Y1);
	timerPanel = new rectanglePrimitive(TIMER_PANEL_X1, TIMER_PANEL_Y1, TIMER_PANEL_X2, TIMER_PANEL_Y2, TIMER_PANEL_X2-TIMER_PANEL_X1, TIMER_PANEL_Y2-TIMER_PANEL_Y1);

	panelAppearance = new myAppearance(hud_emissive,hud_diffuse,hud_diffuse,hud_specular,0,new CGFtexture("resources/hud_box.png"),1.0,1.0);

	this->boardPrim = new boardPrimitive(GL_CCW);

	resetSelection();

	isInDisplayList = false;
	winner = NULL;

	if(player1.type == player::HUMAN)
		state = WAITING_FOR_HUMAN_MOVE;
	else
		state = AI_MOVE;
}

void ChokoState::saveMovesToFile() const{
	ofstream saveFileStream(savesDirectory+"/"+chokoFilename, ofstream::trunc);
	if(!saveFileStream.is_open()){
		cout << "Could not save to file " << chokoFilename << "!" << endl;
		return;
	}
	for(unsigned int i=0; i < moveStack.size(); i++){
		saveFileStream << moveStack[i].moveType << " " << moveStack[i].pieceType << " ";
		saveFileStream << moveStack[i].hadInitiative << " " << (moveStack[i].removed) ? 1 : 0;
		saveFileStream << " " << moveStack[i].x1 << " " << moveStack[i].y1 << " ";
		saveFileStream << moveStack[i].x2 << " " << moveStack[i].y2 << " ";
		saveFileStream << moveStack[i].x3 << " " << moveStack[i].y3 << endl;
	}

	saveFileStream.close();
}

vector<ChokoMove> ChokoState::loadMovesFromFile() const{
	ifstream loadFileStream(savesDirectory+"/"+chokoFilename);
	vector<ChokoMove> loadedMoves;
	if(!loadFileStream.is_open()){
		cout << "Could not load from file " << chokoFilename << "!" << endl;
		return loadedMoves;
	}

	string line = "something";
	while(getline(loadFileStream, line)){
		if(line.size() == 0)
			break;
		ChokoMove currentMove;
		stringstream lineStream(line);
		bool parseError = false;
		int removedValue;
		if(!(lineStream >> currentMove.moveType)) parseError = true;
		if(!(lineStream >> currentMove.pieceType)) parseError = true;
		if(!(lineStream >> currentMove.hadInitiative)) parseError = true;
		if(!(lineStream >> removedValue)) parseError = true;
		if(removedValue != 0)
			currentMove.removed = true;
		else
			currentMove.removed = false;
		if(!(lineStream >> currentMove.x1)) parseError = true;
		if(!(lineStream >> currentMove.y1)) parseError = true;
		if(!(lineStream >> currentMove.x2)) parseError = true;
		if(!(lineStream >> currentMove.y2)) parseError = true;
		if(!(lineStream >> currentMove.x3)) parseError = true;
		if(!(lineStream >> currentMove.y3)) parseError = true;

		if(parseError){
			cout << "File " << chokoFilename << " is corrupt!\nSave could not be loaded" << endl;
			loadedMoves.clear();
			break;
		}
		else{
			loadedMoves.push_back(currentMove);
		}
	}
	loadFileStream.close();
	return loadedMoves;
}

void ChokoState::displayHUD()
{
	string hudTurn = (curPlayer == &player1) ? "Player 1's turn": "Player 2's turn";
	string winnerStr = (winner == &player1) ? "Player 1 wins!": "Player 2 wins";
	string P1 = "Player 1";
	string P2 = "Player 2";
	string hudPiecesInHand = "Pieces Remaining: ";
	string pausedGameStr = "Game Paused";
	stringstream pieceStream;
	pieceStream << player1.numPieces;
	string hudP1Pieces = hudPiecesInHand +  pieceStream.str();
	pieceStream.clear();
	pieceStream.str(string());
	pieceStream << player2.numPieces;
	string hudP2Pieces = hudPiecesInHand + pieceStream.str();
	float time = curTime/1000.0;
	stringstream ss;

	if(winner == NULL)
	{
		if(time < 1000)
			ss << setw(5) << fixed << setprecision(1) << time;
		else
		{
			ss << "A lot...";
		}
	}
	else
	{
		ss << "Winner!";
	}



	// Temporary disable lighting
	glDisable( GL_LIGHTING );

	// Our HUD consists of a simple rectangle
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( -100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glColor3f( 1.0f, 1.0f, 1.0f );
	panelAppearance->apply();
	player1Panel->draw();
	player2Panel->draw();
	timerPanel->draw();
	if(paused || winner != NULL){
		pausePanel->draw();
	}
	turnPanel->draw();
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();

	if(paused || winner != NULL){
		glPushMatrix();
		glColor3f(1.0,0,0);
		glTranslatef(-24.0f, -5.0f,0.0f);
		glScalef( 0.05f, 0.1f, 0.1f );
		if(paused)
			displayString(pausedGameStr, DEFAULT_FONT);
		else{

			displayString(winnerStr, DEFAULT_FONT);
		}
		glPopMatrix();
	}

	glPushMatrix();
	glColor3f(1,0,0);
	glTranslatef(86.0f, -95.0f, 0.0f);
	glScalef( 0.03f, 0.1f, 0.1f );
	displayString(ss.str(), DEFAULT_FONT);
	glPopMatrix();


	glPushMatrix();
	glColor3f(1.0,0,0);
	glTranslatef(-24.0f, 85.0f,0.0f);
	glScalef( 0.05f, 0.1f, 0.1f );
	displayString(hudTurn, DEFAULT_FONT);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1,1,1);
	glTranslatef(-85.0f, 50.0f,0.0f);
	glScalef( 0.03f, 0.1f, 0.1f );
	displayString(hudP1Pieces, DEFAULT_FONT);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1,1,1);
	glTranslatef(-73.0f, 70.0f,0.0f);
	glScalef( 0.03f, 0.1f, 0.1f );
	displayString(P1, DEFAULT_FONT);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,0,0);
	glTranslatef(45.0f, 50.0f,0.0f);
	glScalef( 0.03f, 0.1f, 0.1f );
	displayString(hudP2Pieces, DEFAULT_FONT);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0,0,0);
	glTranslatef(57.0f, 70.0f,0.0f);
	glScalef( 0.03f, 0.1f, 0.1f );
	displayString(P2, DEFAULT_FONT);
	glPopMatrix();

	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	// Reenable lighting
	glEnable( GL_LIGHTING );
}

void ChokoState::displayString( string myString, void* font)
{
	for(unsigned int i = 0; i < myString.size(); i++)
	{
		glutStrokeCharacter(font,myString[i]);
	}
}

ChokoState::~ChokoState()
{ 
	voce::destroy();
	delete player1Panel;
	delete player2Panel;
	delete turnPanel;
	delete panelAppearance;
	delete boardPrim;
	delete pausePanel;
	delete timerPanel;
}

void ChokoState::draw(vector<sceneNode*> blackPiece, vector<sceneNode*> whitePiece, myAppearance* boardAppearance)
{
	glPushMatrix();
	for(unsigned int i = 0; i < player1.pieces.size(); i++)
	{
		if(!player1.pieces[i]->needsUpdate)
			player1.pieces[i]->draw(whitePiece[YAFScene::selectedScene]);
		else
		{
			player1.pieces[i]->generateDisplayLists(whitePiece);
			player1.pieces[i]->needsUpdate = false;
			player1.pieces[i]->draw(whitePiece[YAFScene::selectedScene]);
		}
	}

	for(unsigned int i = 0; i < player2.pieces.size(); i++)
	{
		if(!player2.pieces[i]->needsUpdate)
			player2.pieces[i]->draw(blackPiece[YAFScene::selectedScene]);
		else
		{
			player2.pieces[i]->generateDisplayLists(blackPiece);
			player2.pieces[i]->needsUpdate = false;
			player2.pieces[i]->draw(blackPiece[YAFScene::selectedScene]);
		}
	}

	boardAppearance->apply();
	boardPrim->draw();
	glPopMatrix();
}

void ChokoState::firstDraw( vector<sceneNode*> blackPiece, vector<sceneNode*> whitePiece, vector<myAppearance*> boardAppearance )
{
	glPushMatrix();
	for(unsigned int i = 0; i < player1.pieces.size(); i++)
	{
		player1.pieces[i]->generateDisplayLists(whitePiece);
	}

	for(unsigned int i = 0; i < player2.pieces.size(); i++)
	{
		player2.pieces[i]->generateDisplayLists(blackPiece);
	}

	boardAppearance[YAFScene::selectedScene]->apply();
	boardPrim->draw();
	glPopMatrix();
}

void ChokoState::update( unsigned long millis )
{
	if(player1.numPieces <= 0)
		winner = &player2;
	else if(player2.numPieces <= 0)
		winner = &player1;

	if(winner != NULL)
	{
		//cout << "winner winner chicken dinner!" << endl;
	}
	else if(playingAnimation)
	{
		updatePieceAnimations(player1, millis);

		updatePieceAnimations(player2, millis);
	}
	else
	{
		if(paused){
			timeOutCounter = millis-curTime;
			return;
		}

		if (useTimeOut)
		{
			if(millis-timeOutCounter >= timeOutLimit*1000)
			{
				ChokoMove move;

				char initiative;
				if(player1.hasInitiative)
					initiative = player1.piece;
				else if (player2.hasInitiative)
					initiative = player2.piece;
				else
					initiative = 'e';

				move = ChokoCommunication::getAIMove("easy", initiative, curPlayer->piece, curPlayer->numPiecesInHand,board);
				makeMove(move);
				moveStack.push_back(move);

				if(move.moveType == ChokoMove::CAPTURE_AND_REMOVAL)
				{
					player* opponent = getOpponent();

					for(unsigned int i = 0; i < opponent->pieces.size(); i++)
					{
						if(opponent->pieces[i] == board[moveStack[moveStack.size()-1].x3][moveStack[moveStack.size()-1].y3])
						{
							opponent->pieces[i]->isVisible = false;
							break;
						}
					}

					board[moveStack[moveStack.size()-1].x3][moveStack[moveStack.size()-1].y3] = NULL;
					moveStack[moveStack.size()-1].removed = true;
					opponent->numPieces--;

					passTurn();

					if(curPlayer->type == player::HUMAN)
					{
						state = ROTATING_CAMERA;
					}
					else
					{
						state = AI_MOVE;
					}
				}

				curTime = 0;
				timeOutCounter = millis;
			}
		}


		if (resetTimer)
		{
			curTime = 0;
			timeOutCounter = millis;
			resetTimer = false;
		}
		else
		{
			curTime = millis-timeOutCounter;
		}

		if(state == AI_REMOVE)
		{
			playingReplay = false;
			player* opponent = getOpponent();

			for(unsigned int i = 0; i < opponent->pieces.size(); i++)
			{
				if(opponent->pieces[i] == board[moveStack[moveStack.size()-1].x3][moveStack[moveStack.size()-1].y3])
				{
					opponent->pieces[i]->isVisible = false;
					break;
				}
			}

			board[moveStack[moveStack.size()-1].x3][moveStack[moveStack.size()-1].y3] = NULL;
			moveStack[moveStack.size()-1].removed = true;
			opponent->numPieces--;

			passTurn();

			resetTimer = true;

			if(curPlayer->type == player::HUMAN)
			{
				state = ROTATING_CAMERA;
			}
			else
			{
				state = AI_MOVE;
			}
		}
		else if(state == AI_MOVE)
		{
			playingReplay = false;
			ChokoMove move;
			if(ChokoCommunication::canPlayerMove(curPlayer->numPiecesInHand,curPlayer->piece,board))
			{
				string difficulty;

				if(curPlayer->type == player::AI_EASY)
					difficulty = "easy";
				else if (curPlayer->type == player::AI_MEDIUM)
					difficulty = "medium";
				else
					difficulty = "hard";

				char initiative;
				if(player1.hasInitiative)
					initiative = player1.piece;
				else if (player2.hasInitiative)
					initiative = player2.piece;
				else
					initiative = 'e';

				move = ChokoCommunication::getAIMove(difficulty, initiative, curPlayer->piece, curPlayer->numPiecesInHand,board);
			}
			else
			{
				move.moveType = ChokoMove::NO_MOVES_AVAILABLE;

				if(player1.hasInitiative)
					move.hadInitiative = player1.piece;
				else if(player2.hasInitiative)
					move.hadInitiative = player2.piece;
				else
					move.hadInitiative = 'e';
			}
			resetTimer = true;
			makeMove(move);
			moveStack.push_back(move);
		}
		else if(state == WAITING_FOR_HUMAN_MOVE)
		{
			playingReplay = false;

			if(!ChokoCommunication::canPlayerMove(curPlayer->numPiecesInHand,curPlayer->piece,board))
			{
				ChokoMove move;

				if(player1.hasInitiative)
					move.hadInitiative = player1.piece;
				else if(player2.hasInitiative)
					move.hadInitiative = player2.piece;
				else
					move.hadInitiative = 'e';

				move.moveType = ChokoMove::NO_MOVES_AVAILABLE;
				makeMove(move);
				moveStack.push_back(move);
			}
			recogniveVoiceCommands();
		}
		else if (state == WAITING_FOR_HUMAN_REMOVE)
		{
			recogniveVoiceCommands();
		}
		else if(state == ROTATING_CAMERA)
		{
			playingReplay = false;
			const float* camRot = cam->getRotation();
			float targetRot;

			if(curPlayer == &player1)
				targetRot = 0;
			else
				targetRot = 180;

			do{

				if(camRot[CG_CGFcamera_AXIS_X] == 0 && camRot[CG_CGFcamera_AXIS_Y] == targetRot && camRot[CG_CGFcamera_AXIS_Z] == 0)
					state = WAITING_FOR_HUMAN_MOVE;
				else
				{
					if(camRot[CG_CGFcamera_AXIS_X] != 0 && abs(camRot[CG_CGFcamera_AXIS_X]) < CAM_ROT_SPEED)
						cam->rotate(CG_CGFcamera_AXIS_X,-camRot[CG_CGFcamera_AXIS_X]);
					else if(camRot[CG_CGFcamera_AXIS_X] < 0)
					{
						cam->rotate(CG_CGFcamera_AXIS_X,CAM_ROT_SPEED);
					}
					else if (camRot[CG_CGFcamera_AXIS_X] > 0)
					{
						cam->rotate(CG_CGFcamera_AXIS_X,-CAM_ROT_SPEED);
					}

					if(camRot[CG_CGFcamera_AXIS_Y] != targetRot && abs(camRot[CG_CGFcamera_AXIS_Y]-targetRot) < CAM_ROT_SPEED)
						cam->rotate(CG_CGFcamera_AXIS_Y,camRot[CG_CGFcamera_AXIS_Y]-targetRot);
					else if(camRot[CG_CGFcamera_AXIS_Y] < targetRot)
					{
						cam->rotate(CG_CGFcamera_AXIS_Y,CAM_ROT_SPEED);
					}
					else if (camRot[CG_CGFcamera_AXIS_Y] > targetRot)
					{
						cam->rotate(CG_CGFcamera_AXIS_Y,-CAM_ROT_SPEED);
					}

					if(camRot[CG_CGFcamera_AXIS_Z] != 0 && abs(camRot[CG_CGFcamera_AXIS_Z]) < CAM_ROT_SPEED)
						cam->rotate(CG_CGFcamera_AXIS_Z,-camRot[CG_CGFcamera_AXIS_Z]);
					else if(camRot[CG_CGFcamera_AXIS_Z] < 0)
					{
						cam->rotate(CG_CGFcamera_AXIS_Z,CAM_ROT_SPEED);
					}
					else if (camRot[CG_CGFcamera_AXIS_Z] > 0)
					{
						cam->rotate(CG_CGFcamera_AXIS_Z,-CAM_ROT_SPEED);
					}
				}
			}while(YAFScene::skipCameraRotation && state != WAITING_FOR_HUMAN_MOVE);
		}
		else if (state == REPLAY_MOVE)
		{
			if(curReplayMove >= moveStack.size())
			{
				if(curPlayer->type == player::HUMAN)
				{
					state = ROTATING_CAMERA;
				}
				else
				{
					state = AI_MOVE;
				}
			}
			else
			{
				tmpRemoved = false;
				ChokoMove move = moveStack[curReplayMove];
				resetTimer = true;
				makeMove(move);
			}
		}
		else if (state == REPLAY_REMOVE)
		{
			player* opponent = getOpponent();

			for(unsigned int i = 0; i < opponent->pieces.size(); i++)
			{
				if(opponent->pieces[i] == board[moveStack[curReplayMove].x3][moveStack[curReplayMove].y3])
				{
					opponent->pieces[i]->isVisible = false;
					break;
				}
			}

			board[moveStack[curReplayMove].x3][moveStack[curReplayMove].y3] = NULL;
			tmpRemoved = true;
			opponent->numPieces--;
			resetTimer = true;
			passTurn();

			state = REPLAY_MOVE;
			curReplayMove++;
		}
	}
}

void ChokoState::makeMove( ChokoMove move )
{
	if(move.moveType == ChokoMove::DROP)
		makeDropMove(move);
	else if(move.moveType == ChokoMove::STANDARD)
		makeStandardMove(move);
	else if(move.moveType == ChokoMove::CAPTURE_AND_REMOVAL || move.moveType == ChokoMove::CAPTURE_NO_REMOVAL)
		makeCaptureMove(move);
	else
	{
		passTurn();

		if(!playingReplay)
		{
			if(curPlayer->type == player::HUMAN)
			{
				state = ROTATING_CAMERA;
			}
			else
			{
				state = AI_MOVE;
			}
		}
		else
		{
			state = REPLAY_MOVE;
			curReplayMove++;
		}
	}

	resetTimer = true;
}

int ChokoState::getState() const
{
	return state;
}

void ChokoState::handleInput( int x, int y )
{
	if(!playingAnimation && !paused && state == WAITING_FOR_HUMAN_MOVE || state == WAITING_FOR_HUMAN_REMOVE)
	{
		if(chosenX == -1 && chosenY == -1)
		{
			chosenX = x;
			chosenY = y;
		}
		else
		{
			prevChosenX = chosenX;
			prevChosenY = chosenY;
			chosenX = x;
			chosenY = y;
		}

		player* opponent = getOpponent();

		bool madeMove = false;

		if(state == WAITING_FOR_HUMAN_MOVE)
		{
			if(prevChosenX != -1 && prevChosenY != -1 && chosenX != -1 && chosenY != -1)
			{
				madeMove = validateAndMove();
			}

			if(!madeMove)
			{
				boardPrim->resetSelection();
				boardPrim->setSelected(chosenX,chosenY);
			}
		}
		else if (state == WAITING_FOR_HUMAN_REMOVE)
		{
			if(prevChosenX != -1 && prevChosenY != -1 && chosenX != -1 && chosenY != -1)
			{
				if(prevChosenX == chosenX && prevChosenY == chosenY && board[chosenX][chosenY] != NULL && board[chosenX][chosenY]->color != curPlayer->piece)
				{
					for(unsigned int i = 0; i < opponent->pieces.size(); i++)
					{
						if(opponent->pieces[i] == board[chosenX][chosenY])
						{
							opponent->pieces[i]->isVisible = false;
							break;
						}
					}

					board[chosenX][chosenY] = NULL;
					opponent->numPieces--;
					moveStack[moveStack.size()-1].x3 = chosenX;
					moveStack[moveStack.size()-1].y3 = chosenY;
					moveStack[moveStack.size()-1].removed = true;
					madeMove = true;
					resetTimer = true;
					passTurn();

					if(curPlayer->type == player::HUMAN)
					{
						state = ROTATING_CAMERA;
					}
					else
					{
						state = AI_MOVE;
					}
				}

				resetSelection();
			}

			if(!madeMove)
			{
				boardPrim->resetSelection();
				boardPrim->setSelected(chosenX,chosenY);
			}
		}
	}
}

void ChokoState::resetSelection()
{
	chosenX = -1;
	chosenY = -1;
	prevChosenX = -1;
	prevChosenY = -1;

	boardPrim->resetSelection();
}

player* ChokoState::getOpponent()
{
	player* opponent = NULL;
	if(curPlayer == &player1)
		opponent = &player2;
	else
		opponent = &player1;

	return opponent;
}

void ChokoState::passTurn()
{
	if(curPlayer == &player1)
		curPlayer = &player2;
	else
		curPlayer = &player1;
}

void ChokoState::makeDropMove( ChokoMove move )
{
	if(move.moveType == ChokoMove::DROP)
	{
		for(unsigned int i = 0; i < curPlayer->pieces.size(); i++)
		{
			if(!curPlayer->pieces[i]->inBoard)
			{
				curPlayer->pieces[i]->inBoard = true;
				board[move.x1][move.y1] = curPlayer->pieces[i];
				curPlayer->pieces[i]->generateAnimation( (move.x1*5)-10, 0, -(move.y1*5)+10);
				playingAnimation = true;
				curPlayer->numPiecesInHand--;

				player* opponent = getOpponent();

				if(!opponent->hasInitiative)
					curPlayer->hasInitiative = true;

				if(player1.numPiecesInHand == 0 && player2.numPiecesInHand == 0)
				{
					curPlayer == &player2;
					player1.hasInitiative = false;
					player2.hasInitiative = false;
				}
				else passTurn();

				if(!playingReplay)
				{
					if(curPlayer->type == player::HUMAN)
					{
						nextState = ROTATING_CAMERA;
					}
					else
					{
						nextState = AI_MOVE;
					}
				}
				else
				{
					nextState = REPLAY_MOVE;
					curReplayMove++;
				}

				break;
			}
		}
	}
}

void ChokoState::makeStandardMove( ChokoMove move )
{
	if(move.moveType == ChokoMove::STANDARD)
	{
		curPlayer->hasInitiative = false;
		board[move.x1][move.y1]->generateAnimation( (move.x2*5)-10, 0, -(move.y2*5)+10);
		playingAnimation = true;
		board[move.x2][move.y2] = board[move.x1][move.y1];
		board[move.x1][move.y1] = NULL;

		passTurn();

		if(!playingReplay)
		{
			if(curPlayer->type == player::HUMAN)
			{
				nextState = ROTATING_CAMERA;
			}
			else
			{
				nextState = AI_MOVE;
			}
		}
		else
		{
			nextState = REPLAY_MOVE;
			curReplayMove++;
		}
	}
}

void ChokoState::makeCaptureMove( ChokoMove move )
{
	if(move.moveType == ChokoMove::CAPTURE_AND_REMOVAL || move.moveType == ChokoMove::CAPTURE_NO_REMOVAL)
	{
		curPlayer->hasInitiative = false;
		board[move.x1][move.y1]->generateAnimation( (move.x2*5)-10, 0, -(move.y2*5)+10);
		playingAnimation = true;

		int offsetX = (move.x2 - move.x1) / 2;
		int offsetY = (move.y2 - move.y1) / 2;

		player* opponent = getOpponent();

		for(unsigned int i = 0; i < opponent->pieces.size(); i++)
		{
			if(opponent->pieces[i] == board[move.x1+offsetX][move.y1+offsetY])
			{
				opponent->pieces[i]->isVisible = false;
				break;
			}
		}
		board[move.x2][move.y2] = board[move.x1][move.y1];
		board[move.x1][move.y1] = NULL;
		board[move.x1+offsetX][move.y1+offsetY] = NULL;

		opponent->numPieces--;
	}

	if(move.moveType == ChokoMove::CAPTURE_AND_REMOVAL)
	{
		if(!playingReplay)
		{
			if(curPlayer->type == player::HUMAN)
			{
				nextState = WAITING_FOR_HUMAN_REMOVE;
			}
			else
			{
				nextState = AI_REMOVE;
			}
		}
		else
		{
			nextState = REPLAY_REMOVE;
		}
	}
	else if(move.moveType == ChokoMove::CAPTURE_NO_REMOVAL)
	{
		passTurn();

		if(!playingReplay)
		{
			if(curPlayer->type == player::HUMAN)
			{
				nextState = ROTATING_CAMERA;
			}
			else
			{
				nextState = AI_MOVE;
			}
		}
		else
		{
			nextState = REPLAY_MOVE;
			curReplayMove++;
		}
	}
}

void ChokoState::updatePieceAnimations( player pl , unsigned long millis)
{
	for(unsigned int i = 0; i < pl.pieces.size(); i++)
	{
		if(pl.pieces[i]->curAnimation != NULL)
		{
			pl.pieces[i]->curAnimation->update(millis, paused);

			if(pl.pieces[i]->curAnimation->finished)
			{
				state = nextState;
				playingAnimation = false;
				pl.pieces[i]->x = pl.pieces[i]->x + pl.pieces[i]->curAnimation->getPos().x;
				pl.pieces[i]->y = pl.pieces[i]->y + pl.pieces[i]->curAnimation->getPos().y;
				pl.pieces[i]->z = pl.pieces[i]->z + pl.pieces[i]->curAnimation->getPos().z;
				delete pl.pieces[i]->curAnimation;
				pl.pieces[i]->curAnimation = NULL;
				pl.pieces[i]->needsUpdate = true;
				resetSelection();
				glDeleteLists(pl.pieces[i]->displayListID[YAFScene::selectedScene],1);
			}
		}
	}
}

player* ChokoState::getPlayer( int num )
{
	if(num == 1)
		return &player1;
	else
		return &player2;
}

void ChokoState::restartGame()
{
	player1.reset(true);
	player2.reset(false);
	winner = NULL;
	playingAnimation = false;
	isInDisplayList = false;
	playingReplay = false;
	tmpRemoved = false;
	resetTimer = true;
	useTimeOut = 0;
	timeOutLimit = 60;
	timeOutCounter = 0;
	curTime = 0;
	input = "";

	resetSelection();

	for(unsigned int i = 0; i < BOARD_SIZE; i++)
	{
		for(unsigned int j = 0; j < BOARD_SIZE; j++)
		{
			board[i][j] = NULL;
		}
	}

	curPlayer = &player1;

	if(player1.type == player::HUMAN)
		state = ROTATING_CAMERA;
	else
		state = AI_MOVE;

	moveStack.clear();

}

void ChokoState::undoMove()
{
	if(winner != NULL)
		winner = NULL;
	if(!playingReplay)
	{
		while(moveStack.size() > 0 && moveStack[moveStack.size()-1].moveType == ChokoMove::NO_MOVES_AVAILABLE)
		{
			moveStack.pop_back();
		}
	}
	else
	{
		while(curReplayMove > 0 && moveStack[curReplayMove-1].moveType == ChokoMove::NO_MOVES_AVAILABLE)
		{
			curReplayMove--;
		}
	}

	if(moveStack.size() >= 1)
	{
		paused = true;
		ChokoMove move;
		if(!playingReplay)
			move = moveStack[moveStack.size()-1];
		else
		{
			if(curReplayMove > 0)
				move = moveStack[curReplayMove-1];
			else
			{
				move = moveStack[0];
			}
		}

		player* movPlayer;
		player* opponent;

		playingAnimation = false;

		if(move.pieceType == 'b')
		{
			movPlayer = &player2;
			opponent = &player1;
		}
		else
		{
			movPlayer = &player1;
			opponent = &player2;
		}

		if(move.hadInitiative == movPlayer->piece)
		{
			movPlayer->hasInitiative = true;
			opponent->hasInitiative = false;
		}
		else if (move.hadInitiative == opponent->piece)
		{
			movPlayer->hasInitiative = false;
			opponent->hasInitiative = true;
		}
		else
		{
			movPlayer->hasInitiative = false;
			opponent->hasInitiative = false;
		}

		curPlayer = movPlayer;

		if(!playingReplay)
			moveStack.pop_back();

		if(move.moveType == ChokoMove::DROP)
		{
			board[move.x1][move.y1] = NULL;

			for(int i = movPlayer->pieces.size()-1; i >= 0 ; i--)
			{
				if(movPlayer->pieces[i]->inBoard)
				{
					delete movPlayer->pieces[i]->curAnimation;
					movPlayer->pieces[i]->curAnimation = NULL;
					movPlayer->pieces[i]->inBoard = false;
					movPlayer->numPiecesInHand++;

					if(movPlayer->piece == 'b')
					{
						movPlayer->pieces[i]->x = -16 - 2*(i % 2);
						movPlayer->pieces[i]->y = 0;
						movPlayer->pieces[i]->z = -18 + (i*3);
					}
					else
					{
						movPlayer->pieces[i]->x = 16 + 2*(i % 2);
						movPlayer->pieces[i]->y = 0;
						movPlayer->pieces[i]->z = -18 + (i*3); 
					}

					movPlayer->pieces[i]->needsUpdate = true;
					break;
				}
			}
		}
		else if (move.moveType == ChokoMove::STANDARD)
		{
			board[move.x1][move.y1] = board[move.x2][move.y2];
			board[move.x2][move.y2] = NULL;

			delete board[move.x1][move.y1]->curAnimation;
			board[move.x1][move.y1]->curAnimation = NULL;

			board[move.x1][move.y1]->x = (move.x1*5)-10;
			board[move.x1][move.y1]->y = 0;
			board[move.x1][move.y1]->z = -(move.y1*5)+10;

			board[move.x1][move.y1]->needsUpdate = true;
		}
		else if (move.moveType == ChokoMove::CAPTURE_AND_REMOVAL)
		{
			board[move.x1][move.y1] = board[move.x2][move.y2];
			board[move.x2][move.y2] = NULL;

			delete board[move.x1][move.y1]->curAnimation;
			board[move.x1][move.y1]->curAnimation = NULL;

			board[move.x1][move.y1]->x = (move.x1*5)-10;
			board[move.x1][move.y1]->y = 0;
			board[move.x1][move.y1]->z = -(move.y1*5)+10;

			board[move.x1][move.y1]->needsUpdate = true;

			int offsetX = (move.x2 - move.x1) / 2;
			int offsetY = (move.y2 - move.y1) / 2;

			for(unsigned int i = 0; i < opponent->pieces.size(); i++)
			{
				if(!opponent->pieces[i]->isVisible)
				{
					board[move.x1+offsetX][move.y1+offsetY] = opponent->pieces[i];
					opponent->pieces[i]->x = ((move.x1+offsetX)*5)-10;
					opponent->pieces[i]->y = 0;
					opponent->pieces[i]->z = -((move.y1+offsetY)*5)+10;
					opponent->numPieces++;

					opponent->pieces[i]->isVisible = true;
					opponent->pieces[i]->needsUpdate = true;
					break;
				}
			}

			if((move.removed && !playingReplay) || (playingReplay && tmpRemoved))
			{
				for(unsigned int i = 0; i < opponent->pieces.size(); i++)
				{
					if(!opponent->pieces[i]->isVisible)
					{
						board[move.x3][move.y3] = opponent->pieces[i];
						opponent->pieces[i]->x = ((move.x3)*5)-10;
						opponent->pieces[i]->y = 0;
						opponent->pieces[i]->z = -((move.y3)*5)+10;
						opponent->numPieces++;

						opponent->pieces[i]->isVisible = true;
						opponent->pieces[i]->needsUpdate = true;
						break;
					}
				}
			}
		}
		else if (move.moveType == ChokoMove::CAPTURE_NO_REMOVAL)
		{
			board[move.x1][move.y1] = board[move.x2][move.y2];
			board[move.x2][move.y2] = NULL;

			delete board[move.x1][move.y1]->curAnimation;
			board[move.x1][move.y1]->curAnimation = NULL;

			board[move.x1][move.y1]->x = (move.x1*5)-10;
			board[move.x1][move.y1]->y = 0;
			board[move.x1][move.y1]->z = -(move.y1*5)+10;

			board[move.x1][move.y1]->needsUpdate = true;

			int offsetX = (move.x2 - move.x1) / 2;
			int offsetY = (move.y2 - move.y1) / 2;

			for(unsigned int i = 0; i < opponent->pieces.size(); i++)
			{
				if(!opponent->pieces[i]->isVisible)
				{
					board[move.x1+offsetX][move.y1+offsetY] = opponent->pieces[i];
					opponent->pieces[i]->x = ((move.x1+offsetX)*5)-10;
					opponent->pieces[i]->y = 0;
					opponent->pieces[i]->z = -((move.y1+offsetY)*5)+10;
					opponent->numPieces++;

					opponent->pieces[i]->isVisible = true;
					opponent->pieces[i]->needsUpdate = true;
					break;
				}
			}
		}

		if(!playingReplay)
		{
			if(curPlayer->type == player::HUMAN)
			{
				state = ROTATING_CAMERA;
			}
			else
			{
				state = AI_MOVE;
			}
		}
		else
		{
			if(curReplayMove > 0)
				curReplayMove--;
			state = REPLAY_MOVE;
			tmpRemoved = true;
		}
	}
}

void ChokoState::printMoveStack()
{
	for(unsigned int i = 0; i < moveStack.size(); i++)
	{
		cout << "Move by '" << moveStack[i].pieceType << "'. The move is of type " << moveStack[i].moveType << endl;
	}

	cout << endl << endl;
}

void ChokoState::togglePause()
{
	paused = !paused;
}

void ChokoState::startReplay()
{
	if(moveStack.size() > 0){
		player1.reset(true);
		player2.reset(false);
		winner = NULL;
		playingAnimation = false;
		isInDisplayList = false;
		playingReplay = true;
		tmpRemoved = false;
		resetTimer = true;
		useTimeOut = 0;
		timeOutLimit = 60;
		timeOutCounter = 0;
		curReplayMove = 0;
		curTime = 0;
		resetSelection();

		for(unsigned int i = 0; i < BOARD_SIZE; i++)
		{
			for(unsigned int j = 0; j < BOARD_SIZE; j++)
			{
				board[i][j] = NULL;
			}
		}

		curPlayer = &player1;

		state = REPLAY_MOVE;
	}
}

void ChokoState::skipReplay()
{
	if(playingReplay){
		player1.reset(true);
		player2.reset(false);
		winner = NULL;
		playingAnimation = false;
		isInDisplayList = false;
		playingReplay = true;
		curReplayMove = 0;
		tmpRemoved = false;
		resetTimer = false;
		useTimeOut = 0;
		timeOutLimit = 60;
		timeOutCounter = 0;
		resetSelection();

		for(unsigned int i = 0; i < BOARD_SIZE; i++)
		{
			for(unsigned int j = 0; j < BOARD_SIZE; j++)
			{
				board[i][j] = NULL;
			}
		}

		curPlayer = &player1;

		for(; curReplayMove < moveStack.size();)
		{
			ChokoMove move = moveStack[curReplayMove];

			makeMove(move);

			if(move.moveType == ChokoMove::CAPTURE_AND_REMOVAL && move.removed)
			{
				player* opponent = getOpponent();

				for(unsigned int i = 0; i < opponent->pieces.size(); i++)
				{
					if(opponent->pieces[i] == board[moveStack[curReplayMove].x3][moveStack[curReplayMove].y3])
					{
						opponent->pieces[i]->isVisible = false;
						break;
					}
				}

				board[moveStack[curReplayMove].x3][moveStack[curReplayMove].y3] = NULL;
				opponent->numPieces--;

				passTurn();
				curReplayMove++;

				if(curPlayer->type == player::HUMAN)
				{
					nextState = ROTATING_CAMERA;
				}
				else
				{
					nextState = AI_MOVE;
				}
			}
		}

		for(int i = 0; i < BOARD_SIZE; i++)
		{
			for(int j = 0; j < BOARD_SIZE; j++)
			{
				if(board[i][j] != NULL)
				{
					board[i][j]->x = (i*5)-10;
					board[i][j]->y = 0;
					board[i][j]->z = -(j*5)+10;
					board[i][j]->needsUpdate = true;
				}
			}
		}

		for(unsigned int i = 0; i < player1.pieces.size(); i++)
		{
			delete player1.pieces[i]->curAnimation;
			player1.pieces[i]->curAnimation = NULL;
		}

		for(unsigned int i = 0; i < player2.pieces.size(); i++)
		{
			delete player2.pieces[i]->curAnimation;
			player2.pieces[i]->curAnimation = NULL;
		}
		playingAnimation = false;
		playingReplay = false;
		state = nextState;
	}
}

void ChokoState::loadGame()
{
	moveStack = loadMovesFromFile();
	if(!moveStack.empty()){
		playingReplay = true;
		skipReplay();
	}
}

bool ChokoState::validateAndMove()
{
	player* opponent = getOpponent();
	bool madeMove = false;
	ChokoMove move;
	move.x1 = prevChosenX;
	move.y1 = prevChosenY;
	move.x2 = chosenX;
	move.y2 = chosenY;
	move.pieceType = curPlayer->piece;
	move.removed = false;

	if(player1.hasInitiative)
		move.hadInitiative = player1.piece;
	else if(player2.hasInitiative)
		move.hadInitiative = player2.piece;
	else
		move.hadInitiative = 'e';

	if(prevChosenX == chosenX && prevChosenY == chosenY && curPlayer->numPiecesInHand > 0 && board[chosenX][chosenY] == NULL)
	{
		move.moveType = ChokoMove::DROP;
		madeMove = true;
		makeMove(move);
		moveStack.push_back(move);
	}
	else if(board[prevChosenX][prevChosenY] != NULL && board[prevChosenX][prevChosenY]->color == curPlayer->piece && !opponent->hasInitiative)
	{
		if( (abs(chosenX-prevChosenX) == 2 && chosenY == prevChosenY) || (abs(chosenY-prevChosenY) == 2 && chosenX == prevChosenX) )
		{
			int offsetX = (chosenX - prevChosenX) / 2;
			int offsetY = (chosenY - prevChosenY) / 2;

			if(board[chosenX][chosenY] == NULL && board[prevChosenX+offsetX][prevChosenY+offsetY] != NULL && board[prevChosenX+offsetX][prevChosenY+offsetY]->color != curPlayer->piece)
			{
				if(curPlayer == &player1)
				{
					if(player2.numPieces != player2.numPiecesInHand+1)
						move.moveType = ChokoMove::CAPTURE_AND_REMOVAL;
					else
						move.moveType = ChokoMove::CAPTURE_NO_REMOVAL;
				}
				else
				{
					if(player1.numPieces != player1.numPiecesInHand+1)
						move.moveType = ChokoMove::CAPTURE_AND_REMOVAL;
					else
						move.moveType = ChokoMove::CAPTURE_NO_REMOVAL;
				}
				makeMove(move);
				madeMove = true;
				moveStack.push_back(move);
				boardPrim->setSelected(chosenX,chosenY);
			}

		}
		else if( (abs(chosenX-prevChosenX) == 1 && chosenY == prevChosenY) || (abs(chosenY-prevChosenY) == 1 && chosenX == prevChosenX) )
		{
			if(board[chosenX][chosenY] == NULL)
			{
				move.moveType = ChokoMove::STANDARD;

				makeMove(move);
				madeMove = true;
				moveStack.push_back(move);
				boardPrim->setSelected(chosenX,chosenY);
			}
		}
	}

	return madeMove;
}

int ChokoState::convertToCoord( string coord )
{
	if(curPlayer == &player1)
	{
		if(coord == "a" || coord == "alpha")
		{
			return 0;
		} else if (coord == "b" || coord == "bravo")
		{
			return 1;
		} else if (coord == "c" || coord == "charlie")
		{
			return 2;
		} else if (coord == "d" || coord == "delta")
		{
			return 3;
		} else if (coord == "e" || coord == "echo")
		{
			return 4;
		}
		else if(coord == "one")
		{
			return 0;
		} else if (coord == "two")
		{
			return 1;
		} else if (coord == "three")
		{
			return 2;
		} else if (coord == "four")
		{
			return 3;
		} else if (coord == "five")
		{
			return 4;
		}
	} else
	{
		if(coord == "e"  || coord == "echo")
		{
			return 0;
		} else if (coord == "d" || coord == "delta")
		{
			return 1;
		} else if (coord == "c" || coord == "charlie")
		{
			return 2;
		} else if (coord == "b" || coord == "bravo")
		{
			return 3;
		} else if (coord == "a" || coord == "alpha")
		{
			return 4;
		}
		else if(coord == "five")
		{
			return 0;
		} else if (coord == "four")
		{
			return 1;
		} else if (coord == "three")
		{
			return 2;
		} else if (coord == "two")
		{
			return 3;
		} else if (coord == "one")
		{
			return 4;
		}
	}
	
}

void ChokoState::recogniveVoiceCommands()
{
	if(state == WAITING_FOR_HUMAN_MOVE)
	{
		while (voce::getRecognizerQueueSize() > 0)
		{
			string s = voce::popRecognizedString();
			input = s;

			std::cout << "You said: " << s << std::endl;

			if(s.find("move piece from") != string::npos)
			{
				string wordBuf;
				stringstream ss(s);
				vector<string> words;

				while (ss >> wordBuf)
					words.push_back(wordBuf);

				prevChosenX = convertToCoord(words[3]);
				prevChosenY = convertToCoord(words[4]);
				chosenX = convertToCoord(words[6]);
				chosenY = convertToCoord(words[7]);

				validateAndMove();
				resetSelection();
			}
			else if (s.find("drop") != string::npos)
			{
				string wordBuf;
				stringstream ss(s);
				vector<string> words;

				while (ss >> wordBuf)
					words.push_back(wordBuf);

				prevChosenX = convertToCoord(words[3]);

				prevChosenY = convertToCoord(words[4]);

				chosenX = prevChosenX;
				chosenY = prevChosenY;

				validateAndMove();
				resetSelection();
			}
		}
	}
	else if (state == WAITING_FOR_HUMAN_REMOVE)
	{
		while (voce::getRecognizerQueueSize() > 0)
		{
			string s = voce::popRecognizedString();
			input = s;

			std::cout << "You said: " << s << std::endl;

			if(s.find("remove piece on") != string::npos)
			{
				string wordBuf;
				stringstream ss(s);
				vector<string> words;

				while (ss >> wordBuf)
					words.push_back(wordBuf);

				prevChosenX = convertToCoord(words[3]);

				prevChosenY = convertToCoord(words[4]);

				chosenX = prevChosenX;
				chosenY = prevChosenY;

				if(prevChosenX == chosenX && prevChosenY == chosenY && board[chosenX][chosenY] != NULL && board[chosenX][chosenY]->color != curPlayer->piece)
				{
					player* opponent = getOpponent();
					for(unsigned int i = 0; i < opponent->pieces.size(); i++)
					{
						if(opponent->pieces[i] == board[chosenX][chosenY])
						{
							opponent->pieces[i]->isVisible = false;
							break;
						}
					}

					board[chosenX][chosenY] = NULL;
					opponent->numPieces--;
					moveStack[moveStack.size()-1].x3 = chosenX;
					moveStack[moveStack.size()-1].y3 = chosenY;
					moveStack[moveStack.size()-1].removed = true;
					resetTimer = true;
					passTurn();

					if(curPlayer->type == player::HUMAN)
					{
						state = ROTATING_CAMERA;
					}
					else
					{
						state = AI_MOVE;
					}
				}

				resetSelection();
			}
		}
	}
}

void ChokoState::toggleVoiceCommands()
{
	if(!useVoice)
	{
		voce::setRecognizerEnabled(false);
	}
	else
	{
		voce::setRecognizerEnabled(true);
	}
}

gamePiece::gamePiece(char color, float x, float y, float z, int numScenes)
{
	this->color = color;
	this->x = x;
	this->y = y;
	this->z = z;
	this->curAnimation = NULL;
	displayListID.resize(numScenes);
	for(unsigned int i=0; i < numScenes; i++)
		displayListID[i] = 0;
	inBoard = false;
	needsUpdate = false;
	isVisible = true;
}

void gamePiece::drawsubtree(sceneNode* model){
	glPushMatrix();

	model->applyAnimationTranslation();
	model->applyMatrixAndAppearance();
	model->applyAnimationRotation();

	for(unsigned int i = 0; i < model->getPrimitives().size(); i++)
	{
		model->getPrimitives()[i]->draw();
	}

	for(unsigned int i = 0; i < model->getChildren().size(); i++)
	{
		drawsubtree(model->getChildren()[i]);
	}
	glPopMatrix();
}

void gamePiece::draw(sceneNode* model)
{
	if(isVisible && YAFScene::drawHUD)
	{
		if(model->isUsingDisplayList()){
			glPushMatrix();
			if(curAnimation != NULL)
				curAnimation->applyTranslations();
			glCallList(displayListID[YAFScene::selectedScene]);
			glPopMatrix();
			return;
		}
		glPushMatrix();



		model->applyAnimationTranslation();

		if(curAnimation != NULL)
			curAnimation->applyTranslations();
		glTranslatef(x,y,z);
		model->applyMatrixAndAppearance();

		model->applyAnimationRotation();


		for(unsigned int i = 0; i < model->getPrimitives().size(); i++)
		{
			model->getPrimitives()[i]->draw();
		}
		for(unsigned int i = 0; i < model->getChildren().size(); i++)
		{
			drawsubtree(model->getChildren()[i]);
		}
		glPopMatrix();
	}
}

void gamePiece::generateDisplayLists( vector<sceneNode*> model )
{

	for(unsigned int i=0; i < displayListID.size(); i++){
		if(i != YAFScene::selectedScene)
			glColorMask(false, false, false, false);
		if(model[i]->isUsingDisplayList()){
			displayListID[i] = glGenLists(1);
			glNewList(displayListID[i],GL_COMPILE);
		}
		else if(i != YAFScene::selectedScene){
			glColorMask(true, true, true, true);
			continue;
		}
		glPushMatrix();

		if(curAnimation != NULL)
			curAnimation->applyTranslations();

		glTranslatef(x,y,z);
		model[i]->applyMatrixAndAppearance();

		for(unsigned int j = 0; j < model[i]->getPrimitives().size(); j++)
		{

			model[i]->getPrimitives()[j]->draw();
		}
		for(unsigned int j = 0; j < model[i]->getChildren().size(); j++)
		{
			drawsubtree(model[i]->getChildren()[j]);
		}

		glPopMatrix();
		if(model[i]->isUsingDisplayList()){
			glEndList();
		}
		glColorMask(true, true, true, true);
	}
}

void gamePiece::generateAnimation( float destX, float destY, float destZ )
{
	vector<myPoint> ctrlPoints;

	myPoint destPoint = myPoint(destX - x, destY - y, destZ - z);
	myPoint eightPoint =  myPoint(destPoint.x/8.0, sqrt(7.0)/4.0*sqrt(destPoint.x*destPoint.x + destPoint.z*destPoint.z)/2.0, destPoint.z/8.0);
	myPoint quarterPoint = myPoint(destPoint.x/4.0, sqrt(3.0)/2.0*sqrt(destPoint.x*destPoint.x + destPoint.z*destPoint.z)/2.0, destPoint.z/4.0);
	myPoint threeEightPoint =  myPoint(3.0*destPoint.x/8.0, sqrt(15.0)/4.0*sqrt(destPoint.x*destPoint.x + destPoint.z*destPoint.z)/2.0, 3*destPoint.z/8.0);
	myPoint halfwayPoint = myPoint(destPoint.x/2.0,sqrt(destPoint.x*destPoint.x + destPoint.z*destPoint.z)/2.0,destPoint.z/2.0);
	myPoint fiveEightPoint =  myPoint(5.0*destPoint.x/8.0, sqrt(15.0)/4.0*sqrt(destPoint.x*destPoint.x + destPoint.z*destPoint.z)/2.0, 5.0*destPoint.z/8.0);
	myPoint threeQuarterPoint = myPoint(3.0*destPoint.x/4.0, sqrt(3.0)/2.0*sqrt(destPoint.x*destPoint.x + destPoint.z*destPoint.z)/2.0, 3.0*destPoint.z/4.0);
	myPoint sevenEightPoint =  myPoint(7.0*destPoint.x/8.0, sqrt(7.0)/4.0*sqrt(destPoint.x*destPoint.x + destPoint.z*destPoint.z)/2.0, 7.0*destPoint.z/8.0);

	ctrlPoints.push_back(eightPoint);
	ctrlPoints.push_back(quarterPoint);
	ctrlPoints.push_back(threeEightPoint);
	ctrlPoints.push_back(halfwayPoint);
	ctrlPoints.push_back(fiveEightPoint);
	ctrlPoints.push_back(threeQuarterPoint);
	ctrlPoints.push_back(sevenEightPoint);
	ctrlPoints.push_back(destPoint);
	delete curAnimation;
	curAnimation = new LinearAnimation(ctrlPoints, sqrt(destPoint.x*destPoint.x + destPoint.z*destPoint.z)/20);
}

void player::reset(bool initiative)
{
	for(int i = 0; i < pieces.size(); i++)
	{
		pieces[i]->isVisible = true;
		pieces[i]->inBoard = false;
		delete pieces[i]->curAnimation;
		pieces[i]->curAnimation = NULL;
		pieces[i]->needsUpdate = true;

		if(piece == 'b')
		{
			pieces[i]->x = -16 - 2*(i % 2);
			pieces[i]->y = 0;
			pieces[i]->z = -18 + (i*3);
		} 
		else
		{
			pieces[i]->x = 16 + 2*(i % 2);
			pieces[i]->y = 0;
			pieces[i]->z = -18 + (i*3); 
		}
	}

	this->hasInitiative = initiative;
	this->numPieces = NUM_PIECES;
	this->numPiecesInHand = NUM_PIECES;
}
