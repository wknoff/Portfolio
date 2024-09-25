#include <iostream>
#include "MotorControlXYZ.h"

int main()
{
    HANDLE hSerial;
    // Left USB port on front panel of lab computer is COM5, right is COM7
    MotorControlXYZ obj(L"COM5");
    obj.serialSetup(obj.Port);
	
	
	// Testing functionality
    obj.serialWrite("MSGON");
    obj.serialWrite("JOFF");
    obj.serialWrite("ACCX 1000");
    obj.serialWrite("VELX 1000");
    obj.serialWrite("POSX 1000");
    obj.serialWrite("MOVAX 1000");
    
    obj.serialClose();
}


