#include "scene_app.h"
#include "game_object.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <input/sony_controller_input_manager.h>
#include <graphics/sprite.h>
#include "load_texture.h"
#include <gef.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include "obj_mesh_loader.h"



SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	input_manager_(NULL),
	font_(NULL),
	world_(NULL),
	player_body_(NULL),
	button_icon_(NULL)
{
}

void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	InitFont();
	
	player_.HealthValue = 100;
	player_.HealthOption = 100;

	// initialise input manager
	input_manager_ = gef::InputManager::Create(platform_);

	// Initialis audio manager
	audio_manager_ = gef::AudioManager::Create();

	game_state_ = FRONTEND;
	FrontendInit();

	//game_state_ = OPTION;
	//OptionInit();

	//game_state_ = GAME;
	//GameInit();

	

	//volume
	Volumeinfo.volume = 10;

	if (audio_manager_)
	{
		//sfx
		//test_Sound = audio_manager_->LoadSample("honk.wav", platform_);
		///audio_manager_->SetSampleVoiceVolumeInfo(test_Sound, Volumeinfo);
		//audio_manager_->PlaySample(test_Sound);
		//audio_manager_->SetSamplePitch(test_Sound, 10);

		//music
		audio_manager_->SetMusicVolumeInfo(Volumeinfo);
		audio_manager_->LoadMusic("gas.wav", platform_);
		audio_manager_->PlayMusic();
	}

}

void SceneApp::CleanUp()
{
	delete input_manager_;
	input_manager_ = NULL;

	CleanUpFont();

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool SceneApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;


	input_manager_->Update();

	switch (game_state_)
	{
		case FRONTEND:
		{
			FrontendUpdate(frame_time);
		}
			break;
		case MENU:
		{
			MenuUpdate(frame_time);
		}
		break;

		case OPTION:
		{
			OptionUpdate(frame_time);
		}
			break;
		case GAME:
		{
			GameUpdate(frame_time);
		}
			break;
		default:
			return false; //If state isnt in list this exits the game somehow idk
			break;
	}

	return true;
}




void SceneApp::Render()
	{
		switch (game_state_)
		{
		case FRONTEND:
		{
			FrontendRender();
		}

		break;

		case MENU:
		{
			MenuRender();
		}

		break;

		case OPTION:
		{
			OptionRender();
		}

		break;

		case GAME:
		{
			GameRender();
		}

		break;


		}
	}

void SceneApp::InitPlayer()
{






	// setup the mesh for the player
	gef::Vector4 Player_Dimensions(0.2f, 0.2f, 0.3f); 
	
	// sets player to box shape
	//player_.set_mesh(primitive_builder_->CreateBoxMesh(Player_Dimensions));


	player_.set_position(Player_Dimensions);
	// create a physics body for the player
	

	//model loader   *note remeber to change line in renderer for texture
	OBJMeshLoader Obj_loader;
	MeshMap mesh_map;
	if (Obj_loader.Load("Models/wooden_watch_tower.obj", platform_, mesh_map))
	{
		gef::Mesh* tower_mesh = mesh_map["watchtowerHigh_watchtower"];
		if (tower_mesh)
		{
			player_.set_mesh(tower_mesh);
			
		}

	}
	else {
		std::string Result = Obj_loader.GetLastError();
		gef::DebugOut(Result.c_str());
		player_.set_mesh(primitive_builder_->GetDefaultCubeMesh());
	}
	
	/*
	//gef::Matrix44 Mesh_transform = player_.transform();
	//Mesh_transform.SetIdentity();
	//Mesh_transform.Scale(gef::Vector4(0.6f, 0.6f, 0.6f));
	//Mesh_transform.SetTranslation(gef::Vector4(4.5f, 4.5f, 0.0f)); // test line
	//Mesh_transform.RotationX(90);

	//gef::Matrix44 Mesh_transform; //= player_.transform();
	//Mesh_transform.SetIdentity();

	//gef::Matrix44 Pscale_matrix;
	//Pscale_matrix.Scale(gef::Vector4(0.2f, 0.2f, 0.2f));
	//rotation
	//gef::Matrix44 PRotateX_matrix;
	//int rotate = 90;
	//PRotateX_matrix.RotationX(gef::DegToRad(0.0f));

		//	Mesh_transform = Pscale_matrix * PRotateX_matrix;
		//	player_.set_transform(Mesh_transform);
	
	// Need to transform the mesh spesificly, this should work but dosen't?
	*/

	gef::Vector4 Pscale_matrix(0.2f,0.2f,0.2f);
	float rotate = 90;

	b2BodyDef player_body_def;
	player_body_def.type = b2_dynamicBody;
	player_body_def.position = b2Vec2(0.0f, 4.0f);
	
	player_.set_position(Player_Dimensions);

	player_body_ = world_->CreateBody(&player_body_def);

	// create the shape for the player
	b2PolygonShape player_shape;
	player_shape.SetAsBox(0.4f, 0.4f);

	// create the fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &player_shape;
	player_fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	player_body_->CreateFixture(&player_fixture_def);

	// update visuals from simulation data
	player_.UpdateFromSimulation(player_body_, Pscale_matrix, rotate);
	

	

	// create a connection between the rigid body and GameObject
	player_body_->SetUserData(&player_);
}

