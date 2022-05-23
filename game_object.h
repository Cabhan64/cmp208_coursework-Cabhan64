#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include <gef.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include "primitive_builder.h"


//using namespace gef;

	enum OBJECT_TYPE
	{
		PLAYER,
		ENEMY,
		BULLET
	};

	class GameObject : public gef::MeshInstance
	{
	public:
		void UpdateFromSimulation(const b2Body* body , gef::Vector4 Scale, float RotateX);
		void MyCollisionResponse();

		inline void set_type(OBJECT_TYPE type) { type_ = type; }
		inline OBJECT_TYPE type() { return type_; }
	private:
		OBJECT_TYPE type_;
	};

	class Player : public GameObject
	{
	public:
		Player();
		void DecrementHealth();
		float HealthValue;
		float HealthOption;

		inline void set_position(const gef::Vector4& position) { position_ = position; }
		inline void set_position(const float x, const float y, const float z) { position_ = gef::Vector4(x, y, z); }
		inline const gef::Vector4& position() const { return position_; }


	protected:
		gef::Vector4 position_;
		//float width_;
	//	float height_;
		//UInt32 colour_;
		//float rotation_;

	//	gef::Vector2 uv_position_;
	//	float uv_width_;
	//	float uv_height_;
	//	const class Texture* texture_;
	};

	class Enemy : public GameObject
	{
	public:
		Enemy();
		void DecrementHealth();
		inline void set_position(const gef::Vector4& position) { position_ = position; }
		inline void set_position(const float x, const float y, const float z) { position_ = gef::Vector4(x, y, z); }
		inline const gef::Vector4& position() const { return position_; }
	protected:
		gef::Vector4 position_;
		//float width_;
	//	float height_;
		//UInt32 colour_;
		//float rotation_;

	//	gef::Vector2 uv_position_;
	//	float uv_width_;
	//	float uv_height_;
	//	const class Texture* texture_;
	};


#endif // _GAME_OBJECT_H