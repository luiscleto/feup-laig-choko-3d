#include "YAFScene.h"
#include "CGFapplication.h"

#include "YAFInterface.h"

#include <iostream>
#include <exception>
#include <time.h>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
	string filename;

	CGFapplication app = CGFapplication();

	srand(time(NULL));
	try{
		ChokoCommunication::initConnection();
		ChokoCommunication::srandChoko();

		vector<pair<string, YAFReader> > yafFiles;
		yafFiles.push_back(pair<string,YAFReader>("MIEIC",YAFReader("resources/mieic.yaf")));
		yafFiles.push_back(pair<string,YAFReader>("Naval",YAFReader("resources/naval.yaf")));
		yafFiles.push_back(pair<string,YAFReader>("Stadium",YAFReader("resources/stadium.yaf")));
		yafFiles.push_back(pair<string,YAFReader>("Forest",YAFReader("resources/lotr.yaf")));

		app.init(&argc, argv);
		app.setScene(new YAFScene(yafFiles));
		app.setInterface(new YAFInterface(yafFiles));

		cout << "Camera controls:" << endl;
		cout << "Panning: Right-click, hold down SHIFT and drag on screen" << endl;
		cout << "Rotations: Left-click, hold down SHIFT and drag on screen" << endl;
		cout << "Zoom: Mouse-wheel or CTRL+right-click while dragging the mouse" << endl;
		cout << "Tilt (Z-axis rotation): Hold down ALT and left-click while dragging the mouse" << endl;
		app.run();
		ChokoCommunication::terminateConnection();
	}
	catch(genericException &excp){
		cout << excp.getDescription() << endl;
		getline(cin, filename);
		return -1;
	}
	catch(GLexception& ex) {
		cout << "Erro: " << ex.what();
		getline(cin, filename);
		return -1;
	}
	catch(exception& ex) {
		cout << "Erro inesperado: " << ex.what();
		getline(cin, filename);
		return -1;
	}
return 0;
}