void SceneApp::InitEnemy() {

	gef::Vector4 Enemy_Dimensions(2.0f, 2.0f, 2.0f);

	enemys_.set_position(Enemy_Dimensions);
	gef::Vector4 Enemy_dimensions(2.0f, 2.0f, 2.0f);


	gef::Material* materials[6];

	materials[0] = new gef::Material;
	materials[0]->set_colour(0xff0000ff);
	materials[1] = new gef::Material;
	materials[1]->set_colour(0xff0000ff);
	materials[2] = new gef::Material;
	materials[2]->set_colour(0xff0000ff);
	materials[3] = new gef::Material;
	materials[3]->set_colour(0xff0000ff);
	materials[4] = new gef::Material;
	materials[4]->set_colour(0xff0000ff);
	materials[5] = new gef::Material;
	materials[5]->set_colour(0xff0000ff);

	enemys_.set_mesh(primitive_builder_->CreateBoxMesh(Enemy_dimensions, gef::Vector4(0.0f, 0.0f, 0.0f), materials));

	b2BodyDef enemy_body_def;
	enemy_body_def.type = b2_dynamicBody;
	enemy_body_def.position = b2Vec2(0.0f, 7.0f);

	enemy_body_ = world_->CreateBody(&enemy_body_def);


	b2PolygonShape enemy_shape;
	enemy_shape.SetAsBox(0.4f, 0.4f);

	// create the fixture
	b2FixtureDef enemy_fixture_def;
	enemy_fixture_def.shape = &enemy_shape;
	enemy_fixture_def.density = 0.1f;

	// create the fixture on the rigid body
	enemy_body_->CreateFixture(&enemy_fixture_def);

	gef::Vector4 Pscale_matrix(1.0f, 1.0f, 1.0f);
	float rotate = 90;

	enemys_.UpdateFromSimulation(enemy_body_, Pscale_matrix, rotate);

	enemy_body_->SetUserData(&enemys_);

}


