/*
 * DrivingChassis.cpp
 *
 *  Created on: Jan 31, 2019
 *      Author: hephaestus
 */

#include "DrivingChassis.h"
#include "Pose.h"

/**
 * Compute a delta in wheel angle to traverse a specific distance
 *
 * arc length	=	2*	Pi*	R*	(C/360)
 *
 * C  is the central angle of the arc in degrees
 * R  is the radius of the arc
 * Pi  is Pi
 *
 * @param distance a distance for this wheel to travel in MM
 * @return the wheel angle delta in degrees
 */
float DrivingChassis::distanceToWheelAngle(float distance) {
	return 0;
}

/**
 * Compute the arch length distance the wheel needs to travel through to rotate the base
 * through a given number of degrees.
 *
 * arc length	=	2*	Pi*	R*	(C/360)
 *
 * C  is the central angle of the arc in degrees
 * R  is the radius of the arc
 * Pi  is Pi
 *
 * @param angle is the angle the base should be rotated by
 * @return is the linear distance the wheel needs to travel given the this CHassis's wheel track
 */
float DrivingChassis::chassisRotationToWheelDistance(float angle) {
	return 0;
}

DrivingChassis::~DrivingChassis() {
	// do nothing
}

/**
 * DrivingChassis encapsulates a 2 wheel differential steered chassis that drives around
 *
 * @param left the left motor
 * @param right the right motor
 * @param wheelTrackMM is the measurment in milimeters of the distance from the left wheel contact point to the right wheels contact point
 * @param wheelRadiusMM is the measurment in milimeters of the radius of the wheels
 * @param imu The object that is used to access the IMU data
 *
 */
DrivingChassis::DrivingChassis(PIDMotor * left, PIDMotor * right,
		float wheelTrackMM, float wheelRadiusMM , GetIMU * imu) : robotPose(0,0,0)  //starts with pose of 0,0,0

{
	Serial.println("DrivingChassis::DrivingChassis constructor called here ");
	myleft = left;
	myright = right;
	robotPose.wheelTrackMM = wheelTrackMM;
	robotPose.wheelRadiusMM = wheelRadiusMM;
	IMU = imu;


}

/**
 * Start a drive forward action
 *
 * @param mmDistanceFromCurrent is the distance the mobile base should drive forward
 * @param msDuration is the time in miliseconds that the drive action should take
 *
 * @note this function is fast-return and should not block
 * @note pidmotorInstance->overrideCurrentPosition(0); can be used to "zero out" the motor to
 * 		 allow for relative moves. Otherwise the motor is always in ABSOLUTE mode
 */
void DrivingChassis::driveForward(float mmDistanceFromCurrent, int msDuration) {
}

/**
 * Start a turn action
 *
 * This action rotates the robot around the center line made up by the contact points of the left and right wheels.
 * Positive angles should rotate to the left
 *
 * This rotation is a positive rotation about the Z axis of the robot.
 *
 * @param degreesToRotateBase the number of degrees to rotate
 * @param msDuration is the time in miliseconds that the drive action should take
 *
 *  @note this function is fast-return and should not block
 *  @note pidmotorInstance->overrideCurrentPosition(0); can be used to "zero out" the motor to
 * 		  allow for relative moves. Otherwise the motor is always in ABSOLUTE mode
 */
void DrivingChassis::turnDegrees(float degreesToRotateBase, int msDuration) {

}

/**
 * Check to see if the chassis is performing an action
 *
 * @return false is the chassis is driving, true is the chassis msDuration has elapsed
 *
 * @note this function is fast-return and should not block
 */
bool DrivingChassis::isChassisDoneDriving() {
	return false;
}
/**
 * loop()
 *
 * a fast loop function that will update states of the motors based on the information from the
 * imu.
 */
void DrivingChassis::loop(){//polls for data every 20ms

	if(trigger){
		if(IMU->getEULER_azimuth() != 0){
			offset = IMU->getEULER_azimuth() * (-1);
			trigger = false;
		}
	}
	if (!loopFlag) {
		now = millis();
		loopFlag = true;
	}
	else {
		if(now >= millis() - 20) {
			updatePose();
			loopFlag = false;
		}
	}

}


