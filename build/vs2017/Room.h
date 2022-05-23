#ifndef _ROOM_H
#define _ROOM_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <input/input_manager.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "Room.h"


class Room
{
public:
	Room();
	~Room();
	void Update();
	void Render();

private:

};

#endif