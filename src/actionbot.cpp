#include <iostream>
#include <stdio.h>
#include <string.h>


#include "RoboteqDevice.h"
#include "ErrorCodes.h"
#include "Constants.h"

using namespace std;


int main(int argc, char *argv[])
{
	string response = "";
	string devnameRight = "/dev/tty.usbmodemRTQ0011";
	string devnameLeft = "/dev/tty.usbmodem22";
	string devnameChair = "/dev/tty.usbmodemfd1211";

	//cout << "hello world!" << endl;
	//cout << argc << endl;
	//cout << *argv << endl;
	//return 0;
	// Assign new device names if they are available in the input
	if(argc>1){
		devnameRight = argv[1];
	}
	if(argc>2){
		devnameLeft = argv[2];
	}
	if(argc>3){
		devnameChair = argv[3];
	}

	//string devname = argv[1];
	RoboteqDevice deviceLeft, deviceRight, deviceChair;
	int status = deviceLeft.Connect(devnameLeft);

	if(status != RQ_SUCCESS){
		cout<<"Error connecting to device " << devnameLeft << ": "<<status<<"."<<endl;
		return 1;
	}

	status = deviceRight.Connect(devnameRight);
	if(status != RQ_SUCCESS)
	{
		cout << "Error connecting to device " << devnameRight << ": " << status << "."<<endl;
		return 1;
	}

	status = deviceChair.Connect(devnameChair);
	if(status != RQ_SUCCESS){
		cout << "Error connecting to device " << devnameChair << ": " << status << "."<<endl;
		return 1;
	}

	// Read in 4 zeros.
	for(int i=1; i<=4; i++){
		int jnk;
		cin >> jnk;
	}
	// Read in input from cin and process commands out to devices.
	while(1){
		/*
		int speedL, speedR, speedC;
		deviceLeft.GetValue(_ABSPEED,speedL);
		deviceRight.GetValue(_ABSPEED,speedR);
		deviceChair.GetValue(_ABSPEED,speedC);


		cout << "L speed: " << speedL <<endl;
		cout << "R speed: " << speedR << endl;
		cout << "C speed: " << speedC << endl;
		*/
		cout << "Enter command: <device> <[-1000,1000]." << endl;


		string devs, cmds;
		cin >> devs;
		cin >> cmds;
		int devi, cmdi;
		try{
			devi = stoi(devs);
			cmdi = stoi(cmds);
		}
		catch(std::invalid_argument &e){
			cerr << e.what();
			continue;
		}

		//cin >> devi;
		//cin >> cmdi;
		cout << "Read : " << devi << "," << cmdi << endl;


		if(not( (devi>= -2 && devi<=3) and (abs(cmdi)<=1000) )){
			continue;
		}

		RoboteqDevice tempdev;
		switch(devi){
		case -1:
			break;
		case 1:
			tempdev = deviceLeft;
			break;
		case 2:
			tempdev = deviceRight;
			break;
		case 3:
			tempdev = deviceChair;
			break;
		}

		// Exit if -1.
		if(devi == -1){
			cout << "Disconnecting...";
			deviceLeft.Disconnect();
			deviceRight.Disconnect();
			deviceChair.Disconnect();
			cout << "done. Exiting." << endl;
			return 0;
		}
		// Send command to device.
		else if(tempdev.IsConnected()){
			status = tempdev.SetCommand(_GO,cmdi);
			if(status != RQ_SUCCESS)
				cout<<"failed --> " << status<<endl;
		}
		else{
			cout << "Command not handled." << endl;
		}

		sleepms(10);
	}

}
