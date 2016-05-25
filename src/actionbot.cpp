#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <errno.h>
#include <fcntl.h>
#include <exception>
#include <vector>


#include "RoboteqDevice.h"
#include "ErrorCodes.h"
#include "Constants.h"

using namespace std;

const char* COMMAND_DELIMITER = " ";
const int DEFAULT_SLEEP_MS = 10;

/* Busy block. */
void busyBlock(int ms){
	sleepms(ms);
}
void bb(){
	busyBlock(DEFAULT_SLEEP_MS);
}

// Roboteq Devices
RoboteqDevice deviceLeft, deviceRight, deviceChair;

//TODO: implement duration
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

//TODO: implement duration
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
		bb();
	}
	return true;
}

/* Disconnect motors if they are connected. */
void disconnect(){
	cout << "Disconnecting from controllers...";
	if(deviceLeft.IsConnected())
		deviceLeft.Disconnect();
	if(deviceRight.IsConnected())
		deviceRight.Disconnect();
	if(deviceChair.IsConnected())
		deviceChair.Disconnect();
	cout << "done." << endl;
}

int main(int argc, char *argv[])
{
	atexit(disconnect); // disconnect devices before terminating.
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
	cout << "Enter command: <device> <[-999,999]> <duration>." << endl;
	cout.flush();
	while(1){
		if(verbose>1){
			cout << "Enter command: <device> <[-999,999]> <duration>." << endl;
		}

		string devs, cmds, durs;
		string line;
		int devi, cmdi, duri;
		if( getline(std::cin, line) ){
			try{
				vector<char> temp(line.begin(), line.end());
				temp.push_back('\0');
				char * temp2 = temp[0];
				devi = stoi(strtok(temp2, COMMAND_DELIMITER));
				cmdi = stoi(strtok(temp2, COMMAND_DELIMITER));
				duri = stoi(strtok(temp2, COMMAND_DELIMITER));
			}
			catch(...){
				if(verbose)
					printf("Failed to parse: %s", line.c_str());
				busyBlock(DEFAULT_SLEEP_MS);
				continue;
			}
		}
		else{
			if(cin.fail()){
				cout << "Failed to read input." << endl;
				busyBlock(DEFAULT_SLEEP_MS);
				continue;
			}
			if(cin.eof()){
				cout << "End of input reached. Exiting." << endl;
				exit(0);
			}
		}

//		cin >> devs;
//		cin >> cmds;
//		cin >> durs;
//
//		try{
//			devi = stoi(devs);
//			cmdi = stoi(cmds);
//			duri = stoi(durs);
//		}
//		catch(...){
//			if(verbose>0){
//				printf("Failed to parse: %s,%s,%s\n",devs.c_str(),cmds.c_str(),durs.c_str());
//			}
//			continue;
//			//devi = -1;
//			//cmdi = -1;
//		}

		if(verbose>2){
			cout << "Read : " << devi << "," << cmdi << "," << duri << endl;
		}


		if(not( (devi>= -2 && devi<=3) and (abs(cmdi)<1000) and duri>0)){
			busyBlock(DEFAULT_SLEEP_MS);
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
			exit(0);
		}

		busyBlock(DEFAULT_SLEEP_MS);
	}

}
