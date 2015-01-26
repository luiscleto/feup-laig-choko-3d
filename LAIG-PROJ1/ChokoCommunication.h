#ifndef _CHOKO_COMMUNICATION_H_
#define _CHOKO_COMMUNICATION_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include "errorhandling.h"

#ifdef linux
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#else
#include <winsock2.h>
#include <Windows.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#define BOARD_SIZE 5


using namespace std;

struct gamePiece;

struct ChokoMove{
	enum MoveType{DROP,STANDARD,CAPTURE_NO_REMOVAL,CAPTURE_AND_REMOVAL, NO_MOVES_AVAILABLE};
	int moveType;
	char pieceType;
	char hadInitiative; //indicates who had the initiative when this move was made
	bool removed;
	int x1,y1; //indicates drop position or origin cell for movement
	int x2,y2; //indicates destination cell for movement (if type is capture, piece between (x1,y1) and (x2,y2) must be removed)
	int x3,y3; //for capture and removal moves, indicates position of extra piece to remove
};

class ChokoCommunication
{
	static const string ip;
	static const int port;
#ifdef linux
	static int sock;
#else
	static SOCKET sock;
#endif
	static string readLine();
public:
	static void initConnection();
	static void srandChoko();
	static bool canPlayerMove(int numPiecesInHand, char playerPieceColor, gamePiece* board[BOARD_SIZE][BOARD_SIZE]);
	static ChokoMove getAIMove(string difficulty, char dropInitiative, char aiPieceType, int numberOfPiecesInHand, gamePiece* board[BOARD_SIZE][BOARD_SIZE]);
	static void terminateConnection();

};

#endif