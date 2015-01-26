#include "YAFInterface.h"


// buffer to be used to store the hits during picking
#define BUFSIZE 256
GLuint selectBuf[BUFSIZE];

YAFInterface::YAFInterface(vector<pair<string, YAFReader> > &yafFiles)
{
	this->yafFiles = yafFiles;
}

void YAFInterface::processKeyboard(unsigned char key, int x, int y)
{
	//maybe someday
}

void YAFInterface::initGUI()
{
	GLUI_Panel* newGamePanel = addPanel("New Game");
	addButtonToPanel(newGamePanel, "Start New Game", RESTART_BUTTON_ID);

	//---------------------------------------------------------------------------------------
	//                           PLAYER 1 LISTBOX
	//---------------------------------------------------------------------------------------
	GLUI_Listbox *player1Listbox = addListboxToPanel(newGamePanel,"Player 1",&(((YAFScene*) scene)->game->getPlayer(1)->type),PLAYER_1_ID);

	char *cstr = new char[50];
	strcpy(cstr,"Human");

	player1Listbox->add_item(player::HUMAN,cstr);

	strcpy(cstr,"AI easy");

	player1Listbox->add_item(player::AI_EASY,cstr);

	strcpy(cstr,"AI medium");

	player1Listbox->add_item(player::AI_MEDIUM,cstr);

	strcpy(cstr,"AI hard");

	player1Listbox->add_item(player::AI_HARD,cstr);

	//----------------------------------------------------------------------------------------
	//							PLAYER 2 LISTBOX
	//----------------------------------------------------------------------------------------
	GLUI_Listbox *player2Listbox = addListboxToPanel(newGamePanel,"Player 2",&(((YAFScene*) scene)->game->getPlayer(2)->type),PLAYER_2_ID);

	strcpy(cstr,"Human");

	player2Listbox->add_item(player::HUMAN,cstr);

	strcpy(cstr,"AI easy");

	player2Listbox->add_item(player::AI_EASY,cstr);

	strcpy(cstr,"AI medium");

	player2Listbox->add_item(player::AI_MEDIUM,cstr);

	strcpy(cstr,"AI hard");

	player2Listbox->add_item(player::AI_HARD,cstr);
	delete [] cstr;

	//----------------------------------------------------------------------------------------
	//							OPTIONS
	//----------------------------------------------------------------------------------------
	GLUI_Panel* moveTimeoutPanel = addPanel("Move Duration");
	addCheckboxToPanel(moveTimeoutPanel, "Enable Timeouts", &(((YAFScene*)scene)->game->useTimeOut), ENABLE_TIMEOUTS_ID);
	GLUI_Spinner* timeoutSpinner = addSpinnerToPanel(moveTimeoutPanel, "Max Duration (s)",GLUI_SPINNER_INT, &(((YAFScene*)scene)->game->timeOutLimit), TIMEOUT_VALUE);
	timeoutSpinner->set_int_limits(3,600,GLUI_LIMIT_CLAMP);

	addColumn();
	GLUI_Panel* optionsPanel = addPanel("Options");
	addButtonToPanel(optionsPanel, "Undo Move", UNDO_BUTTON_ID);

	addButtonToPanel(optionsPanel, "Pause/Unpause", PAUSE_BUTTON_ID);

	addColumnToPanel(optionsPanel);

	addButtonToPanel(optionsPanel, "Play Replay", WATCH_REPLAY_ID);

	addButtonToPanel(optionsPanel, "Skip Replay", SKIP_REPLAY_ID);

	addCheckboxToPanel(optionsPanel, "Voice Commands", (int*) &(((YAFScene*) scene)->game->useVoice), VOICE_COMMANDS_ID);


	//----------------------------------------------------------------------------------------
	//							SCENE SELECTION
	//----------------------------------------------------------------------------------------
	GLUI_Panel *appearancePanel = addPanel("Scenario");
	GLUI_Listbox *appearanceListbox = addListboxToPanel(appearancePanel,"Current ",&(((YAFScene*) scene)->selectedScene),GAME_SCENES_ID);
	for(unsigned int i=0; i < yafFiles.size(); i++){
		char *cstr = new char[yafFiles[i].first.length() + 1];
		strcpy(cstr,yafFiles[i].first.c_str());
		appearanceListbox->add_item(i,cstr);
		delete [] cstr;
	}
	addCheckboxToPanel(appearancePanel, "Skip camera rotation", &(((YAFScene*)scene)->skipCameraRotation), CAMERA_ROTATION_TOGGLE_ID);


	addColumn();

	//----------------------------------------------------------------------------------------
	//							FILE I/O
	//----------------------------------------------------------------------------------------
	GLUI_Panel *fileIOPanel = addPanel("Save/Load");
	GLUI_Listbox *filenames = addListboxToPanel(fileIOPanel,"Slot ",&selectedSaveSlot, FILE_SLOTS_ID);
	for(unsigned int i=0; i < 100; i++){
		char *cstr = new char[7];
		strcpy(cstr,"save00");
		cstr[4] += i/10;
		cstr[5] += i%10;
		filenames->add_item(i,cstr);
		delete [] cstr;
	}
	addButtonToPanel(fileIOPanel, "Load", LOAD_BUTTON_ID);
	addButtonToPanel(fileIOPanel, "Save", SAVE_BUTTON_ID);

}

