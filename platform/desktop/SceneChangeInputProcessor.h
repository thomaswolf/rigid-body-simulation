/*
 * Create different Scenes (hardcoded)
 */

#pragma once

#include <string>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "Model.h"
#include "Scene.h"
#include "SceneLoader.h"
#include "PictureLoader.h"
#include "SpawnPoint.h"
#include "constraint/DistanceConstraint.h"
#include "constraint/BodyDistanceConstraint.h"
#include "constraint/SpringConstraint.h"

class SceneChangeInputProcessor : public InputProcessor
{

	bool change = false;
	int key;

public:


	SceneChangeInputProcessor(Scene* scene, CameraInputProcessor* cameraInput)
	{
		this->scene = scene;
		this->cameraInput = cameraInput;

		// add 1st scene
		//createFinalScene();
		//loadBallOverBallScene();
		createContactScene();
	}

	void OnKeyDown(int key) 
	{
		this->key = key;
		change = true;
	}

	void OnKeyInput(const bool* keys, GLfloat dt)
	{
		if (!change) return;
		change = false;

/*
		if (key == GLFW_KEY_1)		createDominoElevatorScene();
		//~ if (key == GLFW_KEY_1)		createFinalDomino();
		if (key == GLFW_KEY_2)		createPictureScene();
		//~ if (key == GLFW_KEY_2)		createRopeScene();
		if (key == GLFW_KEY_3)		createHalfpipeTestScene();
		//if (key == GLFW_KEY_4)		createDominoScene();
		if (key == GLFW_KEY_4)		loadDominoScene();
		//if (key == GLFW_KEY_5)		createComplicatedScene();
		if (key == GLFW_KEY_5)		createStressTest();
		if (key == GLFW_KEY_6)		createTowerScene();
		//~ if (key == GLFW_KEY_6)		loadMatterhornScene();
		//if (key == GLFW_KEY_7)		createCatapultScene();
		//if (key == GLFW_KEY_7)		createTrampolinScene();
		if (key == GLFW_KEY_7)		createWheelScene();
		if (key == GLFW_KEY_8)		createTowerPendelScene();
		//if (key == GLFW_KEY_9)		loadDestructionScene();
		//if (key == GLFW_KEY_9)		loadScene(std::string("looping"));
		//~ if (key == GLFW_KEY_0)		loadMasterScene();
		if (key == GLFW_KEY_0)		loadCarScene();
		//~ if (key == GLFW_KEY_1)		createKAPLAScene();
*/		
		//if (key == GLFW_KEY_9)		createTowerStabilityScene();
		

		// Scenes for presentation

		if (key == GLFW_KEY_1)		createContactScene();
		if (key == GLFW_KEY_2)		createBallJointScene();
		if (key == GLFW_KEY_3)		createHingeJointScene();
		if (key == GLFW_KEY_4)		createDistanceScene();
		if (key == GLFW_KEY_5)		createKAPLAScene();		// might be too computational intensive
		if (key == GLFW_KEY_6)		createPictureScene();	// scene with cgl logo
		if (key == GLFW_KEY_7)		createFinalScene();
	

		//if (key == GLFW_KEY_4)		loadDominoScene();
		//if (key == GLFW_KEY_6)		loadDestructionScene();
		//if (key == GLFW_KEY_6)		loadBallOverBallScene();
		
		//if (key == GLFW_KEY_9)		createTowerScene();
		//if (key == GLFW_KEY_8)		loadLoopingScene();
		if (key == GLFW_KEY_8)		createStressTest();
		//if (key == GLFW_KEY_0)		createStressTest();
		if (key == GLFW_KEY_9)		createComplicatedScene();
		//if (key == GLFW_KEY_0)		createWheelScene();
		//if (key == GLFW_KEY_8)		loadScene("dominobridge");

		//~ if (key == GLFW_KEY_9)		loadScene(std::string("balloverball"));
		if (key == GLFW_KEY_0)		loadLoopingScene();


		
		scene->AddEntities();
	}


private:
	
	Scene* scene;
	CameraInputProcessor* cameraInput;


	void createFinalScene()
	{
		scene->Clear();

		// looping & car track
		SceneLoader loader(scene, true);
		loader.LoadObj("complete_scene");
		scene->GetPhysicManager()->SetSpeedup(4);

		// add balls
		int n = 1;
		float interval = 1;

		SpawnPoint* sp = new SpawnPoint(scene, vec3(-15.7,13.6,-1), interval, n, [&](int i) {	

			vec3 color(1,0,0);
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(color, 12), vec3(-5,12,-1));
			ball->SetScale(vec3(0.25));
			ball->SetRotation(quat(0,0,0,1));
			//ball->GetRigidBody()->SetRestitution(0.5);
			ball->GetRigidBody()->SetRestitution(0.1);
			//~ ball->GetRigidBody()->SetRestitution(0.5);
			//~ ball->GetRigidBody()->SetFriction(0.1);
			return ball;
		});
		scene->AddEntity(sp);

		// create wheel
		/*Entity* wheel = new Entity(vec3(0,0,0));
		createWheel(wheel);
		wheel->SetPosition(vec3(-40,0, 29.8));
		wheel->SetRotation(radians(vec3(0,180,0)));
		wheel->OnParentChanged();
		scene->AddEntityAndFlattenHierachy(wheel);*/

		// ball over ball 
		Entity* ballOverBall = new Entity(vec3(0,0,0));
		createBallOverBall(ballOverBall);
		ballOverBall->SetPosition(vec3(-40,0.25,17));
		ballOverBall->SetRotation(radians(vec3(0,0,0)));
		ballOverBall->OnParentChanged();
		scene->AddEntityAndFlattenHierachy(ballOverBall);
		

		// elevator
		Entity* dominoElevator = new Entity(vec3(0,0,0));
		createDominoElevator(dominoElevator);
		dominoElevator->SetPosition(vec3(-23.9,0,-1));
		dominoElevator->SetRotation(radians(vec3(0,0,0)));
		dominoElevator->OnParentChanged();
		scene->AddEntityAndFlattenHierachy(dominoElevator);

		// domino
		Entity* dominoPicture = new Entity(vec3(12.5,0,0));
		GetFinalDomino(dominoPicture);
		dominoPicture->SetRotation(radians(vec3(0,0,0)));
		dominoPicture->OnParentChanged();
		scene->AddEntityAndFlattenHierachy(dominoPicture);

