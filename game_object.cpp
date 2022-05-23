#include "game_object.h"
#include <system/debug_log.h>

//
// UpdateFromSimulation
// 
// Update the transform of this object from a physics rigid body
//
void GameObject::UpdateFromSimulation(const b2Body* body , gef::Vector4 Scale, float RotateX)
{
	if (body)
	{
		// setup object rotation
		gef::Matrix44 object_rotation;
		object_rotation.RotationZ(body->GetAngle());

		// setup the object translation
		gef::Vector4 object_translation(body->GetPosition().x, body->GetPosition().y, 0.0f);

		// build object transformation matrix
		
		gef::Matrix44 object_transform;//= object_rotation;

		gef::Matrix44 Sobject_transform;
		Sobject_transform.Scale(Scale);
		gef::Matrix44 Robject_transform;
		Robject_transform.RotationX(RotateX);
		object_transform = Sobject_transform * Robject_transform *object_rotation;

		
		object_transform.SetTranslation(object_translation);

		set_transform(object_transform);

		

	}
}

void GameObject::MyCollisionResponse()
{
	
	
	
	//gef::DebugOut("A collision has happened.\n");
}

Player::Player()
{
	set_type(PLAYER);
	
}

void Player::DecrementHealth()
{

	HealthValue = HealthValue - 1;
	gef::DebugOut("Player has taken damage.\n");

}

Enemy::Enemy()
{
	set_type(ENEMY);
}

void Enemy::DecrementHealth()
{
	//gef::DebugOut("Player has taken damage.\n");
}