void DrivingChassis::updatePose(){
	double angleRightMotor = myright->getAngleDegrees();  //gets angle from right and left motor
	double angleLeftMotor = myleft->getAngleDegrees();
	double IMUheading =  IMU->getEULER_azimuth() + offset;  //IMU mounted in reverse, going straight will give us heading of -180 unless we add 180 to offset system by PI
	double timestamp = micros();  //set in micros, if set in millis, timestamp will be 0
	robotPose.updateEncoderPositions(timestamp, angleRightMotor, angleLeftMotor, IMUheading);  //updates encoder position -> see Pose.cpp
}

void DrivingChassis::turn(double deg, double Kp) {
	deg = deg * (PI/180);
	//WITHOUT COMPLEMENTARY FILTER
	//double headingError = this->robotPose.theta - targetHeading;  //robotPose heading - target Heading  -1 because counterclockwise is negative in our coordinate system
	//JUST IMU
	// double headingError = ((offset + this->IMU->getEULER_azimuth()) * (PI/180)) - targetHeading ;
	//WITH COMPLEMENTARY FILTER
	double headingError = (((offset + this->IMU->getEULER_azimuth()) * (PI/180)) * .98 + this->robotPose.theta * .02) - deg;

	double effort = Kp * headingError;
	if(effort > 50) {
		effort = 50;
	}
	else if (effort < -50) {
		effort = -50;
	}
	this->myleft->setVelocityDegreesPerSecond(- effort);
	this->myright->setVelocityDegreesPerSecond(- effort);
	Serial.println("+++++++++++TURNING++++++++++++");
	Serial.println("MyLeft: " + String(myleft->getVelocityDegreesPerSecond()) + " MyRight: " + String(myright->getVelocityDegreesPerSecond()) + "effort: " + String(effort) + " theta: " + String(robotPose.theta));
}

bool DrivingChassis::turnDrive(double speed, double deg, double Kp){
	this->turn(deg,25);
	if(this->myleft->getVelocityDegreesPerSecond() == 0){ //Possibly make this a range so robot gets out of turn drive function faster
		return true;
	}else{
		return false;
	}

}

void DrivingChassis::driveStraight(double speed, double targetHeading, int Kp){ // usually 25 for point turn, usually 50 for driving
	targetHeading = targetHeading * (PI/180);
	//WITHOUT COMPLEMENTARY FILTER
	//double headingError = this->robotPose.theta - targetHeading;  //robotPose heading - target Heading  -1 because counterclockwise is negative in our coordinate system
	//JUST IMU
	// double headingError = ((offset + this->IMU->getEULER_azimuth()) * (PI/180)) - targetHeading ;
	//WITH COMPLEMENTARY FILTER
	double headingError = (((offset + this->IMU->getEULER_azimuth()) * (PI/180)) * .99 + this->robotPose.theta * .01) - targetHeading;

	double effort = Kp * headingError;
	/*if(effort > 50) {
					effort = 50;
				}
				else if (effort < -50) {
					effort = -50;
				} */

	this->myleft->setVelocityDegreesPerSecond(speed - effort);
	this->myright->setVelocityDegreesPerSecond(-speed - effort);
	Serial.println("================STRAIGHT==============");
	Serial.println("MyLeft: " + String(myleft->getVelocityDegreesPerSecond()) + " MyRight: " + String(myright->getVelocityDegreesPerSecond()) + "effort: " + String(effort) + " theta: " + String(robotPose.theta));
}


void DrivingChassis::distanceDrive (double mm){
	double target = mmTOdeg(mm);
	distanceError =  abs(this->myright->getAngleDegrees()) - target;
	double effort = kpDistance * distanceError;

	this->driveStraight(-effort, 0, 1000);
}

double DrivingChassis::mmTOdeg(double mm){
	return (mm/(wheelRadius * (2*PI))) * 360;
}

/*void DrivingChassis::driveToCoordinate (int coord) {
	double target = coord * blockDistance;
	distanceError =  abs(this->myright->getAngleDegrees()) - target;
	double effort = kpDistance * distanceError;
	this->driveStraight(-effort, 0, 1000);
} */
