/*
 * Runs simulation of rigidbodies (should be independent of everything non physic related)
 */
#pragma
#include <assert.h>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <vector>
#include <list>
#include <unordered_map>
#include <functional>
#include <iostream>

#include "RigidBody.h"
#include "InactivityDetector.h"
#include "DebugRenderer.h"
#include "collision/CollisionDetector.h"
#include "constraint/ConstraintSolver.h"
#include "constraint/Constraint.h"
#include "constraint/DistanceConstraint.h"
#include "constraint/TwoBodyDistanceConstraint.h"
#include "constraint/SoftTwoBodyDistanceConstraint.h"
#include "constraint/ContactConstraint.h"
#include "constraint/HingeConstraint.h"
#include "constraint/BallJointConstraint.h"
#include "constraint/SoftDistanceConstraint.h"

//#define TIMING
#ifdef TIMING
#include "timer.h"
#endif


#define GRAVITY 0.9
#define CONSTRAINTSOLVINGITERATIONS 4
#define TIMPESTEPDIVIDER 4
#define SPEEDUP 2

class PhysicManager
{

private:
	std::vector<RigidBody*> bodies;
	bool running = true;

	int timestepDivider = TIMPESTEPDIVIDER;
	int speedup = SPEEDUP;

	InactivityDetector* inactivityDetector;
	CollisionDetector* collisionDetector;
	ConstraintSolver* constraintSolver;
	
public: 
	
	void SetSpeedup(int i) { speedup = i; }
	void SetTimestepDivider(int i) { timestepDivider = i; }
	void SetConstraintSolvingInterations(int i) { constraintSolver->SetIterations(i); }

	PhysicManager()
	{
		bodies.clear();
		inactivityDetector = new InactivityDetector();
		constraintSolver = new ConstraintSolver();
		collisionDetector = new SweepAndPruneCollisionDetector(inactivityDetector);
		//collisionDetector = new NaiveCollisionDetector(inactivityDetector);
		//collisionDetector = new SpatialPartitioningCollisionDetector(inactivityDetector);
		
		constraintSolver->SetIterations(CONSTRAINTSOLVINGITERATIONS);
	}

	~PhysicManager()
	{
		delete collisionDetector;
		delete constraintSolver;
		delete inactivityDetector;
	}

	bool IsRunning()
	{
		return running;
	}

	void AddConstraint(Constraint* c)
	{
		constraintSolver->AddConstraint(c);
	}

	void AddBody(RigidBody* body)
	{
		this->bodies.push_back(body);
		collisionDetector->AddBody(body);
	}

	int CountBodies()
	{
		return bodies.size();
	}

	void Clear()
	{
		bodies.clear();
		RigidBody::ResetCounter();
		collisionDetector->Clear();
		inactivityDetector->Clear();
		constraintSolver->Clear();

		// reset previous values to default values
		constraintSolver->SetIterations(CONSTRAINTSOLVINGITERATIONS);
		timestepDivider = TIMPESTEPDIVIDER;
		speedup = SPEEDUP;
	}

	void Stabilize(GLfloat T)
	{
		std::cout << "stabilize start" << std::endl;
		int constraintSolvingIterationsBackup = constraintSolver->GetIterations();
		int timestepDividerBackup = timestepDivider;
		int speedupBackup = speedup;
		bool runningBackup = running;

		running = true;
		speedup = 1;
		constraintSolver->SetIterations(100);
		timestepDivider = T*220;
		Update(T);

		running = runningBackup;
		constraintSolver->SetIterations(constraintSolvingIterationsBackup);
		timestepDivider = timestepDividerBackup;
		speedupBackup = speedup;
		
		std::cout << "stabilize finish" << std::endl;
	}