void SceneApp::InitGround()
{
	gef::Vector4 GroundScaleNU(1.0f, 1.0f, 1.0f);
	float GroundRotateNU = 0;
	
	// ground dimensions
	gef::Vector4 ground_half_dimensions(5.0f, 0.5f, 0.5f);

	// setup the mesh for the ground
	ground_mesh_ = primitive_builder_->CreateBoxMesh(ground_half_dimensions);
	ground_.set_mesh(ground_mesh_);

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	body_def.position = b2Vec2(0.0f, 0.0f);

	ground_body_ = world_->CreateBody(&body_def);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(ground_half_dimensions.x(), ground_half_dimensions.y());

	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;

	// create the fixture on the rigid body
	ground_body_->CreateFixture(&fixture_def);

	// update visuals from simulation data
	ground_.UpdateFromSimulation(ground_body_, GroundScaleNU, GroundRotateNU);

//	wall left

	gef::Matrix44 Wall_Transform;
	Wall_Transform.SetIdentity();
	//scale
	gef::Matrix44 Wall_scale_matrix;
	Wall_scale_matrix.Scale(gef::Vector4(1.0f, 1.0f, 1.0f));
	//rotation
	gef::Matrix44 Wall_RotateX_matrix;
	Wall_RotateX_matrix.RotationX(gef::DegToRad(0.0f));

	Wall_Transform = Wall_scale_matrix * Wall_RotateX_matrix;
	Wall_Transform.SetTranslation(gef::Vector4(4.5f,4.5f, 0.0f));

	gef::Vector4 ground_half_dimensions3(0.5f, 5.0f, 0.5f);
	ground_mesh_3 = primitive_builder_->CreateBoxMesh(ground_half_dimensions3);
	ground_3.set_mesh(ground_mesh_3);
	b2BodyDef body_def3;
	body_def3.type = b2_staticBody;
	body_def3.position = b2Vec2(4.5f, 4.5f);
	ground_body_3 = world_->CreateBody(&body_def3);
	b2PolygonShape shape3;
	shape3.SetAsBox(ground_half_dimensions3.x(), ground_half_dimensions3.y());
	b2FixtureDef fixture_def3;
	fixture_def3.shape = &shape3;
	ground_body_3->CreateFixture(&fixture_def3);
	ground_3.UpdateFromSimulation(ground_body_3, GroundScaleNU, GroundRotateNU);
	ground_3.set_transform(Wall_Transform);

	gef::Matrix44 Wall_Transform2;
	Wall_Transform2.SetIdentity();
	//scale
	gef::Matrix44 Wall_scale_matrix2;
	Wall_scale_matrix2.Scale(gef::Vector4(1.0f, 1.0f, 1.0f));
	//rotation
	gef::Matrix44 Wall_RotateX_matrix2;
	Wall_RotateX_matrix2.RotationX(gef::DegToRad(0.0f));

	Wall_Transform2 = Wall_scale_matrix2 * Wall_RotateX_matrix2;
	Wall_Transform2.SetTranslation(gef::Vector4(-4.5f, 4.5f, 0.0f));

	gef::Vector4 ground_half_dimensions4(0.5f, 5.0f, 0.5f);
	ground_mesh_4 = primitive_builder_->CreateBoxMesh(ground_half_dimensions4);
	ground_4.set_mesh(ground_mesh_4);
	b2BodyDef body_def4;
	body_def4.type = b2_staticBody;
	body_def4.position = b2Vec2(-4.5f, 4.5f);
	ground_body_4 = world_->CreateBody(&body_def4);
	b2PolygonShape shape4;
	shape4.SetAsBox(ground_half_dimensions4.x(), ground_half_dimensions4.y());
	b2FixtureDef fixture_def4;
	fixture_def4.shape = &shape4;
	ground_body_4->CreateFixture(&fixture_def4);
	ground_4.UpdateFromSimulation(ground_body_4, GroundScaleNU, GroundRotateNU);
	ground_4.set_transform(Wall_Transform2);



	gef::Matrix44 Wall_Transform3;
	Wall_Transform3.SetIdentity();
	//scale
	gef::Matrix44 Wall_scale_matrix3;
	Wall_scale_matrix3.Scale(gef::Vector4(1.0f, 1.0f, 1.0f));
	//rotation
	gef::Matrix44 Wall_RotateX_matrix3;
	Wall_RotateX_matrix3.RotationX(gef::DegToRad(0.0f));

	Wall_Transform3 = Wall_scale_matrix3 * Wall_RotateX_matrix3;
	Wall_Transform3.SetTranslation(gef::Vector4(-2.5f, 8.0f, 0.0f));

	gef::Vector4 ground_half_dimensions5(2.0f, 0.5f, 0.5f);
	ground_mesh_5 = primitive_builder_->CreateBoxMesh(ground_half_dimensions5);
	ground_5.set_mesh(ground_mesh_5);
	b2BodyDef body_def5;
	body_def5.type = b2_staticBody;
	body_def5.position = b2Vec2(-2.5f, 8.0f);
	ground_body_5 = world_->CreateBody(&body_def5);
	b2PolygonShape shape5;
	shape5.SetAsBox(ground_half_dimensions5.x(), ground_half_dimensions5.y());
	b2FixtureDef fixture_def5;
	fixture_def5.shape = &shape5;
	ground_body_5->CreateFixture(&fixture_def5);
	ground_5.UpdateFromSimulation(ground_body_5, GroundScaleNU, GroundRotateNU);
	ground_5.set_transform(Wall_Transform3);


	gef::Matrix44 Wall_Transform4;
	Wall_Transform4.SetIdentity();
	//scale
	gef::Matrix44 Wall_scale_matrix4;
	Wall_scale_matrix4.Scale(gef::Vector4(1.0f, 1.0f, 1.0f));
	//rotation
	gef::Matrix44 Wall_RotateX_matrix4;
	Wall_RotateX_matrix4.RotationX(gef::DegToRad(0.0f));

	Wall_Transform4 = Wall_scale_matrix4 * Wall_RotateX_matrix4;
	Wall_Transform4.SetTranslation(gef::Vector4(2.5f, 8.0f, 0.0f));

	gef::Vector4 ground_half_dimensions6(2.0f, 0.5f, 0.5f);
	ground_mesh_6 = primitive_builder_->CreateBoxMesh(ground_half_dimensions6);
	ground_6.set_mesh(ground_mesh_6);
	b2BodyDef body_def6;
	body_def6.type = b2_staticBody;
	body_def6.position = b2Vec2(2.5f, 8.0f);
	ground_body_6 = world_->CreateBody(&body_def6);
	b2PolygonShape shape6;
	shape6.SetAsBox(ground_half_dimensions6.x(), ground_half_dimensions6.y());
	b2FixtureDef fixture_def6;
	fixture_def6.shape = &shape6;
	ground_body_6->CreateFixture(&fixture_def6);
	ground_6.UpdateFromSimulation(ground_body_6, GroundScaleNU, GroundRotateNU);
	ground_6.set_transform(Wall_Transform4);



	/// <summary>
	///  an Extra wall cube, dosent seem to spawn right now
	/// </summary>
	gef::Matrix44 Wall_Transform5;
	Wall_Transform5.SetIdentity();
	//scale
	gef::Matrix44 Wall_scale_matrix5;
	Wall_scale_matrix5.Scale(gef::Vector4(1.0f, 1.0f, 1.0f));
	//rotation
	gef::Matrix44 Wall_RotateX_matrix5;
	Wall_RotateX_matrix5.RotationX(gef::DegToRad(0.0f));

	Wall_Transform5 = Wall_scale_matrix5 * Wall_RotateX_matrix5;
	Wall_Transform5.SetTranslation(gef::Vector4(1.5f, 8.0f, 0.0f));

	gef::Vector4 ground_half_dimensions7(0.3f, 3.3f, 0.3f);
	ground_mesh_7 = primitive_builder_->CreateBoxMesh(ground_half_dimensions7);
	ground_7.set_mesh(ground_mesh_7);
	b2BodyDef body_def7;
	body_def7.type = b2_staticBody;
	body_def7.position = b2Vec2(1.5f, 8.0f);
	ground_body_7 = world_->CreateBody(&body_def7);
	b2PolygonShape shape7;
	shape7.SetAsBox(ground_half_dimensions7.x(), ground_half_dimensions7.y());
	b2FixtureDef fixture_def7;
	fixture_def7.shape = &shape7;
	ground_body_7->CreateFixture(&fixture_def7);
	ground_7.UpdateFromSimulation(ground_body_7, GroundScaleNU, GroundRotateNU);
	ground_7.set_transform(Wall_Transform5);








//floor 1
	gef::Matrix44 Floor_Transform;
		Floor_Transform.SetIdentity();
	//scale
	gef::Matrix44 Floor_scale_matrix;
		Floor_scale_matrix.Scale(gef::Vector4(1.0f,1.0f,1.0f));
	//rotation
	gef::Matrix44 Floor_RotateX_matrix;
		Floor_RotateX_matrix.RotationX(gef::DegToRad(0.0f));

	Floor_Transform = Floor_scale_matrix * Floor_RotateX_matrix;
	Floor_Transform.SetTranslation(gef::Vector4(0.0f, 4.0f, -1.0f));


	// setup the mesh for the ground
		gef::Vector4 ground_half_dimensions2(4.5f, 4.5f, 0.1f);
		ground_mesh_2 = primitive_builder_->CreateBoxMesh(ground_half_dimensions2);
		ground_2.set_mesh(ground_mesh_2);
		b2BodyDef body_def2;
		body_def2.type = b2_staticBody;
		body_def2.position = b2Vec2(0.0f, 0.0f);
		ground_body_2 = world_->CreateBody(&body_def2);
		b2PolygonShape shape2;
		shape2.SetAsBox(ground_half_dimensions2.x(), ground_half_dimensions2.y());
		b2FixtureDef fixture_def2;
		fixture_def2.shape = &shape2;
		ground_2.UpdateFromSimulation(ground_body_2, GroundScaleNU, GroundRotateNU);

	ground_2.set_transform(Floor_Transform);

	

	


}