		// add floor
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(-10,0,10));
		plane->SetScale(vec3(50,1,30));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add car with entity
		Entity* car = new Entity(vec3(0,0,0));
		GetCar(car);
		car->SetRotation(radians(vec3(0,0,0)));
		car->SetPosition(vec3(3.0,0.5,0));
		car->OnParentChanged();
		scene->AddEntityAndFlattenHierachy(car);

		// add camera
		Camera* camera = new Camera(vec3(-40,4,30));
		scene->SetCamera(camera);
		cameraInput->SetCamera(scene->GetCamera());

		// add light
		Light* light = new Light(vec3(-19,25,9), vec3(-5,0,0), 50, 5, 50);
		scene->SetLight(light);

		scene->AddEntities();
		std::cout << scene->GetPhysicManager()->CountBodies() << " bodies" << std::endl;
	}

	void createWheelScene()
	{
		scene->Clear();

		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10,10,10));
		scene->AddEntity(plane);

		// add car with entity
		Entity* car = new Entity(vec3(0,0,0));
		createWheel(car);
		car->SetRotation(radians(vec3(0,0,0)));
		car->SetPosition(vec3(0,0,0));
		car->OnParentChanged();
		scene->AddEntity(car);

		// add floor
		plane->SetScale(vec3(50,1,30));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add camera
		Camera* camera = new Camera(vec3(0,4,5));
		scene->SetCamera(camera);
		cameraInput->SetCamera(scene->GetCamera());

		// add light
		Light* light = new Light(vec3(-19,25,9), vec3(-5,0,0), 50, 5, 50);
		scene->SetLight(light);

		scene->AddEntities();
		std::cout << scene->GetPhysicManager()->CountBodies() << " bodies" << std::endl;
	}

	/*
	 * waterwheel
	 */
	void createWheel(Entity* entity)
	{
		vec3 red(0.6,0,0);
		vec3 blue(0,0,0.6);
		vec3 yellow(0,0.6,0);
		std::mt19937 gen(13);
		std::uniform_real_distribution<double> dist(0,1);
		
		// dominos
		float L = 1.;
		float B = .5;
		float H = .2;


		// midpoint of polygon
		vec3 position(0,3,0);
		RigidBodyModel* box;
		box = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position + vec3(0,0,0.5));
		box->SetScale(vec3(0.1,0.1,0.03));
		box->SetRotation(radians(vec3(0,0,0)));
		box->SetStatic();
		entity->AddChild(box);
		// polygon
		RigidBodyModel* polygon;
		polygon = new RigidBodyModel(MeshGenerator::CreateCylinder(blue, 2), position );
		polygon->SetScale(vec3(0.6,1,0.6));
		polygon->SetRotation(radians(vec3(90,0,0)));
		entity->AddChild(polygon);
		HingeConstraint* dst1 = new HingeConstraint(box->GetRigidBody(), polygon->GetRigidBody(), vec3(0,0,0.3), position);
		scene->GetPhysicManager()->AddConstraint(dst1);
		// sticks around polygon
		double L_ = -1.2;
		for (int i=0; i<6; i++) {
			vec3 radius(L_*cos(i*M_PI/3 + M_PI/12),L_*sin(i*M_PI/3 + M_PI/12),0);
			RigidBodyModel* stick = new RigidBodyModel(MeshGenerator::CreateCylinder(yellow), position + radius);
			stick->SetScale(vec3(0.1,1,0.1));
			stick->SetRotation(radians(vec3(90,0,0)));
			entity->AddChild(stick);
			vec3 axisPos = position + radius;
			vec3 axis(0,0,1);
			HingeConstraint* stick_dst = new HingeConstraint(polygon->GetRigidBody(), stick->GetRigidBody(), axis, axisPos);
			scene->GetPhysicManager()->AddConstraint(stick_dst);
		}

		// Blockade
		vec3 distance(0,-1,0);
		RigidBodyModel* blockade = new RigidBodyModel(MeshGenerator::CreateBox(), position + distance);
		blockade->SetScale(vec3(0.4,0.5,1.1));
		entity->AddChild(blockade);
		vec3 axisPos = position + distance/2.f;
		vec3 axis(0,0,1);
		HingeConstraint* dst = new HingeConstraint(polygon->GetRigidBody(), blockade->GetRigidBody(), axis, axisPos);
		scene->GetPhysicManager()->AddConstraint(dst);

		// lane above polygon, with balls
		Entity* lane = Combos::Lane();
		lane->SetScale(vec3(1.2,1,4));
		lane->SetPosition(vec3(-3.5,1.3,0));
		lane->SetRotation(radians(vec3(45,90,0)));
		lane->OnParentChanged(); // important for rigidbodies because they are already handled by the physicmanager
		entity->AddChild(lane);
		for (int i=0; i<1; i++) {
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(blue,10), distance + vec3(-4,6.2,0));
			ball->SetScale(vec3(0.5));
			entity->AddChild(ball);
		}
		// plateau + start domino
		RigidBodyModel* box2;
		box2 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), distance + vec3(-6.6,5.62,0) );
		box2->SetScale(vec3(6,0.1,1.2));
		box2->SetStatic();
		entity->AddChild(box2);
		for (int i=0; i<8; i++) {
			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1)), distance + vec3(-5-0.5*i,6.2,0));
			stone->SetScale(vec3(H,L,B));
			entity->AddChild(stone);
		}

		// lane beyond polygon, with balls
		vec3 pos_lane2(0,1.4,-2.8);
		Entity* lane2 = Combos::Lane();
		lane2->SetScale(vec3(0.2,1,4));
		lane2->SetPosition(pos_lane2 - vec3(0,0.4,0.8));
		lane2->SetRotation(radians(vec3(30,0,0)));
		lane2->OnParentChanged(); // important for rigidbodies because they are already handled by the physicmanager
		entity->AddChild(lane2);
		// short horizontal lane
		Entity* lane3 = Combos::Lane();
		lane3->SetScale(vec3(0.2,1,0.4));
		lane3->SetPosition(pos_lane2 + vec3(0,-1.7,2.05));
		lane3->SetRotation(radians(vec3(0,0,0)));
		lane3->OnParentChanged(); // important for rigidbodies because they are already handled by the physicmanager
		entity->AddChild(lane3);
		// balls
		for (int i=0; i<6; i++) {
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(red,20), pos_lane2 + vec3(0, 2.5-0.1*i, i*0.3-1));
			ball->SetScale(vec3(0.1));
			entity->AddChild(ball);
		}
		RigidBodyModel* ramp = new RigidBodyModel(MeshGenerator::CreateBox(), pos_lane2 + vec3(0,-1,+2.8));
		ramp->SetRotation(radians(vec3(20,0,0)));
		ramp->SetScale(vec3(0.5,0.1,2));
		ramp->SetStatic();
		entity->AddChild(ramp);
		// growing Domino
		for (int i=0; i<7; i++) {
			double scaling = (double)(i+3)/10;
			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1)), vec3(0,0.15+(i*0.05),1+(0.12+i*0.03)*i));
			stone->SetScale(vec3(scaling*H,scaling*L,scaling*B));
			stone->SetRotation(radians(vec3(0,90,0)));
			entity->AddChild(stone);
		}
		// end domino
		for (int i=0; i<5; i++) {
			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1)), vec3(0,0.5,3.5+0.5*i));
			stone->SetScale(vec3(H,L,B));
			stone->SetRotation(radians(vec3(0,90,0)));
			entity->AddChild(stone);
		}


		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		entity->AddChild(plane);
		plane->SetStatic();
	}



	void loadTestScene()
	{
		SceneLoader loader(scene);
		loader.LoadObj(std::string("vertikal"));

		// add light
		Light* light = new Light(vec3(-4.0f, 8.0f, 15.0f), vec3(-4.0f, 2.0f, 0.f));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(-4.0f, 8.0f, 12.0f), vec3(0.0f,1.0f,0.0f),-90.0f, 0.0f);
		scene->SetCamera(camera);

		// add balls
		
		int n = 50;
		float interval = 1.8;

		//SpawnPoint* sp = new SpawnPoint(scene, vec3(-4.084,11,-1), 0.9, n, spawnFunc);
		SpawnPoint* sp = new SpawnPoint(scene, vec3(-4.784,11,-1), interval, n, [&](int i) {	

			vec3 color(0,0,0.6);
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(color, 10), vec3(-4,12,-1));
			ball->SetScale(vec3(0.12));
			ball->GetRigidBody()->SetRestitution(0.5);
			ball->GetRigidBody()->SetFriction(0.1);
			return ball;
		});
		scene->AddEntity(sp);

		SpawnPoint* sp2 = new SpawnPoint(scene, vec3(-3.284,11,-1), interval, n, [&](int i) {	

			vec3 color(0,0,0.6);
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(color, 10), vec3(-4,12,-1));
			ball->SetScale(vec3(0.12));
			ball->SetRotation(vec3(0,0,0));
			ball->GetRigidBody()->SetRestitution(0.5);
			ball->GetRigidBody()->SetFriction(0.1);
			return ball;
		});
		scene->AddEntity(sp2);


		cameraInput->SetCamera(scene->GetCamera());
	}
	void loadLoopingScene()
	{
		SceneLoader loader(scene);
		loader.LoadObj(std::string("looping_thomaset"));

		// add light
		Light* light = new Light(vec3(-4.0f, 8.0f, 15.0f), vec3(-4.0f, 2.0f, 0.f));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(-4.0f, 8.0f, 12.0f), vec3(0.0f,1.0f,0.0f),-90.0f, 0.0f);
		scene->SetCamera(camera);

		// add balls
		
		int n = 1;
		float interval = 1;

		SpawnPoint* sp = new SpawnPoint(scene, vec3(-13.7,13.6,-1), interval, n, [&](int i) {	

			vec3 color(0,0,0.6);
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(color, 10), vec3(-4,12,-1));
			ball->SetScale(vec3(0.25));
			ball->SetRotation(quat(0,0,0,1));
			//~ ball->GetRigidBody()->SetRestitution(0.5);
			//~ ball->GetRigidBody()->SetFriction(0.1);
			return ball;
		});
		scene->AddEntity(sp);
		
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(50));
		scene->AddEntity(plane);
		plane->SetStatic();
		
		
		// add domino with entity
		Entity* dominoPicture = new Entity(vec3(2.5,0,0));
		GetFinalDomino(dominoPicture);
		dominoPicture->SetRotation(radians(vec3(0,0,0)));
		dominoPicture->OnParentChanged();
		scene->AddEntity(dominoPicture);

		cameraInput->SetCamera(scene->GetCamera());
	}
	
	
	void createBallOverBall(Entity* parent)
	{
		//SceneLoader loader(scene);
		//loader.LoadObj(std::string("ball_over_ball_thomaset"));

		// add balls
		vec3 color(1,0,0);
		RigidBodyModel* ball1 = new RigidBodyModel(MeshGenerator::CreateSphere(color, 10), vec3(1,3,-17.4));
		ball1->SetScale(vec3(0.25));
		parent->AddChild(ball1);
		RigidBodyModel* ball2 = new RigidBodyModel(MeshGenerator::CreateSphere(color, 10), vec3(-1,6,-14.7));
		ball2->SetScale(vec3(0.25));
		parent->AddChild(ball2);
		// add a lot of dominos
		// domino dimensions
		float L = 1.;
		float B = .5;
		float H = .2;
		float epsilon = 0.01;
		// distance of dominos 
		float distX = 0.6*L-H;
		float distY = 0.1*B;
		color = vec3(0,0.3,0.7);
		// stair
		vec3 stairPos(-1,3.5,-5.7);
		float dZ = 0.25;
		float dY = -0.6;
		for(int i = 0; i < 11; ++i)
		{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(color), stairPos + vec3(0, i*dZ, i*dY));
				stone->SetScale(vec3(B,L,H));
				parent->AddChild(stone);	
		}
		// upstairs domino
		vec3 upStairsPos(-1,6.25, -12.2);
		for(int i = 0; i < 5; ++i)
		{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(color), upStairsPos + vec3(0, 0, -i*distX));
				stone->SetScale(vec3(B,L,H));
				parent->AddChild(stone);	
		}
		// downstairs domino
		vec3 downStairsPos(1,3.25, -5.7);
		for(int i = 0; i < 27; ++i)
		{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(color), downStairsPos + vec3(0, 0, -i*distX));
				stone->SetScale(vec3(B,L,H));
				parent->AddChild(stone);	
		}
		// fan
		vec3 fanStart = vec3(0,3.25,-2.5);
		for(int i = 1; i < 6; ++i)
		{
			float startX = fanStart[0] + (i*(B+distY) - distY)/2 - B/2;
			float startY = fanStart[2] - (distX+H)*(i-1) - H; 
			for(int j = 0; j < i; ++j)
			{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(color), vec3(startX - j*(distY+B), fanStart[1], startY ));
				stone->SetScale(vec3(B,L,H));
				parent->AddChild(stone);
			};
		}
		// dominos before fan
		vec3 startPos(0,3.25,-H);
		for(int i = 0; i < 6; ++i)
		{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(color), startPos + vec3(0, 0, -i*distX));
				stone->SetScale(vec3(B,L,H));
				// trigger the domino for test reasons, only works with rotation of entity = 0
				if(i == 0)
				{
					//stone->GetRigidBody()->ApplyAngularMomentum(dvec3(-0.26,0,0));
				}
				parent->AddChild(stone);
		}
	}
	void createBunny(Entity* parent)
	{
		SceneLoader loader(scene);
		loader.LoadObj(std::string("bunny"));
	}
	
	
	
	void loadBallOverBallScene()
	{
		scene->Clear();
		// load entity
		Entity* ballOverBall = new Entity(vec3(0,0,0));
		createBallOverBall(ballOverBall);
		ballOverBall->SetPosition(vec3(0,0,0));
		ballOverBall->SetRotation(radians(vec3(0,0,0)));
		ballOverBall->OnParentChanged();

		scene->AddEntityAndFlattenHierachy(ballOverBall);
		
		// add plane
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(50));
		scene->AddEntity(plane);
		plane->SetStatic();
		
		// add light
		Light* light = new Light(vec3(-4.0f, 8.0f, 15.0f), vec3(-4.0f, 2.0f, 0.f));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0, 5.f, 0), vec3(0.0f,1.0f,0.0f),-90.0f, 0.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}

	void loadDominoScene()
	{
		SceneLoader loader(scene);
		loader.LoadObj(std::string("domino"));

		// add light
		Light* light = new Light(vec3(-4.0f, 12.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(-4.0f, 5.0f, 12.0f), vec3(0.0f,1.0f,0.0f),-90.0f, 0.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}

	void loadScene(std::string name)
	{
		SceneLoader loader(scene);
		loader.LoadObj(name);

		// add light
		Light* light = new Light(vec3(-4.0f, 12.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(-4.0f, 5.0f, 12.0f), vec3(0.0f,1.0f,0.0f),-90.0f, 0.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
		//scene->Stabalize(0.1);
	}

	void loadDestructionScene()
	{
		SceneLoader loader(scene);
		loader.LoadObj(std::string("destruction"));

		// add light
		Light* light = new Light(vec3(-4.0f, 12.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(-4.0f, 5.0f, 12.0f), vec3(0.0f,1.0f,0.0f),-90.0f, 0.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
		//scene->Stabalize(0.1);
	}
	void loadMatterhornScene()
	{
		SceneLoader loader(scene);
		loader.LoadObj(std::string("matterhorn"));

		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(-4.0f, 12.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(-4.0f, 5.0f, 12.0f), vec3(0.0f,1.0f,0.0f),-90.0f, 0.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}

	void loadMasterScene()
	{
		SceneLoader loader(scene);
		loader.LoadObj(std::string("master"));

		// add light
		Light* light = new Light(vec3(-4.0f, 8.0f, 15.0f), vec3(-4.0f, 2.0f, 0.f));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(-4.0f, 8.0f, 12.0f), vec3(0.0f,1.0f,0.0f),-90.0f, 0.0f);
		scene->SetCamera(camera);

		// add balls
		
		int n = 50;
		float interval = 3;


		SpawnPoint* sp2 = new SpawnPoint(scene, vec3(-0.25,11,7), interval, n, [&](int i) {	

			vec3 color(0,0,0.6);
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(color, 20), vec3(-4,12,-1));
			ball->SetScale(vec3(0.12));
			ball->SetRotation(vec3(0,0,0));
			ball->GetRigidBody()->SetRestitution(0.5);
			ball->GetRigidBody()->SetFriction(0.1);
			return ball;
		});
		scene->AddEntity(sp2);


		cameraInput->SetCamera(scene->GetCamera());
	}
	void loadDraftScene()
	{
		SceneLoader loader(scene);
		loader.LoadObj(std::string("draft_scene"));

		// add light
		Light* light = new Light(vec3(-4.0f, 8.0f, 15.0f), vec3(-4.0f, 2.0f, 0.f));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(-4.0f, 8.0f, 12.0f), vec3(0.0f,1.0f,0.0f),-90.0f, 0.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}
	




	void createCatapultScene()
	{
		scene->Clear();
		
		// add ramp
		RigidBodyModel* ramp = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(0, 1.15,0));
		ramp->SetScale(vec3(6.,0.3,2.));
		scene->AddEntity(ramp);
		
		RigidBodyModel* cylinder = new RigidBodyModel(MeshGenerator::CreateCylinder(), vec3(0, 0.5, 0));
		cylinder->SetScale(vec3(0.5,2,0.5));
		cylinder->SetRotation(vec3(radians(90.),0.,0.));
		cylinder->SetStatic();
		scene->AddEntity(cylinder);
		
		// ball
		RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(.4,.4,.4)), vec3(-1.5,6.,0));
		ball->SetScale(vec3(0.5));
		ball->SetRotation(vec3(radians(90.0f),0,radians(90.0f)));
		scene->AddEntity(ball);
		
		// quaders
		// initializing random generator. Uniform distribution [0,1]. For the moment
		std::mt19937 gen(13);
		std::uniform_real_distribution<double> dist(0,1);
		for (int i=1; i<=1; i++)
		{
			RigidBodyModel* quads = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(dist(gen)+1, 1.35, 2*dist(gen)-1));
			quads->SetScale(vec3(0.1,0.1,0.1));
			scene->AddEntity(quads);
		}
		for (int i=1; i<=1; i++)
		{
			RigidBodyModel* quads = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(-dist(gen)-1, 1.35, 2*dist(gen)-1));
			quads->SetScale(vec3(0.1,0.1,0.1));
			scene->AddEntity(quads);
		}


		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 8.0f, 8.0f), vec3(0.0f,1.0f,0.0f),-90.0f,-45.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}

	/*
	 * Test scene for physic engine dev
	 */
	void createTestScene()
	{
		scene->Clear();

		Model* center = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(1,0,0)), vec3(0,2,0));
		center->SetScale(vec3(0.4));
		center->SetRotation(vec3(0,1,0));
		scene->AddEntity(center);


		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 8.0f, 8.0f), vec3(0.0f,1.0f,0.0f),-90.0f,-45.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}
	
	void createContactScene()
	{
		scene->Clear();
		
		// load bunny
		/*
		Entity* bunny = new Entity(vec3(0,0.5,10));
		createBunny(bunny);
		bunny->SetPosition(vec3(0,0.5,10));
		bunny->SetScale(vec3(20,20,20));
		bunny->SetRotation(radians(vec3(0,0,0)));
		scene->AddEntityAndFlattenHierachy(bunny);
		*/
		
		// Falling cube
		{
			vec3 pos(0, 2, -3);
			float sideLength = 0.5;
			
			RigidBodyModel* TopBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), pos);
			TopBox->SetScale(vec3(sideLength));
			TopBox->SetRotation(radians(vec3(44.f,0.f,34)));
			scene->AddEntity(TopBox);
		}
		// Inclined plane
		{
			float endAngle = 45; // deg
			float startAngle = 5;	// deg
			int no = 6;
			float deltaAngle = (endAngle-startAngle)/(no-1.);
			// plane / ramp
			float L = 1.5f;
			float B = 0.5f;
			float H = 0.1f;
			// box
			float l = 0.4f;
			float b = 0.2f;
			float h = 0.2f;
			for(int i = 0; i < no; ++i)
			{
				// inclined plane
				vec3 pos(3.+i*1.,0.3+i*0.2,0);	
				RigidBodyModel* ramp = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1./i,.6,.8)), pos);
				ramp->SetScale(vec3(B,H,L));
				float angle = startAngle+deltaAngle*i;
				ramp->SetRotation(vec3(radians(-angle),0,0.));
				scene->AddEntity(ramp);
				ramp->SetStatic();
				
				// box on inclined plane
				float deltaZloc = (H+h)/2.f;
				float deltaZglob = deltaZloc*std::cos(radians(angle));
				float deltaXglob = deltaZloc*std::sin(radians(angle));
				vec3 posBox = pos + vec3(0,deltaZglob,-deltaXglob)+vec3(0,std::sin(radians(angle))*L/4, std::cos(radians(angle))*L/4);
				RigidBodyModel* box = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1./i,.7,.8)), posBox);
				box->SetScale(vec3(b,h,l));
				box->GetRigidBody()->SetMass(0.6);
				box->SetRotation(vec3(radians(-angle),0,0.));
				scene->AddEntity(box);
			}
		}
		// pyramid stacking
		{
			float s = 0.5f;
			float dist = s;
			vec3 pos(0,s/2,3);
			RigidBodyModel* box1 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), pos+vec3(dist,0,0));
			box1->SetScale(vec3(s,s,s));
			box1->GetRigidBody()->SetFriction(1);
			box1->SetRotation(quat(0,0,0,1));
			scene->AddEntity(box1);
			RigidBodyModel* box2 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), pos-vec3(dist,0,0));
			box2->SetScale(vec3(s,s,s));
			box2->GetRigidBody()->SetFriction(1);
			box2->SetRotation(quat(0,0,0,1));
			scene->AddEntity(box2);
			// pyramid
			RigidBodyModel* pyramid = new RigidBodyModel(MeshGenerator::CreatePyramid(vec3(1,1,0)), pos);
			pyramid->SetScale(vec3(2*s,2*s,2*s));
			pyramid->SetRotation(radians(vec3(0,0,0)));
			scene->AddEntity(pyramid);
		}
		// Wall stacking
		{
			// add tower
			int width = 1;
			int height = 9;
			int depth = 15;
			GLfloat size = 0.5;
			GLfloat epsilon = 0.01;

			for (int x=0; x<width; ++x)
			{
				for (int y=0; y<height; ++y)
				{
					for (int z=0; z<depth; ++z)
					{
						float brightness = (float)y/height;
						vec3 color = vec3(.1+brightness/2,.2+brightness/2,.3+brightness); 
						RigidBodyModel* stone = new RigidBodyModel(
									MeshGenerator::CreateBox(color),
									vec3(-8+(size+epsilon)*x, size/2. + size*y, size*(z-depth/2)));
						stone->SetScale(vec3(size));
						stone->SetRotation(quat(0,0,0,1));
						stone->GetRigidBody()->SetMass(0.5);
						stone->GetRigidBody()->SetFriction(0.4);
						scene->AddEntity(stone);
					}
				}
			}

			double offset = 6;
		}



		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(20));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(1,7,1), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 1.50f, 0.0f));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}
	void createBallJointScene()
	{
		scene->Clear();
		// simple ball joint
		{
			/// TODO: doesn't work with pyramid... somehow flawed inertia tensor or something else?
			vec3 joint(0, 3, -3);
			float sideLength = 0.5;
			float gap = 0.5*sideLength;
			vec3 offset(0.,std::sqrt(3)*sideLength/2.+gap/2.,0);
			
			RigidBodyModel* TopBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), joint + offset);
			TopBox->SetScale(vec3(sideLength));
			TopBox->SetRotation(radians(vec3(45.f,0.f,35.26439)));
			TopBox->SetStatic();
			scene->AddEntity(TopBox);
			
			RigidBodyModel* BottomBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), joint - offset);
			BottomBox->SetScale(vec3(sideLength));
			BottomBox->SetRotation(radians(vec3(45.f,0.f,35.26439)));
			scene->AddEntity(BottomBox);
			
			BallJointConstraint* constraint = new BallJointConstraint(TopBox->GetRigidBody(), BottomBox->GetRigidBody(), joint);
			scene->GetPhysicManager()->AddConstraint(constraint);
			
			Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), joint);
			center->SetScale(vec3(gap/2.));
			scene->AddEntity(center);
		}
		// multiple ball joint
		{
			int noOfJoints = 2;			
			vec3 joint(3, 3, 0);
			float sideLength = 0.5;
			float gap = 0.5*sideLength;
			vec3 offset(0.,std::sqrt(3)*sideLength/2.+gap/2.,0);
			
			RigidBodyModel* TopBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), joint + offset);
			TopBox->SetScale(vec3(sideLength));
			TopBox->SetRotation(radians(vec3(45.f,0.f,35.26439)));
			TopBox->SetStatic();
			scene->AddEntity(TopBox);
			for(int i = 0; i < noOfJoints; ++i)
			{
				RigidBodyModel* BottomBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), joint - offset);
				BottomBox->SetScale(vec3(sideLength));
				BottomBox->SetRotation(radians(vec3(45.f,0.f,35.26439)));
				scene->AddEntity(BottomBox);
				
				BallJointConstraint* constraint = new BallJointConstraint(TopBox->GetRigidBody(), BottomBox->GetRigidBody(), joint);
				scene->GetPhysicManager()->AddConstraint(constraint);
			
				joint = joint - offset-offset;
				TopBox = BottomBox;
			}
		}
		

		// Ball joint rope
		{
			double L = 0.2;
			double s = 0.05;
			vec3 fixPoint(0, 3, 3);
			RigidBodyModel* TopBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), fixPoint);
			TopBox->SetScale(vec3(s,L,s));
			TopBox->SetRotation(vec3(0,0,0));
			TopBox->SetStatic();
			scene->AddEntity(TopBox);
			
			RigidBodyModel* oldBox = TopBox;
			float gap = 0.3*s;
			vec3 dst(0, L+gap, 0);
			int no = fixPoint[1]/dst[1]*0.8;
			for(int i = 1; i <= no; ++i)
			{
				vec3 pos = fixPoint - float(i)*dst;
				RigidBodyModel* newBox;
				newBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), pos);
				newBox->SetScale(vec3(s,L,s));
				newBox->SetRotation(vec3(0,0,0));
				scene->AddEntity(newBox);
				
				vec3 jointPos = pos + 0.5f*dst;
				BallJointConstraint* constraint = new BallJointConstraint(newBox->GetRigidBody(), oldBox->GetRigidBody(), jointPos);
				scene->GetPhysicManager()->AddConstraint(constraint);
				oldBox = newBox;
			}
		}


		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(-2,4,0), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 2.0f, 0.0f));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}
	
	void createHingeJointScene()
	{
		scene->Clear();
		vec3 red(0.6,0,0);
		vec3 blue(0,0,0.6);
		vec3 yellow(0,0.6,0);
		
		// hinge scene1
		{
			vec3 position(0,2,-3);
			RigidBodyModel* box1;
			box1 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position);
			box1->SetScale(vec3(0.8,0.1,0.4));
			box1->SetStatic();
			scene->AddEntity(box1);
			
			vec3 dist(1,0,0);
			RigidBodyModel* box2;
			box2 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position + dist);
			box2->SetScale(vec3(0.8,0.1,0.4));
			scene->AddEntity(box2);
			
			vec3 axisPos = position + dist/2.f;
			vec3 axis(0,0,1);
			HingeConstraint* constraint = new HingeConstraint(box1->GetRigidBody(), box2->GetRigidBody(), axis, axisPos);
			scene->GetPhysicManager()->AddConstraint(constraint);
		}
		
		// hinge scene2
		{
			vec3 position(3,3,0);
			RigidBodyModel* box1;
			box1 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position);
			box1->SetScale(vec3(0.4,0.8,0.1));
			box1->SetStatic();
			scene->AddEntity(box1);
			
			vec3 dist(0,-1,0);
			RigidBodyModel* box2;
			box2 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position + dist);
			box2->SetScale(vec3(0.4,0.8,0.1));
			scene->AddEntity(box2);
			
			vec3 axisPos = position + dist/2.f;
			vec3 axis(1,0,0);
			HingeConstraint* dst = new HingeConstraint(box1->GetRigidBody(), box2->GetRigidBody(), axis, axisPos);
			scene->GetPhysicManager()->AddConstraint(dst);
		}
		// blue gearwheel
		{
			// midpoint of polygon
			vec3 position(0,2,3);
			double radius = 0.3;
			RigidBodyModel* box;
			box = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position + vec3(0,0,0.2));
			box->SetScale(vec3(0.1,0.1,0.03));
			box->SetRotation(radians(vec3(0,0,0)));
			box->SetStatic();
			scene->AddEntity(box);
			// polygon
			RigidBodyModel* polygon;
			polygon = new RigidBodyModel(MeshGenerator::CreateCylinder(blue, 20), position );
			polygon->SetScale(vec3(radius,0.2,radius));
			polygon->SetRotation(radians(vec3(90,0,0)));
			scene->AddEntity(polygon);
			HingeConstraint* dst = new HingeConstraint(box->GetRigidBody(), polygon->GetRigidBody(), vec3(0,0,0.3), position);
			scene->GetPhysicManager()->AddConstraint(dst);
			// sticks around polygon
			double L_ = -radius-0.06;
			int sticks = 10;
			double stick_radius = 0.06;
			for (int i=0; i<sticks; i++) {
				vec3 radius(L_*cos(i*M_PI/(sticks/2.) + M_PI/(sticks*2)),L_*sin(i*M_PI/(sticks/2.) + M_PI/(sticks*2)),0);
				RigidBodyModel* stick = new RigidBodyModel(MeshGenerator::CreateCylinder(blue), position + radius);
				stick->SetScale(vec3(stick_radius, 0.2, stick_radius));
				stick->SetRotation(radians(vec3(90,0,0)));
				scene->AddEntity(stick);
				vec3 axisPos = position + radius;
				vec3 axis(0,0,1);
				HingeConstraint* stick_dst = new HingeConstraint(polygon->GetRigidBody(), stick->GetRigidBody(), axis, axisPos);
				scene->GetPhysicManager()->AddConstraint(stick_dst);
			}
			
			// box
			vec3 distance(radius*2.5,0,0);
			RigidBodyModel* box1 = new RigidBodyModel(MeshGenerator::CreateBox(), position + distance);
			box1->SetScale(vec3(0.4,0.1,0.2));
			scene->AddEntity(box1);
			vec3 axisPos1 = position + distance/2.f;
			vec3 axis(0,0,1);
			HingeConstraint* dst1 = new HingeConstraint(polygon->GetRigidBody(), box1->GetRigidBody(), axis, axisPos1);
			scene->GetPhysicManager()->AddConstraint(dst1);
			
			// box 2
			RigidBodyModel* box2 = new RigidBodyModel(MeshGenerator::CreateBox(), position + distance*2.f);
			box2->SetScale(vec3(0.4,0.1,0.2));
			scene->AddEntity(box2);
			vec3 axisPos2 = position + distance*3.f/2.f;
			HingeConstraint* dst2 = new HingeConstraint(box1->GetRigidBody(), box2->GetRigidBody(), axis, axisPos2);
			scene->GetPhysicManager()->AddConstraint(dst2);
			
		}
		// red gearwheel
		{
			// midpoint of polygon
			vec3 position(-0.8,2.5,3);
			double radius = 0.5;
			RigidBodyModel* box;
			box = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position + vec3(0,0,0.2));
			box->SetScale(vec3(0.1,0.1,0.03));
			box->SetRotation(radians(vec3(0,0,0)));
			box->SetStatic();
			scene->AddEntity(box);
			// polygon
			RigidBodyModel* polygon;
			polygon = new RigidBodyModel(MeshGenerator::CreateCylinder(red, 20), position );
			polygon->SetScale(vec3(radius,0.2,radius));
			polygon->SetRotation(radians(vec3(90,0,0)));
			scene->AddEntity(polygon);
			HingeConstraint* dst1 = new HingeConstraint(box->GetRigidBody(), polygon->GetRigidBody(), vec3(0,0,0.3), position);
			scene->GetPhysicManager()->AddConstraint(dst1);
			// sticks around polygon
			double L_ = -radius-0.06;
			int sticks = 14;
			double stick_radius = 0.06;
			for (int i=0; i<sticks; i++) {
				vec3 radius(L_*cos(i*M_PI/(sticks/2.) + M_PI/(sticks*2)),L_*sin(i*M_PI/(sticks/2.) + M_PI/(sticks*2)),0);
				RigidBodyModel* stick = new RigidBodyModel(MeshGenerator::CreateCylinder(red), position + radius);
				stick->SetScale(vec3(stick_radius, 0.2, stick_radius));
				stick->SetRotation(radians(vec3(90,0,0)));
				scene->AddEntity(stick);
				vec3 axisPos = position + radius;
				vec3 axis(0,0,1);
				HingeConstraint* stick_dst = new HingeConstraint(polygon->GetRigidBody(), stick->GetRigidBody(), axis, axisPos);
				scene->GetPhysicManager()->AddConstraint(stick_dst);
			}
		}
		// yellow gearwheel
		{
			// midpoint of polygon
			vec3 position(-0.6,3.28,3);
			double radius = 0.2;
			RigidBodyModel* box;
			box = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position + vec3(0,0,0.2));
			box->SetScale(vec3(0.1,0.1,0.03));
			box->SetRotation(radians(vec3(0,0,0)));
			box->SetStatic();
			scene->AddEntity(box);
			// polygon
			RigidBodyModel* polygon;
			polygon = new RigidBodyModel(MeshGenerator::CreateCylinder(yellow, 20), position );
			polygon->SetScale(vec3(radius,0.2,radius));
			polygon->SetRotation(radians(vec3(90,0,0)));
			scene->AddEntity(polygon);
			HingeConstraint* dst1 = new HingeConstraint(box->GetRigidBody(), polygon->GetRigidBody(), vec3(0,0,0.3), position);
			scene->GetPhysicManager()->AddConstraint(dst1);
			// sticks around polygon
			double L_ = -radius-0.06;
			int sticks = 6;
			double stick_radius = 0.06;
			for (int i=0; i<sticks; i++) {
				vec3 radius(L_*cos(i*M_PI/(sticks/2.) + M_PI/(sticks*2)),L_*sin(i*M_PI/(sticks/2.) + M_PI/(sticks*2)),0);
				RigidBodyModel* stick = new RigidBodyModel(MeshGenerator::CreateCylinder(yellow), position + radius);
				stick->SetScale(vec3(stick_radius, 0.2, stick_radius));
				stick->SetRotation(radians(vec3(90,0,0)));
				scene->AddEntity(stick);
				vec3 axisPos = position + radius;
				vec3 axis(0,0,1);
				HingeConstraint* stick_dst = new HingeConstraint(polygon->GetRigidBody(), stick->GetRigidBody(), axis, axisPos);
				scene->GetPhysicManager()->AddConstraint(stick_dst);
			}
		}

		
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(0,6,1), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 2.0f, 0.0f));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}
	
	
	void createDistanceScene()
	{
		scene->Clear();
		// single pendulum
		{	 
			vec3 midpoint(0,2.5,-3);
			Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
			center->SetScale(vec3(0.05));
			scene->AddEntity(center);
			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(1,0,0));
			stone->SetScale(vec3(0.1));
			scene->AddEntity(stone);
			
			DistanceConstraint* dst = new DistanceConstraint(stone->GetRigidBody(), midpoint);
			scene->GetPhysicManager()->AddConstraint(dst);
		}
		// double pendulum
		{
			vec3 midpoint(3,2.5,0);
			Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
			center->SetScale(vec3(0.05));
			scene->AddEntity(center);
			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0,1,0));
			stone->SetScale(vec3(0.1));
			stone->GetRigidBody()->ApplyLinearMomentum(dvec3(0,0,0.1)); // push it out of equilibrium
			scene->AddEntity(stone);
			
			RigidBodyModel* stone2 = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0,1,0.5));
			stone2->SetScale(vec3(0.1));
			scene->AddEntity(stone2);
			DistanceConstraint* dst = new DistanceConstraint(stone->GetRigidBody(), midpoint);
			scene->GetPhysicManager()->AddConstraint(dst);
			
			TwoBodyDistanceConstraint* dst2 = new TwoBodyDistanceConstraint(stone->GetRigidBody(), stone2->GetRigidBody());
			scene->GetPhysicManager()->AddConstraint(dst2);
		}
		// impulse pendulum
		{
			for (int i=0; i<=2; i++)
			{
				float radius = 0.1;
				vec3 midpoint(i*radius*2,3,3);
				double L = 1;
				Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
				center->SetScale(vec3(0.05));
				scene->AddEntity(center);
				
				RigidBodyModel* stone;
				if(i != 0)
				{
					stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0,-L,0));
				}
				else
				{
					stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(-L,0,0));
				}
				stone->SetScale(vec3(radius));
				scene->AddEntity(stone);

				DistanceConstraint* dst = new DistanceConstraint(stone->GetRigidBody(), midpoint);
				scene->GetPhysicManager()->AddConstraint(dst);
			}	
		}
		
		
		// single spring
		{	
			vec3 midpoint(-3,2.5,0);
			Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
			center->SetScale(vec3(0.05));
			scene->AddEntity(center);
			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint - vec3(0,1,0));
			stone->SetScale(vec3(0.1));
			stone->GetRigidBody()->ApplyLinearMomentum(dvec3(0,-0.2,0)); // push it out of equilibrium
			scene->AddEntity(stone);
			SpringConstraint* dst = new SpringConstraint(stone->GetRigidBody(), midpoint);
			scene->GetPhysicManager()->AddConstraint(dst);
		}
		
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(0,6,0), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 2.0f, 0.0f));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}
	
	
	
	// regular n-polygon tower
	void add_nPolygonTower(int n, int noLayers, float L, float H, float B, vec3 pos, float friction, float mass, vec3 color = vec3(204/255.,102/255.,0/255.))
	{
		// regular n-polygon tower
		{	 
			float radius = L/(std::sin(M_PI/n)*2);	// Circumscribed circle diameter
			float deltaAngle = 2*M_PI/n;
			float prePenetration = 0.012f;
			vec3 size = vec3(H,B,L);
			for(int i = 0; i < noLayers; ++i)
			{
				float offsetAngle = 0;
				if(i % 2 == 1){
					offsetAngle = deltaAngle/2.;
				}
				for(int l = 0; l < n; ++l)
				{
					float angle = deltaAngle*l + offsetAngle;
					float height = B/2+i*B-prePenetration*i;
					vec3 kaplaPos = pos + vec3(radius*std::cos(angle), height, radius*std::sin(angle));
					RigidBodyModel* kapla = new RigidBodyModel(MeshGenerator::CreateBox(color), kaplaPos);
					kapla->SetScale(size);
					kapla->SetRotation(vec3(0,-angle,0));
					kapla->GetRigidBody()->SetFriction(friction);
					kapla->GetRigidBody()->SetMass(mass);
					scene->AddEntity(kapla);
				}
			}
		}
		std::cout << "n-polygon tower added with " << noLayers*n << " elements" << std::endl;
	}
	
	void add_squareTower(int noLayers, float L, float H, float B, vec3 pos, float friction, float mass, vec3 color = vec3(204/255.,102/255.,0/255.)){
		float deltaAngle = M_PI_2;
		float prePenetration = 0.012f;
		vec3 size = vec3(L,B,H);
		for(int i = 0; i < noLayers; i+=2)
		{
			// orientation
			// | |
			{
				RigidBodyModel* kapla1 = new RigidBodyModel(MeshGenerator::CreateBox(color), pos + vec3(L/2.-H/2.,B/2+i*B-prePenetration*(i+1),0));
				kapla1->SetScale(size);
				kapla1->SetRotation(vec3(0,deltaAngle,0));
				kapla1->GetRigidBody()->SetFriction(friction);
				kapla1->GetRigidBody()->SetMass(mass);
				scene->AddEntity(kapla1);
				RigidBodyModel* kapla2 = new RigidBodyModel(MeshGenerator::CreateBox(color), pos + vec3(-L/2.+H/2.,B/2+i*B-prePenetration*(i+1),0));
				kapla2->SetScale(size);
				kapla2->SetRotation(vec3(0,deltaAngle,0));
				kapla2->GetRigidBody()->SetFriction(friction);
				kapla2->GetRigidBody()->SetMass(mass);
				scene->AddEntity(kapla2);
			}
			// orientation
			// __
			// __
			{
				RigidBodyModel* kapla1 = new RigidBodyModel(MeshGenerator::CreateBox(color), pos + vec3(0,B/2+(i+1)*B-prePenetration*(i+2),L/2.-H/2.));
				kapla1->SetScale(size);
				kapla1->GetRigidBody()->SetFriction(friction);
				kapla1->GetRigidBody()->SetMass(mass);
				scene->AddEntity(kapla1);
				RigidBodyModel* kapla2 = new RigidBodyModel(MeshGenerator::CreateBox(color), pos + vec3(0,B/2+(i+1)*B-prePenetration*(i+2),-L/2.+H/2.));
				kapla2->SetScale(size);
				kapla2->GetRigidBody()->SetFriction(friction);
				kapla2->GetRigidBody()->SetMass(mass);
				scene->AddEntity(kapla2);
			}
		}
		std::cout << "square tower added with " << noLayers*2 << " elements" << std::endl;
	}
	
	void add_turningTower(int noLayers, float L, float H, float B, vec3 pos, float friction, float mass, vec3 color = vec3(204/255.,102/255.,0/255.)){
		float deltaAngle = M_PI/12;
		float prePenetration = 0.012f;
		vec3 size(L,H,B);
		for(int i = 0; i < noLayers; ++i)
		{	
			RigidBodyModel* kapla = new RigidBodyModel(MeshGenerator::CreateBox(color), pos + vec3(0,H/2+i*H-prePenetration*(i+1),0));
			kapla->SetScale(size);
			kapla->SetRotation(vec3(0,i*deltaAngle,0));
			kapla->GetRigidBody()->SetFriction(friction);
			kapla->GetRigidBody()->SetMass(mass);
			scene->AddEntity(kapla);
		}
		std::cout << "turning tower added with " << noLayers << " elements" << std::endl;
	}
	
	
	
	void createKAPLAScene()
	{
		scene->Clear();
		// Cheating :-)
		scene->GetPhysicManager()->SetTimestepDivider(10);
		scene->GetPhysicManager()->SetConstraintSolvingInterations(10);

		float friction = 0.3;
		float mass = 0.5;
		
		// KAPLA dimensions
		float scaling = 0.08f;
		float L = scaling * 15;
		float B = scaling * 3;
		float H = scaling * 1;
		vec3 color(204/255.,102/255.,0/255.);
		// turning tower
		{	 
			//add_turningTower(24, L, H, B, vec3(5,0,-2), friction, mass);
			//add_turningTower(24, L, H, B, vec3(-5,0,-2), friction, mass);
		}
		// square normal tower
		{	
			for(int i = 0; i < 4; ++i)
			{ 
				float x = i*1.3;
				int height = 25 - x*x;
				vec3 pos(0,0,-5);
				vec3 offset(i*L*1.5,0,0);
				
				//add_squareTower(height, L, H, B, pos+offset, friction, mass);
				if(i!=0)
				{
					//add_squareTower(height, L, H, B, pos-offset, friction, mass);
				}
			}
		}
		
		// multiple polygon tower
		{	
			vec3 pos(0,0,0); 
			//add_nPolygonTower(3, 20, L, H, B, pos, friction, mass, color);
			//add_nPolygonTower(6, 15, L, H, B, pos, friction, mass, color);
			//add_nPolygonTower(8, 10, L, H, B, pos, friction, mass, color);
			
			add_nPolygonTower(3, 60, L, H, B, pos, friction, mass, color);
			//~ add_nPolygonTower(10, 10, L, H, B, pos, friction, mass, color);
		}
		
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(50));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,10,5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(-2.0f, 4.0f, 12.0f));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());


		//scene->Stabalize(0.1);
	}
	
	void createPhysicTestScene()
	{
		scene->Clear();

		{
			//RigidBodyModel* model = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(0,1.2,0));
			//scene->AddEntity(model);
		}

		{
			//RigidBodyModel* model = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(2.5,1.8,0.5));
			//scene->AddEntity(model);
		}

		{
		//	RigidBodyModel* model2 = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(2,0.6,0));
		//	scene->AddEntity(model2);
		}

		{
			//RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(1, 0.6,0.5));
			//stone->SetScale(vec3(0.2,1,0.5));
			//scene->AddEntity(stone);
		}

		// composite
		/*{
			Entity* composite = new Entity(vec3(0,4,0));

			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(0.8,0,0));
			stone->SetScale(vec3(0.2,1,0.5));
			stone->SetRotation(radians(vec3(0,0,20)));

			RigidBodyModel* stone2 = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(-0.8,0,0));

			
			composite->AddChild(stone);
			composite->AddChild(stone2);
			composite->SetRotation(radians(vec3(0,0,45)));
			composite->OnParentChanged(); // important for rigidbodies because they are already handled by the physicmanager

			scene->AddEntity(composite);
		}*/

		// single
		 {
			 /*RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(-2.4,0.10,0.5));
			 stone->SetScale(vec3(0.2,1,0.5));
			 stone->SetRotation(radians(vec3(0,0,90)));
			 scene->AddEntity(stone);*/
		 }
		 {
			/*RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.08*k,.17*k,.25*k)), vec3(-2+i*0.6,0.5,0));
			stone->SetScale(vec3(0.2,1,0.5));
			scene->AddEntity(stone);*/
			/*RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(), vec3(1,1,-1));
			ball->SetScale(vec3(0.1));
			ball->GetRigidBody()->ApplyLinearMomentum(dvec3(.4,0,0));
			ball->SetRotation(vec3(radians(90.0f),0,radians(90.0f)));
			scene->AddEntity(ball);*/
		 }
		 {
			 int size = 0;
			 for (int i=0; i<size; ++i)
			{
				 for (int j=0; j<size; ++j)
				{
					RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(-i*2,0.12,2*j));
					stone->SetScale(vec3(0.2,1,0.5));
					stone->SetRotation(radians(vec3(0,0,90)));
					scene->AddEntity(stone);
				}
			}
		 }
		 /*double epsilon = 0.01;
		 double dst = 0.2+epsilon;
		 {
			 for (int i=0; i<2; ++i)
			 {
				 vec3 midpoint(-dst*i,0,0);
				 Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
				 center->SetScale(vec3(0.05));
				 scene->AddEntity(center);

				 RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0,-1,0));
				 stone->SetScale(vec3(0.1));
				 //stone->GetRigidBody()->ApplyLinearMomentum(dvec3(0.1,0,0)); // push it out of equilibrium
				 scene->AddEntity(stone);

				 DistanceConstraint* dst = new DistanceConstraint(stone->GetRigidBody(), midpoint);
				 scene->GetPhysicManager()->AddConstraint(dst);
			 }
		}
		 {
			vec3 midpoint(dst,0,0);
			Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
			center->SetScale(vec3(0.05));
			scene->AddEntity(center);

			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0,1,0));
			stone->SetScale(vec3(0.1));
			stone->GetRigidBody()->ApplyLinearMomentum(dvec3(0.1,0,0)); // push it out of equilibrium
			scene->AddEntity(stone);
			
			RigidBodyModel* stone2 = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0.5,1,0));
			stone2->SetScale(vec3(0.1));
			scene->AddEntity(stone2);

			DistanceConstraint* dst = new DistanceConstraint(stone->GetRigidBody(), midpoint);
			scene->GetPhysicManager()->AddConstraint(dst);
		}*/
		double epsilon = 0.01;
		double dst = 0.2+epsilon;
		//~ {
			//~ vec3 midpoint(dst,0,0);
			//~ Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
			//~ center->SetScale(vec3(0.05));
			//~ scene->AddEntity(center);
			//~ RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0,-1,0));
			//~ stone->SetScale(vec3(0.1));
			//~ stone->GetRigidBody()->ApplyLinearMomentum(dvec3(0,-0.5,0));
			//~ scene->AddEntity(stone);
			
			//~ SoftDistanceConstraint* dst = new SoftDistanceConstraint(stone->GetRigidBody(), midpoint);
			//~ scene->GetPhysicManager()->AddConstraint(dst);
			

		//~ }
		{	
			// single spring
			vec3 midpoint(dst,3,0);
			Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
			center->SetScale(vec3(0.05));
			scene->AddEntity(center);
			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint - vec3(0,1,0));
			stone->SetScale(vec3(0.1));
			stone->GetRigidBody()->ApplyLinearMomentum(dvec3(0,-0.2,0)); // push it out of equilibrium
			scene->AddEntity(stone);
			SpringConstraint* dst = new SpringConstraint(stone->GetRigidBody(), midpoint);
			scene->GetPhysicManager()->AddConstraint(dst);
			
		}
		{
			/*vec3 midpoint(dst,0,0);
			Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
			center->SetScale(vec3(0.05));
			scene->AddEntity(center);
			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0,1,0));
			stone->SetScale(vec3(0.1));
			stone->GetRigidBody()->ApplyLinearMomentum(dvec3(0.1,0,0)); // push it out of equilibrium
			scene->AddEntity(stone);
			
			RigidBodyModel* stone2 = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0.5,1,0));
			stone2->SetScale(vec3(0.1));
			scene->AddEntity(stone2);
			DistanceConstraint* dst = new DistanceConstraint(stone->GetRigidBody(), midpoint);
			scene->GetPhysicManager()->AddConstraint(dst);
			
			TwoBodyDistanceConstraint* dst2 = new TwoBodyDistanceConstraint(stone->GetRigidBody(), stone2->GetRigidBody());
			scene->GetPhysicManager()->AddConstraint(dst2);*/
		}
		
		
		// TwoBodyDistance constraint
		{
			double L = 0.2;
			double s = 0.05;
			vec3 fixPoint(1, 1.4, 0);
			RigidBodyModel* TopBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), fixPoint);
			TopBox->SetScale(vec3(s,L,s));
			TopBox->SetRotation(vec3(0,0,0));	/// TODO: why is the standard rotation not (0,0,0) ?
			TopBox->SetStatic();
			scene->AddEntity(TopBox);
			
			RigidBodyModel* oldBox = TopBox;
			for(int i = 1; i <= 3; ++i)
			{
				vec3 dist(0, L*1.1*i, 0);
				RigidBodyModel* newBox;
				newBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), fixPoint - dist);
				newBox->SetScale(vec3(s,L,s));
				newBox->SetRotation(vec3(0,0,0));
				scene->AddEntity(newBox);
				
				TwoBodyDistanceConstraint* dst = new TwoBodyDistanceConstraint(newBox->GetRigidBody(), oldBox->GetRigidBody(), vec3(0,0.5,0), vec3(0,-0.5,0));
				scene->GetPhysicManager()->AddConstraint(dst);
				oldBox = newBox;
			}
		}
		
		// Ball joint constraint
		{
			double L = 0.2;
			double s = 0.05;
			vec3 fixPoint(1.5, 1.4, 0);
			RigidBodyModel* TopBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0.5,0.5,0)), fixPoint);
			TopBox->SetScale(vec3(s,L,s));
			TopBox->SetRotation(vec3(0,0,0));
			TopBox->SetStatic();
			scene->AddEntity(TopBox);
			
			RigidBodyModel* oldBox = TopBox;
			vec3 dst(0, L*1.1, 0);
			for(int i = 1; i <= 3; ++i)
			{
				vec3 pos = fixPoint - float(i)*dst;
				RigidBodyModel* newBox;
				newBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0.5,0.5,0)), pos);
				newBox->SetScale(vec3(s,L,s));
				newBox->SetRotation(vec3(0,0,0));
				scene->AddEntity(newBox);
				
				vec3 jointPos = pos + 0.5f*dst;
				BallJointConstraint* dst = new BallJointConstraint(newBox->GetRigidBody(), oldBox->GetRigidBody(), jointPos);
				scene->GetPhysicManager()->AddConstraint(dst);
				oldBox = newBox;
			}
		}
		
		// hinge scene1
		{
			vec3 position(2,1,0);
			RigidBodyModel* box1;
			box1 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position);
			box1->SetScale(vec3(0.8,0.1,0.4));
			//~ box1->SetRotation(vec3(0,0,0));
			//box1->SetStatic();
			scene->AddEntity(box1);
			
			RigidBodyModel* box2;
			box2 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position + vec3(1,0,0));
			box2->SetScale(vec3(0.8,0.1,0.4));
			//~ box2->SetRotation(vec3(0,0,0));
			scene->AddEntity(box2);
			
			HingeConstraint* dst = new HingeConstraint(box1->GetRigidBody(), box2->GetRigidBody(), vec3(0,0,1), vec3(2.5,1,0));
			scene->GetPhysicManager()->AddConstraint(dst);
		}
		
		// hinge scene2
		{
			vec3 position(-1,1.3,0);
			RigidBodyModel* box1;
			box1 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position);
			box1->SetScale(vec3(0.8,0.1,0.4));
			box1->SetRotation(radians(vec3(0,0,90)));
			box1->SetStatic();
			scene->AddEntity(box1);
			
			RigidBodyModel* box2;
			box2 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position + vec3(0,-1,0));
			box2->SetScale(vec3(0.8,0.1,0.4));
			box2->SetRotation(radians(vec3(0,0,90)));
			scene->AddEntity(box2);
			
			HingeConstraint* dst = new HingeConstraint(box1->GetRigidBody(), box2->GetRigidBody(), vec3(0,0,1), position - vec3(0,.5,0));
			scene->GetPhysicManager()->AddConstraint(dst);
		}
	
		//~ {
			//~ RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(1.5,0.2,0));
			//~ stone->SetScale(vec3(0.2,1,0.5));
			//~ stone->SetRotation(radians(vec3(0,0,90)));
			//~ scene->AddEntity(stone);
		//~ }

		// stack
		/*{
			for (int i=0; i<2; ++i)
			{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(1.2,0.21 + i*0.2,0.5));
				stone->SetScale(vec3(0.2,1,0.5));
				stone->SetRotation(radians(vec3(0,0,90)));
				scene->AddEntity(stone);
			}
		}*/

		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,-2,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 1.0f, 4.0f));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}

	void createRopeScene()
	{
		scene->Clear();

		{
			
			double L = 0.1;
			double s = 0.05;
			int length = 10;

			vec3 fixPoint(1, 1.4, 0);
			RigidBodyModel* TopBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), fixPoint);
			TopBox->SetScale(vec3(s,L,s));
			TopBox->SetRotation(vec3(0,0,0));	/// TODO: why is the standard rotation not (0,0,0) ?
			TopBox->SetStatic();
			scene->AddEntity(TopBox);
			
			RigidBodyModel* oldBox = TopBox;
			for(int i = 1; i <= length; ++i)
			{
				vec3 dist(0, L*1.1*i, 0);
				RigidBodyModel* newBox;
				newBox = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), fixPoint - dist);
				newBox->SetScale(vec3(s,L,s));
				newBox->SetRotation(vec3(0,0,0));
				scene->AddEntity(newBox);
				
				BallJointConstraint* dst = new BallJointConstraint(newBox->GetRigidBody(), oldBox->GetRigidBody(), fixPoint - dist + vec3(0,L/2.,0));
				scene->GetPhysicManager()->AddConstraint(dst);
				oldBox = newBox;
			}

			double ballSize = 0.2;
			L = L / 2;
			vec3 dist(0, L*1.1*(length+1)+ballSize, 0);
			RigidBodyModel* ball;
			ball = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(1,1,0)), fixPoint - dist);
			ball->SetScale(vec3(ballSize));
			ball->SetRotation(vec3(0,0,0));
			scene->AddEntity(ball);
			
			BallJointConstraint* dst = new BallJointConstraint(ball->GetRigidBody(), oldBox->GetRigidBody(), fixPoint - dist + vec3(0,(L+ballSize)/2.,0));
			scene->GetPhysicManager()->AddConstraint(dst);
		}

		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,-2,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 1.0f, 4.0f));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}
	
	
	void createTrampolinScene()
	{
		scene->Clear();

		// elastic web
		{
			double dx = 0.15;
			double dy = dx;
			const int x_size = 10;
			const int y_size = 10;
			RigidBodyModel **RidgidBodyArray = new RigidBodyModel*[x_size*y_size];
			
			for(int x = 0; x < x_size; ++x)
			{
				for(int y = 0; y < y_size; ++y)
				{
					RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(1.0f), 20), vec3(x*dx,0.5,y*dy));
					if((y == 0 || y==y_size-1) || (x == 0 || x == x_size-1)){
						ball->SetStatic();
					}
					ball->SetScale(vec3(0.05));
					scene->AddEntity(ball);
					RidgidBodyArray[y*x_size+x] = ball; 
				}
			}
			// add constraints
			for(int x = 0; x < x_size; ++x)
			{
				RigidBodyModel* last = RidgidBodyArray[x];
				for(int y = 1; y < y_size; ++y)
				{
					RigidBodyModel* next = RidgidBodyArray[y*x_size+x];
					SoftTwoBodyDistanceConstraint* dst = new SoftTwoBodyDistanceConstraint(last->GetRigidBody(), next->GetRigidBody());
					scene->GetPhysicManager()->AddConstraint(dst);
					last = next;
				}
			}
			for(int y = 0; y < y_size; ++y)
			{
				RigidBodyModel* last = RidgidBodyArray[y*x_size];
				for(int x = 1; x < x_size; ++x)
				{
					RigidBodyModel* next = RidgidBodyArray[y*x_size+x];
					SoftTwoBodyDistanceConstraint* dst = new SoftTwoBodyDistanceConstraint(last->GetRigidBody(), next->GetRigidBody());
					scene->GetPhysicManager()->AddConstraint(dst);
					last  = next;
				}
			}
			
			
			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), vec3((x_size-1)*dx/2,4,(y_size-1)*dy/2));
			stone->SetScale(vec3(0.3));
			stone->GetRigidBody()->SetMass(2);
			scene->AddEntity(stone);
			
		}

		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,-2,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 1.0f, 4.0f));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}

	void createTowerStabilityScene()
	{
		scene->Clear();

		// add tower
		int width = 5;
		int height = 5;
		int depth = 5;
		GLfloat size = 0.6;
		GLfloat epsilon = 0.01;

		for (int x=0; x<width; ++x)
		{
			for (int y=0; y<height; ++y)
			{
				for (int z=0; z<depth; ++z)
				{
					RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.1*y,.2*y,.3*y)), vec3((size+epsilon)*x, size/2. + size*y, size*(z-depth/2)));
					stone->SetScale(vec3(size));
					stone->SetDirty();
					stone->GetRigidBody()->SetMass(0.5);
					scene->AddEntity(stone);
				}
			}
		}

		// add floor
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 8.0f, 8.0f), vec3(0.0f,1.0f,0.0f),-90.0f,-45.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}
	
	
	
	
	// get Get wall as entity
	void GetWall( Entity* wallPicture, std::string filename = "CGL_Logo-Text_Right"){
		
		// add tower
		float L = .5;
		float B = .5;
		float H = .5;
		float epsilon = 0.01;
		float collisionThreshold = 0.01;	// how much are the bodies colliding in the beginning?
		PictureLoader picture;

		picture.LoadPicture(filename);
		std::cout << "Size: " << picture.xRes << "x" << picture.yRes << std::endl;
		vec3 pos = vec3(-picture.xRes*L/2,0,0);
		for(int i = 0; i < picture.xRes; ++i)
		{
			for(int j = 0; j < picture.yRes; ++j)
			{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(picture.rgbValues[i][j]), pos+vec3((L+epsilon)*i,H*j+0.5*H-collisionThreshold*j,pos[2]));
				stone->SetScale(vec3(L,H,B));
				stone->GetRigidBody()->SetMass(0.5);
				stone->GetRigidBody()->SetFriction(0.2);
				wallPicture->AddChild(stone);
			}
		}	
		wallPicture->OnParentChanged();
	}
	
	// get car as entity
	// not working???
	void GetCar(Entity* car){
		
		// car body dimensions
		float L = 1.5;
		float B = .7;
		float H = .5;
		
		// car wheel dimensions
		float r = .15;
		float h = 0.15;
		float wheelBase = L*0.8;
		float clearance = 0.05;
		// car body
		RigidBodyModel* carBody = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1,1,0)), vec3(0,H/2. + H*0.2,0));
		carBody->SetScale(vec3(L,H,B));
		//~ carBody->SetRotation(radians(vec3(0,0,0)));
		car->AddChild(carBody);
		
		// constraint parameter
		std::vector<vec3> posAxis = {vec3(-wheelBase/2, r, 0), vec3(wheelBase/2, r, 0)};
		vec3 wheelAxis(0,0,1);
		
		// wheels
		for(int i = 0; i < 2; ++i)
		{
			for(int j = 0; j < 2; ++j)
			{
				RigidBodyModel* carWheel = new RigidBodyModel(MeshGenerator::CreateCylinder(vec3(0.1,0.1,0.1)), vec3(posAxis[i][0], posAxis[i][1] ,(-B-h-clearance)/2.f + j*(B+h+clearance)));
				carWheel->SetScale(vec3(r,h,r));
				carWheel->SetRotation(radians(vec3(90,0,0)));
				
				// axis / hinge constraint
				HingeConstraint* hinge = new HingeConstraint(carBody->GetRigidBody(), carWheel->GetRigidBody(), wheelAxis, posAxis[i]);
				scene->GetPhysicManager()->AddConstraint(hinge);
				
				car->AddChild(carWheel);
				car->OnParentChanged();
			}
		}
		car->OnParentChanged();
	}
	
	// get final domino scene wall as entity
	// origin (0,0,0) is placed at the first single domino
	void GetFinalDomino( Entity* dominoPicture, std::string filename = "CGL_Logo-Text_Right"){
		// domino dimensions
		float L = 1.;
		float B = .5;
		float H = .2;
		float epsilon = 0.01;
		// distance of dominos
		float distX = 0.6*L-H;
		float distY = 0.1*B;
		// picture import
		PictureLoader picture;
		picture.LoadPicture(filename);
		std::cout << "Size: " << picture.xRes << "x" << picture.yRes << std::endl;
		
		vec3 pos = vec3(0,0,0);
		for(int i = 1; i < picture.yRes; ++i)
		{
			float startX = (i-1)*(H+distX);
			float startY = -(i*(B+distY) - distY)/2 +B/2;
			for(int j = 0; j < i; ++j)
			{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBoxOneSidedColored(vec3(1), picture.rgbValues[0][j]), vec3(startX, 0.5*L, startY + j*(B+distY) ));
				stone->SetScale(vec3(H,L,B));
				// trigger the domino for test reasons, only works with rotation of entity = 0
				if(i == 1 && j == 0)
				{
					//stone->GetRigidBody()->ApplyAngularMomentum(dvec3(0,0,-0.26));
				}
				dominoPicture->AddChild(stone);				
			}
			pos = vec3(startX,0,0);
		}
		
		pos += vec3(H+distX,0,picture.yRes*((B+distY) - distY)/2+B/2);
		for(int i = 0; i < picture.xRes; ++i)
		{
			for(int j = 0; j < picture.yRes; ++j)
			{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBoxOneSidedColored(vec3(1), picture.rgbValues[i][j]), pos+vec3((H+distX)*i, 0.5*L, -(B+distY)*j));
				stone->SetScale(vec3(H,L,B));
				dominoPicture->AddChild(stone);
			}
		}	
		dominoPicture->OnParentChanged();
	}
	
	void createDominoElevatorScene()
	{
		scene->Clear();
		Entity* dominoElevator = new Entity(vec3(0,0,0));
		createDominoElevator(dominoElevator);
		dominoElevator->SetPosition(vec3(5,0,0));
		dominoElevator->SetRotation(radians(vec3(0,25,0)));
		dominoElevator->OnParentChanged();

		scene->AddEntityAndFlattenHierachy(dominoElevator);
		//scene->AddEntity(dominoElevator);


		// calc camera position
		float L = 1.;
		float B = .5;
		float H = .2;
		float epsilon = 0.01;
		float distX = 0.6*L-H;
		float distY = 0.1*B;
		int dominos = 4;
		float boundarySpace = H;	// space at 'border' of platform
		float length = dominos*(distX + H) - distX + boundarySpace;
		
		// add camera
		Camera* camera = new Camera(vec3(length/2,6,12));
		scene->SetCamera(camera);
		cameraInput->SetCamera(scene->GetCamera());

		// add light
		Light* light = new Light(vec3(10,6,5), vec3(0));
		scene->SetLight(light);
	}

	void createDominoElevator(Entity* parent)
	{

		// domino dimensions
		float L = 1.;
		float B = .5;
		float H = .2;
		float epsilon = 0.01;
		// distance of dominos
		float distX = 0.6*L-H;
		float distY = 0.1*B;
		
		// no of dominos
		int dominos = 4;
		float boundarySpace = H;	// space at 'border' of platform
		float length = dominos*(distX + H) - distX + boundarySpace;
		
		vec3 pos(0,0,0);
		
		int noLevels = 7;
		
		for(int level = 0; level < noLevels; ++level)
		{
			float thickness = 0.1; // platform
			vec3 levelPos = pos + vec3(0,2*L*level,0);
			// 'fan' dimension
			float l = 3*L;
			float b = 2*B;
			float h = H;
			// distance of fan to platform and of dominos
			float delta = 0.05+h/2;
			vec3 fanPos(0,levelPos[1]+2*L - thickness/2,0);
			float offset = 0;
			float offsetBoundary = 0;
			float increasedLength = L;
			if( level % 2 == 0) // right side
			{
				fanPos[0] = length + delta;
				offsetBoundary = -boundarySpace;
			}
			else 	 // left side
			{
				fanPos[0] = - delta;
				offset = -increasedLength;
			}
			// platform for dominos
			RigidBodyModel* levelFloor = new RigidBodyModel(MeshGenerator::CreateBox(), levelPos + vec3( offset + (length + increasedLength)/2, -thickness/2,0));
			levelFloor->SetScale(vec3(length+increasedLength, thickness, 2*B));
			levelFloor->SetStatic();
			levelFloor->GetRigidBody()->SetFriction(0.7); 	// otherwise dominos are sliding away...
			parent->AddChild(levelFloor);	
			int startIndex = 0;
			// if highest platform
			if( level == noLevels-1)
			{
				if( level % 2 == 0)
				{
					++dominos;
				}
				else 	 // left side
				{
					startIndex  = -1;
				}
			}
			for(int i = startIndex; i < dominos; ++i)
			{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,0.3,0.7)), levelPos + vec3(boundarySpace + offsetBoundary + H/2+i*(H+distX), L/2, 0));
				stone->SetScale(vec3(H,L,B));
				// trigger the domino for test reasons, only works with rotation of entity = 0
				if(level == 0 && i == 0)
				{
					//stone->GetRigidBody()->ApplyAngularMomentum(dvec3(0,0,-0.25));
				}
				parent->AddChild(stone);
			}
			
			// if not highest platform
			if( level != noLevels-1)
			{	
				// decoration and needed for the hinge constraint
				RigidBodyModel* decoration = new RigidBodyModel(MeshGenerator::CreateCylinder(vec3(0.1,0.1,0.1)), fanPos + vec3(0,0,B+h/2+0.02));
				decoration->SetScale(vec3(h,h,h));
				decoration->SetRotation(radians(vec3(90,0,0)));
				decoration->SetStatic();
				parent->AddChild(decoration);
				// decoration
				Model* decoration1 = new Model(MeshGenerator::CreateCylinder(vec3(0.1,0.1,0.1)), fanPos + vec3(0,0,-(B+h/2+0.02)));
				decoration1->SetScale(vec3(h,h,h));
				decoration1->SetRotation(radians(vec3(90,0,0)));
				parent->AddChild(decoration1);
				
				// 'fan'
				RigidBodyModel* fan = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0.,0.5,0.5)), fanPos);
				fan->SetScale(vec3(h,l,b));
				fan->GetRigidBody()->SetMass(0.5);
				parent->AddChild(fan);
				
				// axis / hinge constraint
				HingeConstraint* hinge = new HingeConstraint(decoration->GetRigidBody(), fan->GetRigidBody(), vec3(0,0,1), fanPos);
				scene->GetPhysicManager()->AddConstraint(hinge);
			}
		}
	}

	void loadCarScene()
	{
		scene->Clear();
	
		// add car with entity
		Entity* car = new Entity(vec3(0,0,0));
		GetCar(car);
		car->SetRotation(radians(vec3(0,0,0)));
		car->SetPosition(vec3(1,0,0));
		car->OnParentChanged();
		scene->AddEntityAndFlattenHierachy(car);


		// add camera
		Camera* camera = new Camera(vec3(0,1,4));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	

		// add floor
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(50));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(10,6,5), vec3(0));
		scene->SetLight(light);
	}
	
	
	void createFinalDomino()
	{
		scene->Clear();

		// add domino with entity
		Entity* dominoPicture = new Entity(vec3(-10,0,-6));
		GetFinalDomino(dominoPicture);
		dominoPicture->SetRotation(radians(vec3(0,0,0)));
		dominoPicture->OnParentChanged();
		scene->AddEntityAndFlattenHierachy(dominoPicture); // no additional cost during simulation

		// add camera
		Camera* camera = new Camera(vec3(0,6,0));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	

		// add floor
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(50));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(10,6,5), vec3(0));
		scene->SetLight(light);


	}
	void createPictureScene()
	{
		scene->Clear();

		// add tower
		float L = .5;
		float B = .5;
		float H = .5;
		float epsilon = 0.01;
		float collisionThreshold = 0.01;	// how much are the bodies colliding in the beginning?
		PictureLoader picture;
		//~ picture.LoadPicture("ETHZ");
		picture.LoadPicture("duck");
		std::cout << "Size: " << picture.xRes << "x" << picture.yRes << std::endl;
		vec3 pos(-picture.xRes*L/2,0,0);
		for(int i = 0; i < picture.xRes; ++i)
		{
			for(int j = 0; j < picture.yRes; ++j)
			{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(picture.rgbValues[i][j]), pos+vec3((L+epsilon)*i,H*j+0.5*H-collisionThreshold*j,pos[2]));
				stone->SetScale(vec3(L,H,B));
				stone->GetRigidBody()->SetMass(0.5);
				stone->GetRigidBody()->SetFriction(0.2);
				scene->AddEntity(stone);
			}
		}
		
		
		// add camera
		float distance = (picture.xRes*L/2.)/std::tan(M_PI_4/2.)+pos[2];
		Camera* camera = new Camera(vec3(0., (picture.yRes*1.4)*H/2., distance*0.8));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
		

		/*
		picture.LoadPicture("trump_wall");
		std::cout << "Size: " << picture.xRes << "x" << picture.yRes << std::endl;
		pos = vec3(-picture.xRes*L/2,0,-12);
		for(int i = 0; i < picture.xRes; ++i)
		{
			for(int j = 0; j < picture.yRes; ++j)
			{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(picture.rgbValues[i][j]), pos+vec3((L+epsilon)*i,H*j+0.5*H-collisionThreshold*j,pos[2]));
				stone->SetScale(vec3(L,H,B));
				stone->GetRigidBody()->SetMass(0.5);
				stone->GetRigidBody()->SetFriction(0.2);
				scene->AddEntity(stone);
			}
		}		
		*/
		
		/*
		picture.LoadPicture("CGL_Logo-Text_Right");
		std::cout << "Size: " << picture.xRes << "x" << picture.yRes << std::endl;
		pos = vec3(-picture.xRes*L/2,0,12);
		for(int i = 0; i < picture.xRes; ++i)
		{
			for(int j = 0; j < picture.yRes; ++j)
			{
				RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(picture.rgbValues[picture.xRes-1-i][j]), pos+vec3((L+epsilon)*i,H*j+0.5*H-collisionThreshold*j,pos[2]));
				stone->SetScale(vec3(L,H,B));
				stone->GetRigidBody()->SetMass(0.5);
				stone->GetRigidBody()->SetFriction(0.2);
				scene->AddEntity(stone);
			}
		}
		*/

		// add floor
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(30));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(10,6,5), vec3(0));
		scene->SetLight(light);


	}
	void createTowerPendelScene()
	{
		scene->Clear();

		// add tower
		int width = 1;
		int height = 7;
		int depth = 1;
		GLfloat size = 0.6;
		GLfloat epsilon = 0.001;

		for (int x=0; x<width; ++x)
		{
			for (int y=0; y<height; ++y)
			{
				for (int z=0; z<depth; ++z)
				{
					RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.1*y,.2*y,.3*y)), vec3((size+epsilon)*x, size/2. + size*y, size*(z-depth/2)));
					stone->SetScale(vec3(size));
					stone->SetDirty();
					stone->GetRigidBody()->SetMass(0.5);
					scene->AddEntity(stone);
				}
			}
		}

		 {
			vec3 midpoint(0,4,0);
			double L = 3;
			Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
			center->SetScale(vec3(0.05));
			scene->AddEntity(center);

			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0,L,0));
			stone->SetScale(vec3(0.5));
			stone->GetRigidBody()->ApplyLinearMomentum(dvec3(4.4,0,0)); // push it out of equilibrium
			scene->AddEntity(stone);

			DistanceConstraint* dst = new DistanceConstraint(stone->GetRigidBody(), midpoint);
			scene->GetPhysicManager()->AddConstraint(dst);
		}

		// add floor
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 8.0f, 8.0f), vec3(0.0f,1.0f,0.0f),-90.0f,-45.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}

	void createTowerScene()
	{
		scene->Clear();

		// add tower
		int width = 1;
		int height = 9;
		int depth = 20;
		GLfloat size = 0.6;
		GLfloat epsilon = 0.01;

		for (int x=0; x<width; ++x)
		{
			for (int y=0; y<height; ++y)
			{
				for (int z=0; z<depth; ++z)
				{
					RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.1*y,.2*y,.3*y)), vec3((size+epsilon)*x, size/2. + size*y, size*(z-depth/2)));
					stone->SetScale(vec3(size));
					stone->GetRigidBody()->SetMass(0.5);
					stone->GetRigidBody()->SetFriction(0.4);
					scene->AddEntity(stone);
				}
			}
		}

		double offset = 6;

		/*for (int x=0; x<width; ++x)
		{
			for (int y=0; y<height; ++y)
			{
				for (int z=0; z<depth; ++z)
				{
					RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.1*y,.2*y,.3*y)), vec3((size+epsilon)*x -offset, size/2. + size*y, size*(z-depth/2)));
					stone->SetScale(vec3(size));
					stone->SetDirty();
					stone->GetRigidBody()->SetMass(0.5);
					scene->AddEntity(stone);
				}
			}
		}*/

		// add ramp
		/*RigidBodyModel* rightramp = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.4,.6,.8)), vec3(7,1,0));
		rightramp->SetScale(vec3(4,0.1,2));
		rightramp->SetRotation(vec3(0,0,radians(25.0f)));
		scene->AddEntity(rightramp);
		rightramp->SetStatic();*/

		// ball
		RigidBodyModel* myCylinder = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(.6,.4,.8), 15), vec3(8,1.5,0));
		myCylinder->SetScale(vec3(1));
		myCylinder->GetRigidBody()->SetMass(1);
		myCylinder->GetRigidBody()->ApplyLinearMomentum(dvec3(-1,0,0));
		myCylinder->SetRotation(vec3(radians(90.0f),0,radians(90.0f)));
		scene->AddEntity(myCylinder);

		// add floor
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(20));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 8.0f, 8.0f), vec3(0.0f,1.0f,0.0f),-90.0f,-45.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}

	void createStressTest()
	{
		scene->Clear();

		scene->GetPhysicManager()->SetTimestepDivider(3);
		scene->GetPhysicManager()->SetConstraintSolvingInterations(4);

		// borders
		double wallHeight = 30;
		double floorSize = 50;
		RigidBodyModel* Border1 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1.,1.,1.)), vec3(0.,double(wallHeight)/2.,floorSize/2));
		Border1->SetScale(vec3(floorSize, double(wallHeight), 0.1));
		Border1->SetStatic();
		Border1->SetVisible(false);
		scene->AddEntity(Border1);

		RigidBodyModel* Border2 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1.,1.,1.)), vec3(floorSize/2,double(wallHeight)/2.,0.));
		Border2->SetScale(vec3(0.1, double(wallHeight), floorSize));
		Border2->SetStatic();
		Border2->SetVisible(false);
		scene->AddEntity(Border2);

		RigidBodyModel* Border3 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1.,1.,1.)), vec3(0.,double(wallHeight)/2.,-floorSize/2));
		Border3->SetScale(vec3(floorSize, double(wallHeight), 0.1));
		Border3->SetStatic();
		Border3->SetVisible(false);
		scene->AddEntity(Border3);

		RigidBodyModel* Border4 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1.,1.,1.)), vec3(-floorSize/2,double(wallHeight)/2.,0.));
		Border4->SetScale(vec3(0.1, double(wallHeight), floorSize));
		Border4->SetStatic();
		Border4->SetVisible(false);
		scene->AddEntity(Border4);


		int width = 25;
		int height = 10;
		int depth = 25;
		GLfloat size = 0.2;
		GLfloat yoffset = 0;
		vec3 epsilon(1,0.001,1);

		std::cout << width*height*depth << " objects" << std::endl;

		bool first = true;
		Model* stone;

		for (int x=0; x<width; ++x)
		{
			for (int y=0; y<height; ++y)
			{
				for (int z=0; z<depth; ++z)
				{
					float brightness = (float)y/height;
					vec3 color = vec3(.1+brightness/2,.2+brightness/2,.3+brightness); 
					vec3 pos = vec3((size+epsilon.x)*(x-width/2), yoffset + size/2. + (size+epsilon.y)*y, (size+epsilon.z)*(z-depth/2));

					if (first)
					{
						 stone= new RigidBodyModel(
								MeshGenerator::CreateBox(color), 
								//MeshGenerator::CreateSphere(color), 8), 
								pos);

						stone->SetScale(vec3(size));
						scene->AddEntity(stone);
						first = false;
					}
					else
					{
						RigidBodyModel* instance = dynamic_cast<RigidBodyModel*>(stone->CreateInstance(pos));
						instance->SetScale(vec3(size));
						scene->AddEntity(instance);
					}

					/*stone->GetRigidBody()->SetFriction(0.8);
					stone->GetRigidBody()->SetFriction(0.21);
					stone->GetRigidBody()->SetRestitution(0.8);
					stone->GetRigidBody()->SetMass(0.5);*/
				}
			}
		}


		// add floor
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(0,-1,0));
		plane->SetScale(vec3(floorSize,2,floorSize));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 8.0f, 8.0f), vec3(0.0f,1.0f,0.0f),-90.0f,-45.0f);
		scene->SetCamera(camera);
		cameraInput->SetCamera(camera);

	}

	void createComplicatedScene()
	{
		scene->Clear();

		// add tower
		int width = 0;
		int height = 0;
		int depth = 0;
		GLfloat size = 0.6;

		for (int x=0; x<width; ++x)
		{
			for (int y=0; y<height; ++y)
			{
				for (int z=0; z<depth; ++z)
				{
					RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.1*y,.2*y,.3*y)), vec3(size*x, size/2. + size*y, size*z));
					stone->SetScale(vec3(size));
					scene->AddEntity(stone);
				}
			}
		}

		// Walls
		vec3 wallSize(1,2,0.2);
		double wall_height = wallSize.y;
		//stone->SetScale(vec3(0.2,1,0.5));
		for (int i=4; i<=7; i++) {
			RigidBodyModel* FallingWall1 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.8,.6,.2)), vec3(double(i),double(wall_height)/2.,double(i)));
			FallingWall1->SetScale(wallSize);
			FallingWall1->SetRotation(radians(vec3(0,45,0)));
			scene->AddEntity(FallingWall1);
		}

		RigidBodyModel* FallingWall2 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.7,.3,.6)), vec3(-4.,double(wall_height)/2.,4.));
		FallingWall2->SetScale(wallSize);
		FallingWall2->SetRotation(radians(vec3(0,135,0)));
		scene->AddEntity(FallingWall2);

		RigidBodyModel* FallingWall3 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.4,.7,.5)), vec3(4.,double(wall_height)/2.,-4.));
		FallingWall3->SetScale(wallSize);
		FallingWall3->SetRotation(radians(vec3(0,135,0)));
		scene->AddEntity(FallingWall3);

		RigidBodyModel* FallingWall4 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.4,.2,.6)), vec3(-4.,double(wall_height)/2.,-4.));
		FallingWall4->SetScale(wallSize);
		FallingWall4->SetRotation(radians(vec3(0,45,0)));
		scene->AddEntity(FallingWall4);

		// borders
		height = 6;
		RigidBodyModel* Border1 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1.,1.,1.)), vec3(0.,double(height)/2.,10.));
		Border1->SetScale(vec3(20., double(height), 0.1));
		Border1->SetStatic();
		Border1->SetVisible(false);
		scene->AddEntity(Border1);

		RigidBodyModel* Border2 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1.,1.,1.)), vec3(10.,double(height)/2.,0.));
		Border2->SetScale(vec3(0.1, double(height), 20.));
		Border2->SetStatic();
		Border2->SetVisible(false);
		scene->AddEntity(Border2);

		RigidBodyModel* Border3 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1.,1.,1.)), vec3(0.,double(height)/2.,-10.));
		Border3->SetScale(vec3(20., double(height), 0.1));
		Border3->SetStatic();
		Border3->SetVisible(false);
		scene->AddEntity(Border3);

		RigidBodyModel* Border4 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(1.,1.,1.)), vec3(-10.,double(height)/2.,0.));
		Border4->SetScale(vec3(0.1, double(height), 20.));
		Border4->SetStatic();
		Border4->SetVisible(false);
		scene->AddEntity(Border4);



		// rectangular lane
		Entity* lane1 = Combos::Lane1();
		lane1->SetPosition(vec3(0,3.5,-7));
		lane1->SetRotation(radians(vec3(180,0,0)));
		lane1->OnParentChanged(); // important for rigidbodies because they are already handled by the physicmanager
		scene->AddEntity(lane1);

		Entity* lane2 = Combos::Lane2();
		lane2->SetPosition(vec3(-7,3.5,0));
		lane2->OnParentChanged(); // important for rigidbodies because they are already handled by the physicmanager
		scene->AddEntity(lane2);


		int balls = 150;
		for (int i=0; i<balls; i++) {

			// ball
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(.0+double(i)/20,.0-(double(i)/20),.3), 10), vec3(-7,3.8+double(i)/3,-7+double(i)/10));
			ball->SetScale(vec3(0.1));
			ball->SetRotation(vec3(radians(90.0f),0,radians(90.0f)));
			scene->AddEntity(ball);
		}

		for (int i=0; i<balls; i++) {
			// ball
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(.1-(double(i)/20),.0+(double(i)/20),.3), 10), vec3(-7+double(i)/10,3.8+double(i)/3,-7));
			ball->SetScale(vec3(0.1));
			ball->SetRotation(vec3(radians(90.0f),0,radians(90.0f)));
			scene->AddEntity(ball);
		}


		// add ramp
		RigidBodyModel* rightramp = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.4,.6,.8)), vec3(7,1,0));
		rightramp->SetScale(vec3(4,0.1,2));
		rightramp->SetRotation(vec3(0,0,radians(25.0f)));
		scene->AddEntity(rightramp);
		rightramp->SetStatic();

		// ball
		RigidBodyModel* myCylinder = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(.6,.4,.8)), vec3(8,3.8,0));
		myCylinder->SetScale(vec3(0.5));
		myCylinder->SetRotation(vec3(radians(90.0f),0,radians(90.0f)));
		scene->AddEntity(myCylinder);

		// add floor
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 8.0f, 8.0f), vec3(0.0f,1.0f,0.0f),-90.0f,-45.0f);
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}
	
	void createDominoScene()
	{
		scene->Clear();

		// add dominos
		int n = 100;
		for (int i=0; i<n; ++i)
		{
			double k = 2 + (double)i/(double)n;
			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.08*k,.17*k,.25*k)), vec3(4-i*0.6,0.5,0));
			stone->SetScale(vec3(0.2,1,0.5));
			scene->AddEntity(stone);
		}

		// add ramp
		RigidBodyModel* rightramp = new RigidBodyModel(MeshGenerator::CreateBox(), vec3(7,1,0));
		rightramp->SetScale(vec3(4,0.1,2));
		rightramp->SetRotation(vec3(0,0,radians(25.0f)));
		scene->AddEntity(rightramp);
		rightramp->SetStatic();

		// wheel
		RigidBodyModel* myCylinder = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(.6,.4,.8)), vec3(8,3.8,0));
		myCylinder->SetScale(vec3(1));
		//myCylinder->GetRigidBody()->SetInverseMas(1./3.);
		myCylinder->SetRotation(vec3(radians(90.0f),0,radians(90.0f)));
		scene->AddEntity(myCylinder);

		// add floor
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(650,10,10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 1.5f, 8.0f), vec3(0,1,0));
		scene->SetCamera(camera);

		cameraInput->SetCamera(scene->GetCamera());
	}

	/*
	 * Test scene for model instancing and lighting
	 */
	void createHalfpipeTestScene()
	{
		scene->Clear();

		for (int i=0; i<=5; i++)
		 {
			vec3 midpoint(i*0.5,8,5);
			double L = 3;
			Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
			center->SetScale(vec3(0.05));
			scene->AddEntity(center);

			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0,-L,0));
			stone->SetScale(vec3(0.25));