	void Update(GLfloat T)
	{
		if (!running)
		{
			drawDebugInformation();
			return;
		}

		T = T*speedup;

		double h = T / (double)timestepDivider;
		double t = 0;

		int n = bodies.size();
		if (n == 0) return;

		#ifdef TIMING
		Timer t1, t2, t3, t4, t5;
		#endif

		while (t < T)
		{
			#ifdef TIMING
			t1.start();
			#endif
			integrateEulerAtCurrentState(h); // wolftho: I think this is equivalent to having the to seperate integrations, thomaset: that's true as indeed..., as long the velocity is integrated first
			#ifdef TIMING
			t1.stop();
			t2.start();
			#endif
			calculateExternalForcesAndTorque(h);
			#ifdef TIMING
			t2.stop();
			t3.start();
			#endif
			collisionDetector->FindCollisions();
			#ifdef TIMING
			t3.stop();
			t4.start();
			#endif
			constraintSolver->Solve(h, collisionDetector->activeContactManifolds);
			#ifdef TIMING
			t4.stop();
			#endif

			t += h;
		}

		#ifdef TIMING
		t5.start();
		#endif
		inactivityDetector->Update(T, bodies);
		#ifdef TIMING
		t5.stop();
		#endif
		
		#ifdef TIMING
		std::cout << std::setprecision(6) << std::fixed;
		std::cout << "Timing velocity integrator:    " << t1.mean() << std::endl;
		std::cout << "Timing ext forces:             " << t2.mean() << std::endl;
		std::cout << "Timing find constacts:         " << t3.mean() << std::endl;
		std::cout << "Timing resolve constraints:    " << t4.mean() << std::endl;
		std::cout << "Timing inactivity detector:    " << t5.mean() << std::endl;
		std::cout << std::endl;
		#endif
		
		drawDebugInformation();
	}

	void Stop() { running = false; }
	void Start() { running = true; }
	
	void PrintAll()
	{
		PrintAllForces();
		PrintAllLinearMomentums();
		PrintAllVelocities();
	}
	
	void PrintAllForces(){
		for (RigidBody *b : bodies)
		{
			// force output
			b->PrintForce();
		}
	}
	void PrintAllLinearMomentums() const
	{
		for (RigidBody *b : bodies)
		{
			// linear momentum output
			b->PrintLinearMomentum();
		}
	}
	void PrintAllVelocities() const
	{
		for (RigidBody *b : bodies)
		{
			// velocity output
			b->PrintVelocity();
		}
	}
	
	void PrintContactManifolds()
	{
		for (std::pair<const std::pair<int,int>, ContactManifold*>& i : collisionDetector->activeContactManifolds)
		{
			i.second->PrintContacts();
		}
	}
private:

	void integrateEulerAtCurrentState(double h)
	{
		int n = bodies.size();
		#pragma omp parallel for
		for (int i=0; i<n; ++i)
		{
			RigidBody* b = bodies[i];
			b->IntegrationStep(h);	
		}
	}
	void integrateVelocitiesAtCurrentState(double h)
	{
		int n = bodies.size();
		#pragma omp parallel for
		for (int i=0; i<n; ++i)
		{
			RigidBody* b = bodies[i];
			b->IntegrationStepVelocities(h);	
		}
	}
	void integratePositionsAtCurrentState(double h)
	{
		int n = bodies.size();
		#pragma omp parallel for
		for (int i=0; i<n; ++i)
		{
			RigidBody* b = bodies[i];
			b->IntegrationStepPositions(h);	
		}
	}

	void calculateExternalForcesAndTorque(double dt)
	{
		int n = bodies.size();

		#pragma omp parallel for
		for (int i=0; i<n; ++i)
		{
			RigidBody* b = bodies[i];
			b->force = dvec3(0,-GRAVITY, 0);
			b->torque = dvec3(0);
		}
	}

	double getStabilityAverage()
	{
		double v = 0;
		for (RigidBody* b: bodies)
		{
			v += std::pow(b->changeAverage, 2);
		}

		return std::sqrt(v);
	}

	void drawDebugInformation()
	{
		for (std::pair<const std::pair<int,int>, ContactManifold*>& i : collisionDetector->activeContactManifolds)
		{
			for (Contact* c : i.second->contacts)
			{
				c->Update();

				dvec3 color(0,0,1);
				if (c->type == ContactType::Colliding) color = dvec3(1,0,0);

				int size = 15;
				DebugRenderer::Instance()->AddDebugPoint(c->location, color, size);
				DebugRenderer::Instance()->AddDebugPoint(c->locationB, color, size);
			}
		}

		for (RigidBody* a : bodies)
		{
			vec3 color(1,0,0);
			if (a->sleeping)
			{
				color = vec3(0,1,0);
			}
			if (a->inactive)
			{
				color = vec3(0,0,1);
			}
			DebugRenderer::Instance()->AddDebugBox(a->aabb.GetPosition(), color, a->aabb.GetScale());
		}
	}

};