void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
	font2_ = new gef::Font(platform_);
	font2_->Load("comic_sans");

}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void SceneApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);


	}
	if (font2_)
	{
		font2_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 490.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "HP:%.1f", player_.HealthValue);

	}

}

void SceneApp::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void SceneApp::UpdateSimulation(float frame_time)
{
	// update physics world
	float32 timeStep = 1.0f / 60.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	gef::Vector4 PlayerScaleNU(0.2f, 0.2f, 0.2f);
	float PlayerRotateNU = gef::DegToRad(90.0f);

	// update object visuals from simulation data
	player_.UpdateFromSimulation(player_body_, PlayerScaleNU, PlayerRotateNU);

	enemys_.UpdateFromSimulation(enemy_body_, PlayerScaleNU, PlayerRotateNU);

	// don't have to update the ground visuals as it is static

	// collision detection
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();

	for (int contact_num = 0; contact_num<contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{
			// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

		
			Player* player = NULL;
			Enemy* enemy = NULL;

			GameObject* gameObjectA = NULL;
			GameObject* gameObjectB = NULL;

			gameObjectA = (GameObject*)bodyA->GetUserData();
			gameObjectB = (GameObject*)bodyB->GetUserData();

			if (gameObjectA)
			{
				if (gameObjectA->type() == PLAYER)
				{
					player = (Player*)bodyA->GetUserData();
				}
				if (gameObjectA->type() == ENEMY)
				{
					enemy = (Enemy*)bodyA->GetUserData();
				}


			}

			if (gameObjectB)
			{
				if (gameObjectB->type() == PLAYER)
				{
					player = (Player*)bodyB->GetUserData();
				}
				if (gameObjectB->type() == ENEMY)
				{
					enemy = (Enemy*)bodyB->GetUserData();
				}
			}

			if (player)
			{
				if (enemy) {
					player->DecrementHealth();
				}
				
			}
		}

		// Get next contact point
		contact = contact->GetNext();
	}
}

