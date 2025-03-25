#include "camera.h"



void camera::MoveForward(float speed) {
	mEye.z -= speed;
}
void camera::MoveBackward(float speed) {
	mEye.z += speed;
}
void camera::MoveLatLeft(float speed) {

}
void camera::MoveLatRigth(float speed) {

}