void YAFInterface::processGUI(GLUI_Control *ctrl)
{
	if(ctrl->user_id == RESTART_BUTTON_ID)
	{
		((YAFScene*) scene)->game->restartGame();
	}
	else if (ctrl->user_id == UNDO_BUTTON_ID)
	{
		((YAFScene*) scene)->game->undoMove();
	}
	else if (ctrl->user_id == PAUSE_BUTTON_ID)
	{
		((YAFScene*) scene)->game->togglePause();
	}
	else if(ctrl->user_id == GAME_SCENES_ID)
	{
		((YAFScene*) scene)->sceneChanged = true;
	}
	else if(ctrl->user_id == WATCH_REPLAY_ID)
	{
		((YAFScene*) scene)->game->startReplay();
	}
	else if(ctrl->user_id == SKIP_REPLAY_ID)
	{
		((YAFScene*) scene)->game->skipReplay();
	}
	else if(ctrl->user_id == SAVE_BUTTON_ID)
	{
		((YAFScene*) scene)->game->saveMovesToFile();
	}
	else if(ctrl->user_id == LOAD_BUTTON_ID)
	{
		((YAFScene*) scene)->game->loadGame();
	}
	else if(ctrl->user_id == FILE_SLOTS_ID)
	{
		((YAFScene*) scene)->game->chokoFilename = "save00."+((YAFScene*) scene)->game->savesFileExtention;
		((YAFScene*) scene)->game->chokoFilename[4] += selectedSaveSlot/10;
		((YAFScene*) scene)->game->chokoFilename[5] += selectedSaveSlot%10;
	}
	else if (ctrl->user_id == VOICE_COMMANDS_ID)
	{
		((YAFScene*) scene)->game->toggleVoiceCommands();
	}

}

