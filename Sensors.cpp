/*
 * Sensors.cpp
 *
 *  Created on: Nov 21, 2019
 *      Author: Peter Nikopoulos
 */

#include "Sensors.h"

Sensors::Sensors() {
	this->triggerPin = -1;
	this->echoPin = -1;

}

int Sensors::attach(int trig, int echo){
	this->triggerPin = trig;
	this->echoPin = echo;
	pinMode(this->triggerPin, OUTPUT);
	digitalWrite(this->triggerPin, LOW);
	pinMode(this->echoPin, INPUT);
	return 0;
}

double Sensors::PingUltrasonic(){
	if(!reading){
		if(trigger){
			dTime = micros();
			digitalWrite(triggerPin, HIGH);
			trigger = false;
		}
		if(!trigger && dTime + 20 < micros()){
			digitalWrite(triggerPin, LOW);
			reading = false;
			trigger = true;
			double pulseWidth = pulseIn(echoPin,HIGH);
			double distance = pulseWidth * (1/58.0) * 10;
			return distance;  //pulsewidth of echoPin multiplied by conversion factor converted to mm from cm
		}
	}
	return -1.0;
}




