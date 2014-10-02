#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <errno.h>
#include <fcntl.h>
#include <exception>


#include "RoboteqDevice.h"
#include "ErrorCodes.h"
#include "Constants.h"

using namespace std;

// Named pipes.
static string pnli = "/tmp/pipeli";
static string pnri = "/tmp/piperi";
static string pnci = "/tmp/pipeci";
static string pnlo = "/tmp/pipelo";
static string pnro = "/tmp/pipero";
static string pnco = "/tmp/pipeco";
ofstream pipeli, piperi, pipeci;
ifstream pipelo, pipero, pipeco;

// Roboteq Devices
RoboteqDevice deviceLeft, deviceRight, deviceChair;

bool commandL(int cmd){
	try{
		int result0 = deviceLeft.SetCommand(_GO,0,cmd);
		int result1 = deviceLeft.SetCommand(_GO,1,cmd);
		if(result0 != RQ_SUCCESS and result1 != RQ_SUCCESS){
			throw(new runtime_error("Left motor exception on command %d" + to_string(cmd)));
		}
	}
	catch(...){
		cout << "Command (" << cmd << ") to left motor failed." << endl;
		return false;
	}
	return true;
}

bool commandR(int cmd){
	try{
		int result0 = deviceRight.SetCommand(_GO,0,cmd);
		int result1 = deviceRight.SetCommand(_GO,1,cmd);
		if(result0 != RQ_SUCCESS and result1 != RQ_SUCCESS){
			throw(new runtime_error("Right motor exception on command %d" + to_string(cmd)));
		}
	}
	catch(...){
		cout << "Command (" << cmd << ") to right motor failed." << endl;
		return false;
	}
	return true;
}

/* Send command to chair motor. */
bool commandC(int cmd){
	try{
		int result0 = deviceChair.SetCommand(_GO,0,cmd);
		if(result0 != RQ_SUCCESS){
			throw(new runtime_error("Chair motor exception on command %d" + to_string(cmd)));
		}
	}
	catch(exception& e){
		cout << e.what() << endl;
	}
	catch(...){
		cout << "Command (" << cmd << ") to chair motor failed." << endl;
		return false;
	}
	return true;
}

/* Disconnect motors if they are connected. */
void disconnect(){
	if(deviceLeft.IsConnected())
		deviceLeft.Disconnect();
	if(deviceRight.IsConnected())
		deviceRight.Disconnect();
	if(deviceChair.IsConnected())
		deviceChair.Disconnect();
}

/* Close input and output pipes. */
void closePipes(){
	pipeli.close();
	piperi.close();
	pipeci.close();
	pipelo.close();
	pipero.close();
	pipeco.close();
}
int main(int argc, char *argv[])
{
	string response = "";

	// Serial ports for the motors
	string devnameLeft = "/dev/tty.usbmodem22";
	string devnameRight = "/dev/tty.usbmodemRTQ0011";
	string devnameChair = "/dev/tty.usbmodemfd1211";

	// Assign new device names if they are available in the input
	if(argc>1){
		devnameLeft = argv[1];
	}
	if(argc>2){
		devnameRight = argv[2];
	}
	if(argc>3){
		devnameChair = argv[3];
	}


	// Open output (to us) pipes.
	pipeli.open(pnli);
	piperi.open(pnri);
	pipeci.open(pnci);

	// Open input (to us) pipes.
	cout << "Opening left pipe..." << endl;
	pipelo.open(pnlo);
	cout << "Opening right pipe..." << endl;
	pipero.open(pnro);
	cout << "Opening chair pipe..." << endl;
	pipeco.open(pnco);

	// Wait for 4 zeros from Processing
	// Read in 4 zeros.
	for(int i=1; i<=4; i++){
		int jnk;
		cin >> jnk;
	}

	cout << "Opening left motor...";
	int status = deviceLeft.Connect(devnameLeft);
	if(status != RQ_SUCCESS){
		cout<<"Error connecting to device " << devnameLeft << ": "<<status<<"."<<endl;
		return 1;
	}
	cout << "done.\nOpening right motor...";


	status = deviceRight.Connect(devnameRight);
	if(status != RQ_SUCCESS)
	{
		cout << "Error connecting to device " << devnameRight << ": " << status << "."<<endl;
		return 1;
	}
	cout << "done.\nOpening chair motor...";
	status = deviceChair.Connect(devnameChair);
	if(status != RQ_SUCCESS){
		cout << "Error connecting to device " << devnameChair << ": " << status << "."<<endl;
		return 1;
	}
	cout << "done.\n";

	// Read in input from cin and process commands out to devices.
	while(1){
		cout << "Enter command: <device> <[-999,999]." << endl;


		string devs, cmds;
		cin >> devs;
		cin >> cmds;
		int devi, cmdi;
		try{
			devi = stoi(devs);
			cmdi = stoi(cmds);
		}
		catch(...){
			printf("Read: %s,%s\n",devs.c_str(),cmds.c_str());
			devi = -1;
			cmdi = -1;
		}

		cout << "Read : " << devi << "," << cmdi << endl;


		if(not( (devi>= -2 && devi<=3) and (abs(cmdi)<1000) )){
			continue;
		}

		// Send command to device
		switch(devi){
		case -1:
			break;
		case 1:
			commandL(cmdi);
			break;
		case 2:
			commandR(cmdi);
			break;
		case 3:
			commandC(cmdi);
			break;
		default:
			cout << "Command not handled." << endl;
		}

		// Exit if -1.
		if(devi == -1){
			cout << "Disconnecting...";
			disconnect();
			cout << "done. Exiting." << endl;
			return 0;
		}

		sleepms(5);
	}

}

void run(){
}
