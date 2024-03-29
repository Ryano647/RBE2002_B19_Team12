/*
 * Pose.cpp
 *
 *  Created on: Nov 14, 2019
 *      Author: Brian
 */
#include "Pose.h"

Pose::Pose(double x, double y, double theta) {
	this->x = x;
	this->y = y;
	this->theta = theta;
}

void Pose:: updateEncoderPositions(double timestamp, double encoder0, double encoder1, double IMUheading){  //same code from java, transposed into eclipse second order forward kinematic model
	if (lastTimestamp > 0) {
		double deltaTime = timestamp - lastTimestamp;
		double deltaEncoder0 = encoder0 - lastEncoder0;
		double deltaEncoder1 = encoder1 - lastEncoder1;
		double deltaIMU = IMUheading - lastIMUHeading;
		if (abs(deltaEncoder1) > 0 && abs(deltaEncoder0) > 0) {
			//Serial.println("Delta Time=" + String(deltaTime) + " enc0=" + String(deltaEncoder0) + " enc1=" + String(deltaEncoder1)+ " heading=" + String(deltaIMU));
		}

		// do the Forward Kinematic update

		double speedofLeftWheel = (-deltaEncoder1 / deltaTime) * (PI / 180) * wheelRadiusMM; //   m/s  -1 on deltaEncoder 1 may be changed after negative issue is resolved
		double speedofRightWheel = (deltaEncoder0 / deltaTime) * (PI / 180) * wheelRadiusMM;
		double initialVelocity = (speedofLeftWheel + speedofRightWheel)/2;
		double angularVelocity = (speedofRightWheel - speedofLeftWheel) / wheelTrackMM;
		double nextAngularPosition = (angularVelocity * deltaTime) + theta;


		double changeX = deltaTime * initialVelocity * cos((theta + nextAngularPosition)/2);
		x = x + changeX;
		double changeY = deltaTime * initialVelocity * sin((theta + nextAngularPosition)/2);
		y = y + changeY;

		theta = nextAngularPosition;

	Serial.println("Final pose x= " + String(x) + " y= " + String(y) + " theta= " + String(theta) + " IMU Heading: " + String(((IMUheading) * (PI/180)))); //print of pose

	}
	lastEncoder0 = encoder0;  //reset values for next loop through
	lastEncoder1 = encoder1;
	lastTimestamp = timestamp;
	lastIMUHeading = IMUheading;


}






