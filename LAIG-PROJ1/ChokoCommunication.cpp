#include "ChokoCommunication.h"
#include "ChokoState.h"

const string ChokoCommunication::ip = "127.0.0.1";
const int ChokoCommunication::port = 60001;
SOCKET ChokoCommunication::sock = 0;

void ChokoCommunication::initConnection(){
#ifdef linux
	pid_t pid=fork();
	if (pid==0) { /* child process */
		static char *argv[]={"choko",NULL};
		execv("./choko.exe",argv);
		exit(127); /* only if execv fails */
	}
	struct sockaddr_in echoServAddr; /* Echo server address */
	struct  hostent  *ptrh;

	/* Create a reliable, stream socket using TCP */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		ConnectionFailed("socket() failed");

	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
	echoServAddr.sin_family      = AF_INET;             /* Internet address family */
	echoServAddr.sin_port = htons(port);                /* Server port */

	ptrh = gethostbyname("localhost");

	memcpy(&echoServAddr.sin_addr, ptrh->h_addr, ptrh->h_length);

	/* Establish the connection to the echo server */
	if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		ConnectionFailed("connect() failed");
#else
	string command = "choko.exe";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
#if _MSC_VER >= 1700
	wchar_t wtext[20];
	mbstowcs(wtext, command.c_str(), command.size()+1);//Plus null
	LPWSTR command_lpstr = wtext;
	if( !CreateProcess( NULL,   // No module name (use command line)
		command_lpstr,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
		) {
			throw ConnectionFailed("CreateProcess failed.");
	}
#else
	size_t len=command.length();
	LPSTR str2=new char[len+1];
	command._Copy_s(str2,len,len);
	str2[len]='\0';
	if( !CreateProcess( NULL,   // No module name (use command line)
		str2,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
		) {
			throw ConnectionFailed("CreateProcess failed.");
	}
	delete [] str2;
#endif
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR)
		throw ConnectionFailed("WSAStartup failed");

	// Create a socket.
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		WSACleanup();
		throw ConnectionFailed("Error at socket");
	}

	// Connect to a server.
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	// Just test using the localhost, you can try other IP address
	clientService.sin_addr.s_addr = inet_addr(ip.c_str());
	clientService.sin_port = htons(port);

	if (connect(sock, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		WSACleanup();
		throw ConnectionFailed("Failed to connect");
	}
#endif
}

void ChokoCommunication::srandChoko(){
	stringstream ss;

	ss << "srand(";
	ss << rand();
	ss << ").\n";

	int res = send(sock, ss.str().c_str(), ss.str().size(), 0);
	if (res < 0) 
		ConnectionFailed("Unable to send message");
	string answer = readLine();

	stringstream replyStream(answer);
	string result;
	getline(replyStream, result , '.');
	if(result != "ok")
		throw ConnectionFailed("Choko received invalid command");
}

bool ChokoCommunication::canPlayerMove(int numPiecesInHand, char playerPieceColor, gamePiece* board[BOARD_SIZE][BOARD_SIZE]){
	if(numPiecesInHand > 0)
		return true;
	stringstream ss;

	ss << "can_player_move(";
	ss << playerPieceColor << ",";
	ss << "[";
	for(int i=0; i < BOARD_SIZE; i++){
		ss << "[";
		for(int j = 0; j < BOARD_SIZE; j++){
			if(board[i][j] != NULL)
				ss << board[i][j]->color;
			else
				ss << 'e';
			if(j < BOARD_SIZE-1)
				ss << ",";
		}
		ss << "]";
		if(i < BOARD_SIZE-1)
			ss << ",";
	}
	ss << "]).\n";

	int res = send(sock, ss.str().c_str(), ss.str().size(), 0);
	if (res < 0) 
		ConnectionFailed("Unable to send message");
	string answer = readLine();

	stringstream replyStream(answer);
	string result;
	getline(replyStream, result , '.');

	if(result != "ok(yes)" && result != "ok(no)")
		throw ConnectionFailed("Choko received invalid command");
	return result=="ok(yes)";
}

ChokoMove ChokoCommunication::getAIMove(string difficulty, char dropInitiative, char aiPieceType, int numberOfPiecesInHand, gamePiece* board[BOARD_SIZE][BOARD_SIZE]){
	ChokoMove chosenMove;
	stringstream ss;

	if(dropInitiative != 'e' && dropInitiative != aiPieceType)
		ss << "choose_drop(";
	else if(numberOfPiecesInHand > 0)
		ss << "choose_move(";
	else
		ss << "choose_movement(";

	ss << difficulty << "," << dropInitiative << "," << aiPieceType << ",";
	ss << "[";
	for(int i=0; i < BOARD_SIZE; i++){
		ss << "[";
		for(int j = 0; j < BOARD_SIZE; j++){
			if(board[i][j] != NULL)
				ss << board[i][j]->color;
			else
				ss << 'e';
			if(j < BOARD_SIZE-1)
				ss << ",";
		}
		ss << "]";
		if(i < BOARD_SIZE-1)
			ss << ",";
	}
	ss << "]).\n";

	int res = send(sock, ss.str().c_str(), ss.str().size(), 0);
	if (res < 0) 
		ConnectionFailed("Unable to send message");

	string answer = readLine();

	stringstream replyStream(answer);
	string result;
	getline(replyStream, result , '(');
	if(result == "ok"){
		string token;
		vector<string> positions;
		while(getline(replyStream, token, '-' )){
			positions.push_back(token);
		}
		if(positions.size() == 3){
			sscanf(positions[0].c_str(),"%d/%d",&chosenMove.y1,&chosenMove.x1);
			sscanf(positions[1].c_str(),"%d/%d",&chosenMove.y2,&chosenMove.x2);
			sscanf(positions[2].c_str(),"%d/%d",&chosenMove.y3,&chosenMove.x3);
			if(chosenMove.x3 > 0 && chosenMove.y3 > 0)
				chosenMove.moveType = ChokoMove::CAPTURE_AND_REMOVAL;
			else if(abs(chosenMove.x2-chosenMove.x1) > 1 || abs(chosenMove.y2-chosenMove.y1) > 1)
				chosenMove.moveType = ChokoMove::CAPTURE_NO_REMOVAL;
			else
				chosenMove.moveType = ChokoMove::STANDARD;
		}
		else if(positions.size() == 2){

			sscanf(positions[0].c_str(),"%d/%d",&chosenMove.y1,&chosenMove.x1);
			sscanf(positions[1].c_str(),"%d/%d",&chosenMove.y2,&chosenMove.x2);
			chosenMove.moveType = ChokoMove::STANDARD;
		}
		else if(positions.size() == 1){
			sscanf(positions[0].c_str(),"%d/%d",&chosenMove.y1,&chosenMove.x1);
			chosenMove.moveType = ChokoMove::DROP;
		}
		else
			chosenMove.moveType = ChokoMove::NO_MOVES_AVAILABLE;
	}
	else
		throw ConnectionFailed("Choko received invalid command");

	//In our data structure (arrays) the positions range from 0 to 4. in prolog's they range from 1 to 5
	chosenMove.x1--;chosenMove.x2--;chosenMove.x3--;
	chosenMove.y1--;chosenMove.y2--;chosenMove.y3--;
	chosenMove.x1=4-chosenMove.x1;chosenMove.x2=4-chosenMove.x2;chosenMove.x3=4-chosenMove.x3;
	chosenMove.pieceType = aiPieceType;

	chosenMove.hadInitiative = dropInitiative;
	chosenMove.removed = false;
	return chosenMove;
}
string ChokoCommunication::readLine() {
	string msg;
	char ch;
	while (true) {
		recv(sock, &ch, 1, 0);
		if (ch == '\n')
			break;
		msg.push_back(ch);
	}
	return msg;
}
void ChokoCommunication::terminateConnection(){
	int res = send(sock,"bye.\n", 5, 0);
	if (res < 0) 
		throw ConnectionFailed("Unable to send");
	readLine();
}