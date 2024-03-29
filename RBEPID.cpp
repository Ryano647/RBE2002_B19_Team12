/*
 * Messages.h
 *
 *  Created on: 10/1/16
 *      Author: joest
 *      Author:	Phnikopoulos
 */
#include "Arduino.h"
#include "RBEPID.h"

//Class constructor
RBEPID::RBEPID() {
}

//Function to set PID gain values
void RBEPID::setpid(float P, float I, float D) {
	kp = P;
	ki = I;
	kd = D;

	errorSum = 0;
	Ierr = 0;
	errorIndex = 0;
	this->clearIntegralBuffer(); //check with SA/Lewin if this is the reason why our motors wont spin when pressing the boot button
}

/**
 * calc the PID control signel
 *
 * @param setPoint is the setpoint of the PID system
 * @param curPosition the current position of the plan
 * @return a value from -1.0 to 1.0 representing the PID control signel
 */
float RBEPID::calc(double setPoint, double curPosition) {

	// calculate error
	float err = setPoint - curPosition;
	// calculate derivative of error
	//TODO
	// calculate integral error. Running average is best but hard to implement


	this ->errorArray[this->errorIndex] = err;

	errorSum = 0;
	for(int i = 0; i < 200; i++){
		this->errorSum += errorArray[i];
	}

	this->errorIndex++;

	if(this->errorIndex >= 200){
		this->errorIndex = 0;
	}

	float Ierr = (this->errorSum/200.0);

	float out = err * kp + Ierr *ki;
	// simple P controller
	//return the control signal from -1 to 1
	if (out > 1)
		out = 1;
	if (out < -1)
		out = -1;
	return out ;
}

/**
 * Clear the internal representation fo the integral term.
 *
 */
void RBEPID::clearIntegralBuffer() {
	for(int j = 0; j < 200; j++){
		this->errorArray[j] = 0;
	}
	this->errorSum = 0;
	this->Ierr = 0;
}
