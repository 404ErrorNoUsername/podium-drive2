/* 
 *  Simplified Logitech Extreme 3D Pro Joystick Report Parser, ready to go.
*/
#include <Servo.h>
#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>
#include "le3dp_rptparser2.0.h"
#include <SPI.h>

USB                                             Usb;
USBHub                                          Hub(&Usb);
HIDUniversal                                    Hid(&Usb);
JoystickEvents                                  JoyEvents;
JoystickReportParser                            Joy(&JoyEvents);


Servo leftMotor, rightMotor;

#define LEFT_MOTOR_PIN 9
#define RIGHT_MOTOR_PIN 6

void setup()
{  
   Serial.begin(115200);
    
#if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect
#endif

    Serial.println("Start");
    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");
    delay(200);

    if (!Hid.SetReportParser(0, &Joy))
        ErrorMessage<uint8_t>(PSTR("SetReportParser"), 1);


    pinMode(LEFT_MOTOR_PIN, OUTPUT);
    pinMode(RIGHT_MOTOR_PIN, OUTPUT);

    leftMotor.attach(LEFT_MOTOR_PIN);
    rightMotor.attach(RIGHT_MOTOR_PIN);

    // Initialize both motors to neutral (1500 µs)
    leftMotor.writeMicroseconds(1500);
    rightMotor.writeMicroseconds(1500);
    delay(500);

}



 void loop()
{
    Usb.Task();  // Read joystick input

    // Declare joystick values
    int Xval, Yval, Hat, Twist, Slider, Button;
    
    // Fetch joystick values
    JoyEvents.GetValues(Xval, Yval, Hat, Twist, Slider, Button);

    // Debugging output
    Serial.print("Yval: "); Serial.print(Yval);
    Serial.print(" | Twist: "); Serial.print(Twist);
    Serial.print(" | Button: "); Serial.println(Button);

    // Ensure valid joystick input before moving motors
    if (Yval == 0 && Twist == 0 && Button == 0) {
        Serial.println("No valid joystick input detected, keeping motors at neutral.");
        leftMotor.writeMicroseconds(1500);
        rightMotor.writeMicroseconds(1500);
        return;  // Skip further processing
    }
    //int percent = map(Slider, 0,255,25,75)/100;
    // Map joystick Y-axis to forward/backward motion (1000-2000 µs)
    int forwardSpeed = map(Yval, 0, 1023, 1400, 1600);

    // Map twist for zero-point turn (-255 to 255 → -500 to 500 µs)
    int turnAmount = map(Twist, 0, 255, 500, -500);

    //percentage
    
    
    // Calculate motor signals for zero-point turns
    int leftPWM = constrain(forwardSpeed - turnAmount, 1400, 1600);
    int rightPWM = constrain(forwardSpeed + turnAmount, 1400, 1600);

    

    // Send PWM to motors
    leftMotor.writeMicroseconds(leftPWM);
    rightMotor.writeMicroseconds(rightPWM);

    // Debugging output
    Serial.print("Left PWM: "); Serial.print(leftPWM);
    Serial.print(" | Right PWM: "); Serial.println(rightPWM);
    //Serial.print("percent"); Serial.println(percent);
    
    delay(20); // Short delay to prevent excessive updates
}



