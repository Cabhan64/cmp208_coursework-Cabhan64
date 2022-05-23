#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <input/input_manager.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "Room.h"
#include <audio/audio_manager.h>

enum GAMESTATE
{
	FRONTEND,
	MENU,
	GAME,
	OPTION,
	ENDLOSS,
	
};
// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class AudioManager;
	class Renderer3D;
}

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitPlayer();
	void InitGround();
	void InitEnemy();
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void UpdateSimulation(float frame_time);
    
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Font* font2_;
	gef::InputManager* input_manager_;
	gef::AudioManager* audio_manager_;

	GAMESTATE game_state_;

	//
	// FRONTEND DECLARATIONS
	//
	gef::Texture* button_icon_;


	//
	// MENU DECLARATIONS
	//
	int MenuOption;
	bool OptionControl = true;
	gef::Texture* arrow_icon_;

	int MenuOption2;
	bool OptionControl2 = true;
	gef::Texture* arrow_icon_2;

	bool ChosenOption = false;

	float timer;
	float FULLtimer;
	//
	// GAME DECLARATIONS
	//
	gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	gef::Texture* wood_texture_;
	gef::Texture* brick_texture_;
	// create the physics world
	b2World* world_;

	// player variables
	//gef::Mesh* player_mesh_;
	Player player_;
	b2Body* player_body_;

	Enemy enemys_;
	b2Body* enemy_body_;


	// ground variables
	gef::Mesh* ground_mesh_;
	GameObject ground_;
	b2Body* ground_body_;
	gef::Mesh* ground_mesh_2;
	GameObject ground_2;
	b2Body* ground_body_2;
	gef::Mesh* ground_mesh_3;
	GameObject ground_3;
	b2Body* ground_body_3;
	gef::Mesh* ground_mesh_4;
	GameObject ground_4;
	b2Body* ground_body_4;

	gef::Mesh* ground_mesh_5;
	GameObject ground_5;
	b2Body* ground_body_5;

	gef::Mesh* ground_mesh_6;
	GameObject ground_6;
	b2Body* ground_body_6;

	gef::Mesh* ground_mesh_7;
	GameObject ground_7;
	b2Body* ground_body_7;







	// Sfx
	int test_Sound;
	gef::VolumeInfo Volumeinfo;

	float fps_;

	void FrontendInit();
	void FrontendRelease();
	void FrontendUpdate(float frame_time);
	void FrontendRender();

	void MenuInit();
	void MenuRelease();
	void MenuUpdate(float frame_time);
	void MenuRender();

	void OptionInit();
	void OptionRelease();
	void OptionUpdate(float frame_time);
	void OptionRender();

	void GameInit();
	void GameRelease();
	void GameUpdate(float frame_time);
	void GameRender();

	void EndInit();
	void EndRelease();
	void EndUpdate(float frame_time);
	void EndRender();



};

#endif // _SCENE_APP_H
