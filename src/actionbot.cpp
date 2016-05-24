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

// Roboteq Devices
RoboteqDevice deviceLeft, deviceRight, deviceChair;

bool commandL(int cmd, int dur){
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

bool commandR(int cmd, int dur){
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
bool commandC(int cmd, int dur){
	for(int i=0; i<dur; i+=10){
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
		sleepms(10);
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

int main(int argc, char *argv[])
{
	int verbose = 1;

	string response = "";

	// Serial ports for the motors
	string devnameLeft = "/dev/cu.usbmodem9";
	string devnameRight = "/dev/cu.usbmodemRTQ0011";
	string devnameChair = "/dev/cu.usbmodemfd1211";

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


	if(devnameLeft != " "){
		if(verbose>0){
			cout << "Opening left motor...";
		}

		int status = deviceLeft.Connect(devnameLeft);
		if(status != RQ_SUCCESS){
			cerr<<"Error connecting to device " << devnameLeft << ": "<<status<<"."<<endl;
			//return 1;
		}
	}


	if(devnameRight != " "){
		if(verbose>0){
			cout << "done.\nOpening right motor...";
		}

		int status = deviceRight.Connect(devnameRight);
		if(status != RQ_SUCCESS)
		{
			cerr << "Error connecting to device " << devnameRight << ": " << status << "."<<endl;
			//return 1;
		}
	}

	if(devnameChair != " "){
		if(verbose>0){
			cout << "done.\nOpening chair motor...";
		}

		int status = deviceChair.Connect(devnameChair);
		if(status != RQ_SUCCESS){
			cerr << "Error connecting to device " << devnameChair << ": " << status << "."<<endl;
			//return 1;
		}
		if(verbose>0){
			cout << "done.\n";
		}
	}

	// Read in input from cin and process commands out to devices.
	while(1){
		if(verbose>1){
			cout << "Enter command: <device> <[-999,999]> <duration>." << endl;
		}

		string devs, cmds, durs;
		cin >> devs;
		cin >> cmds;
		cin >> durs;
		int devi, cmdi, duri;
		try{
			devi = stoi(devs);
			cmdi = stoi(cmds);
			duri = stoi(durs);
		}
		catch(...){
			if(verbose>0){
				printf("Failed to parse: %s,%s,%s\n",devs.c_str(),cmds.c_str(),durs.c_str());
			}
			continue;
			//devi = -1;
			//cmdi = -1;
		}

		if(verbose>2){
			cout << "Read : " << devi << "," << cmdi << "," << duri << endl;
		}


		if(not( (devi>= -2 && devi<=3) and (abs(cmdi)<1000) and duri>0)){
			continue;
		}

		// Send command to device
		switch(devi){
		case -1:
			break;
		case 1:
			commandL(cmdi,duri);
			break;
		case 2:
			commandR(cmdi,duri);
			break;
		case 3:
			commandC(cmdi,duri);
			break;
		default:
			cerr << "Command not handled: " << devi << endl;
		}

		// Exit if -1.
		if(devi == -1){
			if(verbose>0){
				cout << "Disconnecting...";
			}
			disconnect();
			if(verbose>0){
				cout << "done. Exiting." << endl;
			}
			return 0;
		}

		sleepms(10);
	}

}