void SceneApp::FrontendInit()
{
	button_icon_ = CreateTextureFromPNG("playstation-cross-dark-icon.png", platform_);
	MenuOption = 1;
}

void SceneApp::FrontendRelease()
{
	delete button_icon_;
	button_icon_ = NULL;
}

void SceneApp::FrontendUpdate(float frame_time)
{

	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);

	if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS){
		FrontendRelease();

		game_state_ = MENU;
		MenuInit();
	}

}

void SceneApp::FrontendRender()
{
	sprite_renderer_->Begin();

	// render "PRESS" text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width()*0.5f, platform_.height()*0.5f - 56.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"PRESS TO START");

	// Render button icon
	gef::Sprite button;
	button.set_texture(button_icon_);
	button.set_position(gef::Vector4(platform_.width()*0.5f, platform_.height()*0.5f, -0.99f));
	button.set_height(32.0f);
	button.set_width(32.0f);
	sprite_renderer_->DrawSprite(button);

	//GameRender();
	// render start area behind menu.

	DrawHUD();
	sprite_renderer_->End();
}

void SceneApp::MenuInit() {

	
	arrow_icon_ = CreateTextureFromPNG("Arrow.png", platform_);

}

void SceneApp::MenuRelease() {
	delete arrow_icon_;
	arrow_icon_ = NULL;
}

