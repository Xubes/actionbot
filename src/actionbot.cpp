#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <errno.h>
#include <fcntl.h>


#include "RoboteqDevice.h"
#include "ErrorCodes.h"
#include "Constants.h"

using namespace std;

// Named pipes.
static string pipeL = "/tmp/pipeL";
static string pipeR = "/tmp/pipeR";
static string pipeC = "/tmp/pipeC";

// Roboteq Devices
RoboteqDevice deviceLeft, deviceRight, deviceChair;

bool commandL(int cmd){
	try{
		int result0 = deviceLeft.SetCommand(_GO,0,cmd);
		int result1 = deviceLeft.SetCommand(_GO,1,cmd);
		if(result0 != RQ_SUCCESS and result1 != RQ_SUCCESS){
			throw(NULL);
		}
	}
	catch(...){
		cout << "command to left motor failed." << endl;
		return false;
	}
	return true;
}

bool commandR(int cmd){
	try{
		int result0 = deviceRight.SetCommand(_GO,0,cmd);
		int result1 = deviceRight.SetCommand(_GO,1,cmd);
		if(result0 != RQ_SUCCESS and result1 != RQ_SUCCESS){
			throw(NULL);
		}
	}
	catch(...){
		cout << "command to right motor failed." << endl;
		return false;
	}
	return true;
}

bool commandC(int cmd){
	try{
		int result0 = deviceChair.SetCommand(_GO,0,cmd);
		int result1 = deviceChair.SetCommand(_GO,1,cmd);
		if(result0 != RQ_SUCCESS and result1 != RQ_SUCCESS){
			throw(NULL);
		}
	}
	catch(...){
		cout << "command to chair motor failed." << endl;
		return false;
	}
	return true;
}
void disconnect(){
	if(deviceLeft.IsConnected())
		deviceLeft.Disconnect();
	if(deviceRight.IsConnected())
		deviceRight.Disconnect();
	if(deviceChair.IsConnected())
		deviceChair.Disconnect();
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

#if 0
	// Open pipes.
	cout << "Opening left pipe..." << endl;
	int fidL[2];
	open(devnameLeft.c_str(), O_RDONLY);
	ifstream fifoL(pipeL, ios::in);
	cout << "done" << endl;
	if(fifoL.is_open()){
		cout << "hi" << endl;
		cerr << "Error opening left pipe\n" << strerror(errno) << endl;
		return 1;
	}
	cout << "done.\nOpening right pipe..." << endl;
	ifstream fifoR(pipeR,ios::in);
	if(!fifoR.is_open()){
		cerr << strerror(errno) << endl;
		return 1;
	}
	cout <<"done.\nOpening chair pipe...";
	ifstream fifoC(pipeC, ios::in);
	if(fifoC.is_open()){
		cerr << strerror(errno) << endl;
	}
	cout << "done.\n";
#endif

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
