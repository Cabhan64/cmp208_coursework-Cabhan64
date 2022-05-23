//keyboard controls for testing

#include "Input.h"

void Input::SetKeyDown(unsigned char key)
{
	keys[key] = true;
}

void Input::SetKeyUp(unsigned char key)
{
	keys[key] = false;
}

bool Input::isKeyDown(int key)
{
	return keys[key];
}

void Input::setMouseX(int pos)
{
	mouse.x = pos;
}

void Input::setMouseY(int pos)
{
	mouse.y = pos;
}

void Input::setMousePos(int ix, int iy)
{
	mouse.x = ix;
	mouse.y = iy;
}

int Input::getMouseX()
{
	return mouse.x;
}

int Input:: getMouseY()
{
	return mouse.y;
}

void Input::setLeftMouseButton(bool b)
{
	mouse.left = b;
}

bool Input::isLeftMouseButtonPressed()
{
	return mouse.left;
}