void SceneApp::MenuUpdate(float frame_time) {
	
	
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	
	if (controller)
	{
		float left_y = controller->left_stick_y_axis();
		if (OptionControl)
		{

		

		if (left_y > -0.5) {
			if (MenuOption < 3) {
				MenuOption = MenuOption + 1;
				OptionControl = false;
			}
		}


		if (left_y < 0.5) {
			if (MenuOption > 1) {
				MenuOption = MenuOption - 1;
				OptionControl = false;
			}
		}

		}

		if (left_y < 0.5 && left_y > -0.5) {
			OptionControl = true;
		}
	}

	


	if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS) {
		
		if (MenuOption == 1) {

			MenuRelease();

			game_state_ = GAME;
			GameInit();
		}

		if (MenuOption == 2) {

			MenuRelease();

			game_state_ = OPTION;
			OptionInit();
		}

		if (MenuOption == 3) {

			MenuRelease();

			game_state_ = FRONTEND;
			FrontendInit();
		}


	}

}

void SceneApp::MenuRender() {



	sprite_renderer_->Begin();

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.25f - 56.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"PRESS TO START");

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 56.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"OPTIONS");

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.75f - 56.0f, -0.99f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"EXIT");




	if (MenuOption == 1) {

		gef::Sprite arrow1;
		arrow1.set_texture(arrow_icon_);
		arrow1.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.25f, -0.99f));
		arrow1.set_height(32.0f);
		arrow1.set_width(32.0f);
		sprite_renderer_->DrawSprite(arrow1);


	}

	if (MenuOption == 2) {

		gef::Sprite arrow1;
		arrow1.set_texture(arrow_icon_);
		arrow1.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f, -0.99f));
		arrow1.set_height(32.0f);
		arrow1.set_width(32.0f);
		sprite_renderer_->DrawSprite(arrow1);


	}

	if (MenuOption == 3) {

		gef::Sprite arrow1;
		arrow1.set_texture(arrow_icon_);
		arrow1.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.75f, -0.99f));
		arrow1.set_height(32.0f);
		arrow1.set_width(32.0f);
		sprite_renderer_->DrawSprite(arrow1);


	}



	sprite_renderer_->End();
}


void SceneApp::OptionInit() {
	arrow_icon_2 = CreateTextureFromPNG("Arrow.png", platform_);
	MenuOption2 = 1;
}

void SceneApp::OptionRelease() {
	delete arrow_icon_2;
	arrow_icon_2 = NULL;

}