//			stone->GetRigidBody()->ApplyLinearMomentum(dvec3(4.4,0,0)); // push it out of equilibrium
			scene->AddEntity(stone);

			DistanceConstraint* dst = new DistanceConstraint(stone->GetRigidBody(), midpoint);
			scene->GetPhysicManager()->AddConstraint(dst);
		}

		 {
			vec3 midpoint(6*0.5,8,5);
			double L = 3;
			Model* center = new Model(MeshGenerator::CreateSphere(vec3(1,0,0)), midpoint);
			center->SetScale(vec3(0.05));
			scene->AddEntity(center);

			RigidBodyModel* stone = new RigidBodyModel(MeshGenerator::CreateSphere(), midpoint + vec3(0,-L,0));
			stone->SetScale(vec3(0.25));
			stone->GetRigidBody()->ApplyLinearMomentum(dvec3(2,0,0)); // push it out of equilibrium
			scene->AddEntity(stone);

			DistanceConstraint* dst = new DistanceConstraint(stone->GetRigidBody(), midpoint);
			scene->GetPhysicManager()->AddConstraint(dst);
		}




		// hinge
		for (int i=0; i<=7; i++)
		{
			vec3 position(0.295*i,6,0);
			RigidBodyModel* box1;
			box1 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position);
			box1->SetScale(vec3(0.1,0.1,0.4));
			box1->SetRotation(radians(vec3(0,0,90)));
			box1->SetStatic();
			scene->AddEntity(box1);
			
			RigidBodyModel* ball;
			ball = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position + vec3(0,-2,0));
			ball->SetScale(vec3(0.3));
			scene->AddEntity(ball);
			
			HingeConstraint* dst = new HingeConstraint(box1->GetRigidBody(), ball->GetRigidBody(), vec3(0,0,1), position - vec3(0,.1,0));
			scene->GetPhysicManager()->AddConstraint(dst);
		}

		{
			vec3 position(0.3*8,6,0);
			RigidBodyModel* box1;
			box1 = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position);
			box1->SetScale(vec3(0.1,0.1,0.4));
			box1->SetRotation(radians(vec3(0,0,90)));
			box1->SetStatic();
			scene->AddEntity(box1);
			
			RigidBodyModel* ball;
			ball = new RigidBodyModel(MeshGenerator::CreateBox(vec3(0,1,1)), position + vec3(2*sin(M_PI/3),-2*cos(M_PI/3),0));
			ball->SetScale(vec3(0.3));
			ball->SetRotation(vec3(0,0,radians(60.0f)));
			scene->AddEntity(ball);
			
			HingeConstraint* dst = new HingeConstraint(box1->GetRigidBody(), ball->GetRigidBody(), vec3(0,0,1), position - vec3(0,.1,0));
			scene->GetPhysicManager()->AddConstraint(dst);
		}



		double a=2;
		// initializing random generator. Uniform distribution [0,a]. For the moment
		std::mt19937 gen(13);
		std::uniform_real_distribution<double> distribution(-a,a);


		// ramp with incline borders
		Entity* incline = Combos::LaneIncline();
		scene->AddEntity(incline);


		for (int i=0; i<20; i++) {
			// ball
			RigidBodyModel* ball = new RigidBodyModel(MeshGenerator::CreateSphere(vec3(.0+double(i)/20,.0-(double(i)/20),.3), 18), vec3(distribution(gen),3.,distribution(gen)));
			ball->SetScale(vec3(0.1));
			ball->SetRotation(vec3(radians(90.0f),0,radians(90.0f)));
			scene->AddEntity(ball);
		}


		// add plane
		RigidBodyModel* plane = new RigidBodyModel(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(10));
		scene->AddEntity(plane);
		plane->SetStatic();

		// add light
		Light* light = new Light(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 8.0f, 8.0f), vec3(0.0f,1.0f,0.0f),-90.0f,-45.0f);
		scene->SetCamera(camera);
		cameraInput->SetCamera(scene->GetCamera());
	}



	/*
	 * Test scene for model instancing and lighting
	 */
	void createRenderTestScene()
	{
		scene->Clear();
		Model* box = NULL;

		int size = 22;
		for (int i=0; i<size; ++i)
		{
			for (int j=0; j<size; ++j)
			{
				for (int k=0; k<size; ++k)
				{
					if (box == NULL)
					{
						box = new Model(MeshGenerator::CreateBox(), vec3(i,0.5 + j,k));
						box->SetScale(vec3(0.3));
						scene->AddEntity(box);
					}
					else
					{
						Model* instance = box->CreateInstance(vec3(i, 0.5 + j, k));
						scene->AddEntity(instance);
					}
				}
			}
		}


		// add plane
		Model* plane = new Model(MeshGenerator::CreatePlane(), vec3(0,0,0));
		plane->SetScale(vec3(20));
		scene->AddEntity(plane);

		// add light
		Light* light = new MovingLight(vec3(5,6,-5), vec3(0));
		scene->SetLight(light);

		// add camera
		Camera* camera = new Camera(vec3(0.0f, 1.0f, 3.0f));
		scene->SetCamera(camera);
		cameraInput->SetCamera(scene->GetCamera());
	}
};