void YAFInterface::processMouseMoved(int x, int y) {

	displacementX = x- prev_X;
	displacementY = y- prev_Y;
	prev_X = x;
	prev_Y = y;

	if(pressing_left && (modifiers & GLUT_ACTIVE_SHIFT))
	{
		((YAFScene*)scene)->rotateCamera(CG_CGFcamera_AXIS_X, displacementY*MOUSE_ROTATE_FACTOR);
		((YAFScene*)scene)->rotateCamera(CG_CGFcamera_AXIS_Y, displacementX*MOUSE_ROTATE_FACTOR);
	}
	else if(pressing_right && (modifiers & GLUT_ACTIVE_SHIFT))
	{
		((YAFScene*)scene)->translateCamera(CG_CGFcamera_AXIS_X, displacementX*MOUSE_PAN_FACTOR);
		((YAFScene*)scene)->translateCamera(CG_CGFcamera_AXIS_Y, -displacementY*MOUSE_PAN_FACTOR);
	}
	else if(pressing_middle || (pressing_left && modifiers & GLUT_ACTIVE_CTRL))
	{
		((YAFScene*)scene)->translateCamera(CG_CGFcamera_AXIS_Z, displacementY*MOUSE_ZOOM_FACTOR);
	}
	else if(pressing_left && modifiers & GLUT_ACTIVE_ALT){
		((YAFScene*)scene)->rotateCamera(CG_CGFcamera_AXIS_Z, displacementX*MOUSE_ROTATE_FACTOR);
	}

	glutPostRedisplay();
}

void YAFInterface::processMouse(int button, int state, int x, int y)
{
	CGFinterface::processMouse(button,state, x, y);

	// do picking on mouse press (GLUT_DOWN)
	// this could be more elaborate, e.g. only performing picking when there is a click (DOWN followed by UP) on the same place
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && modifiers == 0)
		performPicking(x,y);
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		((YAFScene*)scene)->game->resetSelection();
}

void YAFInterface::performPicking(int x, int y) 
{
	// Sets the buffer to be used for selection and activate selection mode
	glSelectBuffer (BUFSIZE, selectBuf);
	glRenderMode(GL_SELECT);

	// Initialize the picking name stack
	glInitNames();

	// The process of picking manipulates the projection matrix
	// so we will be activating, saving and manipulating it
	glMatrixMode(GL_PROJECTION);

	//store current projmatrix to restore easily in the end with a pop
	glPushMatrix ();

	//get the actual projection matrix values on an array of our own to multiply with pick matrix later
	GLfloat projmat[16];
	glGetFloatv(GL_PROJECTION_MATRIX,projmat);

	// reset projection matrix
	glLoadIdentity();

	// get current viewport and use it as reference for 
	// setting a small picking window of 5x5 pixels around mouse coordinates for picking
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// this is multiplied in the projection matrix
	gluPickMatrix ((GLdouble) x, (GLdouble) (CGFapplication::height - y), 5.0, 5.0, viewport);

	// multiply the projection matrix stored in our array to ensure same conditions as in normal render
	glMultMatrixf(projmat);

	// force scene drawing under this mode
	// only the names of objects that fall in the 5x5 window will actually be stored in the buffer
	((YAFScene*)scene)->drawHUD = false;
	((YAFScene*)scene)->display();
	((YAFScene*)scene)->drawHUD = true;

	// restore original projection matrix
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();

	glFlush();

	// revert to render mode, get the picking results and process them
	GLint hits;
	hits = glRenderMode(GL_RENDER);
	processHits(hits, selectBuf);
}

void YAFInterface::processHits (GLint hits, GLuint buffer[]) 
{
	GLuint *ptr = buffer;
	GLuint mindepth = 0xFFFFFFFF;
	GLuint *selected=NULL;
	GLuint nselected;

	// iterate over the list of hits, and choosing the one closer to the viewer (lower depth)
	for (int i=0;i<hits;i++) {
		int num = *ptr; ptr++;
		GLuint z1 = *ptr; ptr++;
		ptr++;
		if (z1 < mindepth && num>0) {
			mindepth = z1;
			selected = ptr;
			nselected=num;
		}
		for (int j=0; j < num; j++) 
			ptr++;
	}

	// if there were hits, the one selected is in "selected", and it consist of nselected "names" (integer ID's)
	if (selected!=NULL)
	{
		/*printf("Picked ID's: ");
		for (int i=0; i<nselected; i++)
			printf("%d ",selected[i]);
		printf("\n");
		*/
		((YAFScene*) scene)->game->handleInput(selected[0],selected[1]);
	}
	/*else
		printf("Nothing selected while picking \n");*/	
}