void SceneApp::OptionUpdate(float frame_time) {

	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);


	if (!ChosenOption) {

		if (controller)
		{
			float left_y = controller->left_stick_y_axis();
			if (OptionControl2)
			{



				if (left_y > -0.5) {
					if (MenuOption2 < 3) {
						MenuOption2 = MenuOption2 + 1;
						OptionControl2 = false;
					}
				}


				if (left_y < 0.5) {
					if (MenuOption2 > 1) {
						MenuOption2 = MenuOption2 - 1;
						OptionControl2 = false;
					}
				}

			}

			if (left_y < 0.5 && left_y > -0.5) {
				OptionControl2 = true;
			}
		}




		if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS) {

			if (MenuOption2 == 1) {

				//MenuRelease();

				//game_state_ = GAME;
				//GameInit();
				ChosenOption = true;
			}

			if (MenuOption2 == 2) {

				//MenuRelease();

				//game_state_ = OPTION;
				//FrontendInit();

				ChosenOption = true;
			}

			if (MenuOption2 == 3) {

				OptionRelease();

				game_state_ = MENU;
				MenuInit();
			}


		}

	}

	if (ChosenOption) {

		float left_x = controller->left_stick_x_axis();

		if (MenuOption2 == 1) {

			if (left_x < -0.5)
			{

				if (player_.HealthValue > 0)
				{
					player_.HealthValue = player_.HealthValue - 1;

				}
			}
			if (left_x > 0.5)
			{

				if (player_.HealthValue < 999)
				{
					player_.HealthValue = player_.HealthValue + 1;
				}

			}

			if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE) {
				ChosenOption = false;
			}
			// 
			// 
			//MenuRelease();

			//game_state_ = GAME;
			//GameInit();
		}

		if (MenuOption2 == 2) {


			if (left_x < -0.5)
			{

				if (Volumeinfo.volume > 0)
				{
					Volumeinfo.volume = Volumeinfo.volume - 1;
					audio_manager_->SetMusicVolumeInfo(Volumeinfo);

				}
			}
			if (left_x > 0.5)
			{

				if (Volumeinfo.volume < 50)
				{
					Volumeinfo.volume = Volumeinfo.volume + 1;
					audio_manager_->SetMusicVolumeInfo(Volumeinfo);
				}

				//MenuRelease();
							//volumeInfo.volume
				//game_state_ = OPTION;
				//FrontendInit();
			}

			if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE) {
				ChosenOption = false;
			}


		}

	}
}




void SceneApp::OptionRender() {

sprite_renderer_->Begin();

font_->RenderText(
	sprite_renderer_,
	gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.25f - 56.0f, -0.99f),
	1.0f,
	0xffffffff,
	gef::TJ_CENTRE,
	"HEALTH");

font_->RenderText(
	sprite_renderer_,
	gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.35f - 56.0f, -0.99f),
	1.0f,
	0xffffffff,
	gef::TJ_CENTRE,
	"HP:%.1f", player_.HealthValue);


font_->RenderText(
	sprite_renderer_,
	gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.5f - 56.0f, -0.99f),
	1.0f,
	0xffffffff,
	gef::TJ_CENTRE,
	"VOLUME");

font_->RenderText(
	sprite_renderer_,
	gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.6f - 56.0f, -0.99f),
	1.0f,
	0xffffffff,
	gef::TJ_CENTRE,
	"VL:%.1f", Volumeinfo.volume);


font_->RenderText(
	sprite_renderer_,
	gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.75f - 56.0f, -0.99f),
	1.0f,
	0xffffffff,
	gef::TJ_CENTRE,
	"RETURN");




if (MenuOption2 == 1) {

	gef::Sprite arrow1;
	arrow1.set_texture(arrow_icon_2);
	arrow1.set_position(gef::Vector4(platform_.width() * 0.4f, platform_.height() * 0.25f, -0.99f));
	arrow1.set_height(32.0f);
	arrow1.set_width(32.0f);
	sprite_renderer_->DrawSprite(arrow1);


}

if (MenuOption2 == 2) {

	gef::Sprite arrow1;
	arrow1.set_texture(arrow_icon_2);
	arrow1.set_position(gef::Vector4(platform_.width() * 0.4f, platform_.height() * 0.5f, -0.99f));
	arrow1.set_height(32.0f);
	arrow1.set_width(32.0f);
	sprite_renderer_->DrawSprite(arrow1);


}

if (MenuOption2 == 3) {

	gef::Sprite arrow1;
	arrow1.set_texture(arrow_icon_2);
	arrow1.set_position(gef::Vector4(platform_.width() * 0.5f, platform_.height() * 0.75f, -0.99f));
	arrow1.set_height(32.0f);
	arrow1.set_width(32.0f);
	sprite_renderer_->DrawSprite(arrow1);


}
	

sprite_renderer_->End();

}


