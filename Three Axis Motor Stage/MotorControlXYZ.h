//Limit switch code is commented out because the limit switches aren't installed yet.

#pragma once
#include <windows.h>
#include <string>

class MotorControlXYZ
{
public:
    LPCWSTR Port; // COM Port
    HANDLE handle; // Serial handle
    // Constructors
    MotorControlXYZ();
    MotorControlXYZ(LPCWSTR P);

    // Basic functions
    void serialSetup(LPCWSTR cport);
    void serialClose();
    void serialWrite(const std::string& strTransmit);
    std::string serialRead();

    // Get functions
    int getPos(MotorControlXYZ obj, int axis)
    {
        serialWrite("RX");
    }

    // General Setting
    bool Joy = false; // Joystick ON/OFF
    bool Message = true; // Message On/OFF

    // Motor Setting
    // X-axis
    bool X_En = true; // the motor driver on/off
    double X_a = 1000; // X axis acceleration
    bool X_jog = false; // jogging mode of the X-axis.
    double X_Vmax = 1000; // the maximum step rate of the X-axis to value.
    double X_status = 0; // the status of X-axis
    double X_Pos = 0; // X axis position % the value of X-axis step counter.
    bool X_Mov = false; // move mode
    

    /*
    bool X_PosLim; // Positive Limit Switch
    bool X_NegLim; // Negative Limit Switch
    bool X_HomeLim; // Home Limit Switch
    */

    // Y-axis
    bool Y_En = true; // the motor driver on/off
    double Y_a = 1000; // Y axis acceleration
    bool Y_jog = false; // jogging mode of the Y-axis.
    double Y_Vmax = 1000; // the maximum step rate of the Y-axis to value.
    double Y_status = 0; // the status of Y-axis
    double Y_Pos = 0; // Y axis position % the value of Y-axis step counter.
    bool Y_Mov = false; // move mode

    /*
    bool Y_PosLim; // Positive Limit Switch
    bool Y_NegLim; // Negative Limit Switch
    bool Y_HomeLim; // Home Limit Switch
    */

    // Z-axis
    bool Z_En = true; // the motor driver on/off
    double Z_a = 1000; // Z axis acceleration
    bool Z_jog = false; // jogging mode of the Z-axis.
    double Z_Vmax = 1000; // the maximum step rate of the Z-axis to value.
    double Z_status = 0; // the status of Z-axis
    double Z_Pos = 0; // Z axis position % the value of Z-axis step counter.
    bool Z_Mov = false; // move mode

    /*
    bool Z_PosLim; // Positive Limit Switch
    bool Z_NegLim; // Negative Limit Switch
    bool Z_HomeLim; // Home Limit Switch
    */ 
};