void SceneApp::GameInit()
{
	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);


	SetupLights();

	// initialise the physics world
	b2Vec2 gravity(0.0f, 0.0f);
	world_ = new b2World(gravity);

	InitPlayer();
	InitGround();
	InitEnemy();
}

void SceneApp::GameRelease()
{
	// destroying the physics world also destroys all the objects within it
	delete world_;
	world_ = NULL;

	delete ground_mesh_;
	ground_mesh_ = NULL;

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	//delete wood_texture_;
//	wood_texture_ = NULL;
	//delete brick_texture_;
	//brick_texture_ = NULL;
}

void SceneApp::GameUpdate(float frame_time)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);



	UpdateSimulation(frame_time);

	if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS) {

		GameRelease();

		game_state_ = FRONTEND;
		FrontendInit();
	}

	if (controller)
	{//gef::DebugOut("y: %f\n", left_y);
//gef::DebugOut("x: %f\n", left_x);
	float left_x = controller->left_stick_x_axis();
	float left_y = controller->left_stick_y_axis();
	
	  player_body_->SetLinearVelocity(b2Vec2(left_x * 2, -left_y * 2));
	//gef::Vector4 Player_position = player_.position();
	///Player_position.set_x(Player_position.x() + left_x);
	//Player_position.set_y(Player_position.y() + left_y);
	//player_.set_position(Player_position.x(), Player_position.y(), Player_position.z());

	}

	gef::Vector4 Player_position = player_.position();
	gef::Vector4 Enemy_position = enemys_.position();

	float Speed = 0.5f;
	float EnemyVelocityX = 0.0f;
	float EnemyVelocityY = 0.0f;

	if (player_body_->GetPosition().x > enemy_body_->GetPosition().x)
	{

		EnemyVelocityX = 1.0f;

		//if (player_body_->GetPosition().x > enemy_body_->GetPosition().x + 2)
		//{
		//	EnemyVelocityX = 1.0f;
		//}

	}
	else
	{

		EnemyVelocityX = -1.0f;

		//if (player_body_->GetPosition().x < enemy_body_->GetPosition().x - 2)
		//{
		//	EnemyVelocityX = -1.0f;
		//}

	}

	if (player_body_->GetPosition().y > enemy_body_->GetPosition().y)
	{
		EnemyVelocityY = 1.0f;

		//if (player_body_->GetPosition().y > enemy_body_->GetPosition().y + 2)
		//{
		//	EnemyVelocityY = 1.0f;
		//}

	}
	else
	{
		EnemyVelocityY = -1.0f;

		//if (player_body_->GetPosition().y < enemy_body_->GetPosition().y - 2)
		//{
		//	EnemyVelocityY = -1.0f;
		//}
	}

	enemy_body_->SetLinearVelocity(b2Vec2(EnemyVelocityX * Speed, EnemyVelocityY * Speed));


	if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE) {
		// shoot sphere at obstacles

	}

	if (player_.HealthValue = 0) {



	}

}

void SceneApp::GameRender()
{
	gef::DebugOut("player: %f\n", player_.position());

	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 1.5f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// view
	gef::Vector4 camera_eye(player_body_->GetPosition().x, player_body_->GetPosition().y, 10.0f);
	gef::Vector4 camera_lookat(player_body_->GetPosition().x, player_body_->GetPosition().y, 0.0f);
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);


	// draw 3d geometry
	renderer_3d_->Begin();

	// draw ground
	renderer_3d_->DrawMesh(ground_);
	renderer_3d_->DrawMesh(ground_2);
	renderer_3d_->DrawMesh(ground_3);
	renderer_3d_->DrawMesh(ground_4);
	renderer_3d_->DrawMesh(ground_5);
	renderer_3d_->DrawMesh(ground_6);
	renderer_3d_->DrawMesh(ground_7);
	// draw player
	//renderer_3d_->set_override_material(&primitive_builder_->red_material());
	renderer_3d_->DrawMesh(player_);
	renderer_3d_->DrawMesh(enemys_);
	renderer_3d_->set_override_material(NULL);

	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}

