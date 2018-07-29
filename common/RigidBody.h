#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/component_wise.hpp>
#include <cassert>
using namespace glm;

#include <list>
#include <unordered_map>
#include <iomanip>

#include "Model.h"
#include "Shape.h"
#include "timer.h"
#include "limits.h"
#include "AABB.h"
#include "collision/Contact.h"
#include "collision/GJKSimplex.h"
#include "collision/EPAPolytope.h"
#include "collision/ContactManifold.h"


/*
 * Represents an abstract object that is physically simulated by the PhysicManager
 */
class RigidBody 
{
	// for faster prototyping and direct memory access
	friend class PhysicManager; 
	friend class Contact; 
	friend class CollisionDetector; 
	friend class ConstraintSolver; 
	friend class SweepAndPruneCollisionDetector; 
	friend class NaiveCollisionDetector; 
	friend class SpatialPartitioningCollisionDetector; 
	friend class InactivityDetector; 
	friend class ContactConstraint; 
	friend class DistanceConstraint; 
	friend class BodyDistanceConstraint; 
	friend class TwoBodyDistanceConstraint;
	friend class SoftTwoBodyDistanceConstraint;
	friend class HingeConstraint;
	friend class BallJointConstraint;
	friend class SoftDistanceConstraint;
	friend class SpringConstraint;

private:
		static int idCounter;
		int id; // gives rigidBodies an order

		bool isDirty = true; // model matrix M is not up to date
		dmat4 M;

		// shape
		Shape* shape;			
		AABB aabb;

		// constants
		double inverseMass;
		dmat3 inertiaTensorBodyInverse;
		dvec3 scale; 

		// state
		dvec3 position; 
		dquat rotation;
		dvec3 linearMomentum;  // Impuls		
		dvec3 angularMomentum; // Drehimpuls (L)
		dmat3 inertiaTensorInverse;

		// rate of change
		dvec3 velocity; 
		dvec3 angularVelocity;  // omega
		dvec3 force;  // Kraft
		dvec3 torque; // Drehmoment (M)
		
		// static flag for physics
		bool isStatic = false;

		double friction = 0.5; // between 0 and 1 (1 means highest friction; will be mutiplied with friction of other body during contact)
		double restitution = 0.7; // betwee 0 and 1 (multiplied with restitution of other body during contact)

		// sleeping
		bool enableSleeping = true;
		bool sleeping = false;
		double changeAverageN = 10./120.;
		double sleepThreshold = 0.1;
		double changeAverage = 1000; // dont enable sleeping for the first cycles

		bool inactive = false;
		int inactiveSetId;
		bool forceWakeup = false;
		bool grounded = false;

		std::unordered_map<int, ContactManifold*> manifolds;

	public:

		// wake up for at least a round to check if we really want to wak up
		void RevalidateSleeping()
		{
			if (!isStatic)
			{
				this->forceWakeup = true;
				this->changeAverage = 0.2;
			}
		}

		void PrintForce(){
			std::cout << "  force on body with id " << id << ": (" << force[0] << "," << force[1] << "," << force[2] << ")  isStatic = " << isStatic << "\n";
			//~ std::cout << "  torque on body with id " << id << ": (" << torque[0] << "," << torque[1] << "," << torque[2] << ")  isStatic = " << isStatic << "\n";
		}
		void PrintLinearMomentum(){
			std::cout << "  linear momentum of body with id " << id << ": (" << linearMomentum[0] << "," << linearMomentum[1] << "," << linearMomentum[2] << ")  isStatic = " << isStatic << "\n";
		}
		void PrintVelocity(){
			std::cout << "  velocity of body with id " << id << ": (" << velocity[0] << "," << velocity[1] << "," << velocity[2] << ")  isStatic = " << isStatic << "\n";
		}
	
	
		RigidBody(dvec3 pos, Shape* shape)
		{
			assert(shape != NULL);

			this->position = pos;
			this->shape = shape;
			
			this->rotation = dquat(dvec3(0,0,0));
			this->velocity = dvec3(0,0,0);
			this->linearMomentum = dvec3(0,0,0);
			this->angularMomentum = dvec3(0,0,0);

			this->angularVelocity = dvec3(0,0,0);
			this->force = dvec3(0,0,0);
			this->torque = dvec3(0,0,0);

			// derived 
			double mass = 1.0f;
			this->inverseMass = 1. / mass;

			dmat3 inertiaTensorBody = dmat3(1);
			this->inertiaTensorBodyInverse = inverse(inertiaTensorBody);
			this->inertiaTensorInverse = inertiaTensorBodyInverse;
			

			this->id = idCounter++;

			UpdateDerivedState();
			UpdateInertiaTensorBody();
		}


		~RigidBody()
		{
			delete shape;
		}

		static void ResetCounter() { idCounter = 0; }
		void SetAngularVelocity(const dvec3 vel) { this->angularVelocity = vel; }
		void SetInertiaTensorBody(const dmat3 t) { this->inertiaTensorBodyInverse = inverse(t); }
		void SetInertiaTensorBodyInverse(const dmat3 t) { this->inertiaTensorBodyInverse = t; }
		const dmat3 GetInertiaTensorInverse() { return this->inertiaTensorInverse; }

		double GetInverseMass() { return this->inverseMass; }
		void SetInverseMass(double mass) { this->inverseMass = mass; }
		void SetMass(double mass) { assert(mass != 0); this->inverseMass = 1./mass; }

		void SetFriction(const double friction) { this->friction = friction; }
		const double GetFriction() { return this->friction; }

		void SetRestitution(const double restitution) { this->restitution = restitution; }
		const double GetRestitution() { return this->restitution; }

		void SetScale(const dvec3 scale) { isDirty = true; this->scale = scale; UpdateAABB(); UpdateInertiaTensorBody(); }
		const dvec3 GetScale() { return this->scale; }

		void SetPosition(const dvec3 pos) { isDirty = true; this->position = pos; UpdateAABB(); }
		const dvec3 GetPosition() { return this->position; }

		void SetRotation(const dquat r) { isDirty = true; this->rotation = r; UpdateAABB(); }
		const dquat GetRotation() { return this->rotation; }

		void SetSleepingEnabled(bool en) { this->enableSleeping = en; }

		const AABB GetAABB() { return this->aabb; }

		const int GetId() { return this->id; }

		void UpdateInertiaTensorBody()
		{
			if (!isStatic) SetInertiaTensorBody(shape->GetInertiaTensor(1./inverseMass, scale));
		}

		void SetStatic()
		{
			// see D49
			this->inverseMass = 0;
			this->isStatic = true;
			this->sleeping = true;
			this->inertiaTensorInverse = dmat3(0);
			this->inertiaTensorBodyInverse = dmat3(0);
			UpdateAABB(); // we calculate the aabb only once !
		}
		
		bool IsStatic()
		{
			return this->isStatic;
		}


		// simple euler integration
		void IntegrationStep(double dt)
		{
			if (isStatic) return;
			if (inactive) return;
	
			if (enableSleeping && !forceWakeup)
			{
				if (changeAverage < sleepThreshold && length(linearMomentum) < sleepThreshold && length(angularMomentum) < sleepThreshold)
				{
					sleeping = true;

					// artificial damping increases stability
					linearMomentum *= 0.7;
					angularMomentum *= 0.4;
				}
				else if (sleeping)
				{
					sleeping = false;
				}
			}
			
			if (!sleeping || forceWakeup)
			{

				// integrate position
				position += dt*velocity;

				rotation += dquat(0, 0.5*dt*angularVelocity.x, 0.5*dt*angularVelocity.y, 0.5*dt*angularVelocity.z) * rotation;
				rotation = normalize(rotation);
				dmat3 R = glm::mat3_cast(rotation);
				inertiaTensorInverse = R * inertiaTensorBodyInverse * transpose(R);
				isDirty = true;

				// integrate velocity
				angularMomentum += dt* torque;
				linearMomentum += dt*force;
				velocity = linearMomentum * inverseMass;
				angularVelocity = inertiaTensorInverse * angularMomentum;

				// update bounding box
				UpdateAABB();
			}


			// update sleep params
			changeAverage = (changeAverageN/dt * changeAverage + length(velocity) + length(angularVelocity)) / (changeAverageN/dt + 1);
			forceWakeup = false;
		}

		// integration step 1st part, integrate velocities
		void IntegrationStepVelocities(double dt)
		{
			if (isStatic) return;
	
			/// TODO: is this needed in velocities intgrator???
			if (enableSleeping && changeAverage < sleepThreshold && length(linearMomentum) < sleepThreshold && length(angularMomentum) < sleepThreshold)
			{
				linearMomentum *= 0.7;
				angularMomentum *= 0.4;
				return;
			}
			

			changeAverage = (changeAverageN * changeAverage + length(velocity) + length(angularVelocity)) / (changeAverageN + 1);

			linearMomentum += dt*force;

			angularMomentum += dt* torque;

			velocity = linearMomentum * inverseMass;
			angularVelocity = inertiaTensorInverse * angularMomentum;
		}

		// integration step 2nd part, integrate positions
		void IntegrationStepPositions(double dt)
		{
			isDirty = true;

			if (isStatic) return;
	
			/// TODO: is this needed in velocities intgrator???
			if (enableSleeping)
			{
				if (changeAverage < sleepThreshold && length(linearMomentum) < sleepThreshold && length(angularMomentum) < sleepThreshold)
				{
					sleeping = true;
					return;
				}
				else if (sleeping)
				{
					sleeping = false;
				}
			}
			changeAverage = (changeAverageN * changeAverage + length(velocity) + length(angularVelocity)) / (changeAverageN + 1);

			position += dt*velocity;

			rotation += dquat(0, 0.5*dt*angularVelocity.x, 0.5*dt*angularVelocity.y, 0.5*dt*angularVelocity.z) * rotation;
			rotation = normalize(rotation);
			dmat3 R = glm::mat3_cast(rotation);
			inertiaTensorInverse = R * inertiaTensorBodyInverse * transpose(R);

			UpdateAABB();
		}

		inline double GetEffectiveMassInverse(const dvec3 J1, const dvec3 J2)
		{
			return inverseMass * dot(J1, J1) + dot(J2, inertiaTensorInverse * J2);
		}

		// coupled effective mass matrix for constraints
		// also used for hinge
		// J is a 12x2 matrix J=(J1, J2) Ji = (JiUpper, JiLower)'
		inline dmat2 GetEffectiveMassInverse(const dvec3 J1Upper,const dvec3 J1Lower,const dvec3 J2Upper,const dvec3 J2Lower)
		{
			double m11 = inverseMass * dot(J1Upper, J1Upper) + dot(J1Lower, inertiaTensorInverse * J1Lower);
			double m22 = inverseMass * dot(J2Upper, J2Upper) + dot(J2Lower, inertiaTensorInverse * J2Lower);
			double m12 = inverseMass * dot(J1Upper, J2Upper) + dot(J1Lower, inertiaTensorInverse * J2Lower);

			dmat2 Mass(m11,m12,m12,m22);
			return Mass;
		}

		// get 3x3 mass matrix, K=J_trans*M^-1*J_trans'
		// used for hinge
		// formula (48) http://danielchappuis.ch/download/ConstraintsDerivationRigidBody3D.pdf
		inline dmat3 GetEffectiveMassInverse(const dmat3& J1, const dmat3& J2)
		{
			return inverseMass*J1*transpose(J1) + J2*inertiaTensorInverse*transpose(J2);
		}

		void UpdateDerivedState()
		{
			velocity = linearMomentum * inverseMass;
			dmat3 R = glm::mat3_cast(rotation);
			inertiaTensorInverse = R * inertiaTensorBodyInverse * transpose(R);
			angularVelocity = inertiaTensorInverse * angularMomentum;
		}
		
		inline void ApplyLinearMomentum(const dvec3 p)
		{
			if (isStatic) return;
			this->linearMomentum += p;
			this->velocity = this->linearMomentum * this->inverseMass;
		}

		inline void ApplyAngularMomentum(const dvec3 p)
		{
			if (isStatic) return;
			this->angularMomentum += p;
			this->angularVelocity = this->inertiaTensorInverse * this->angularMomentum;
		}

		void ApplyForce(dvec3 force)
		{
			this->force += force;
		}

		void ApplyForce(dvec3 force, dvec3 pos)
		{
			this->force += force;
			ApplyTorque(cross(pos - this->position, force));
		}

		void ApplyTorque(dvec3 torque)
		{
			this->torque += torque;
		}
		
		//! see ~baraff/sigcourse/notesd2.pdf, D41 equation (8-1)
		// thomaset: checked
		dvec3 GetPointVelocity(dvec3 p)
		{
			return this->velocity + cross(this->angularVelocity, p - this->position);
		}

		dmat4 GetModelMatrix()
		{
			if (!this->isDirty)
			{
				return M;
			}

			M = dmat4(1.0f); // model matrix to store translation and rotation

			// apply translation
			M = glm::translate(M, this->position);

			// rotation matrix
			dmat4 R = glm::mat4_cast(rotation);
			M = M*R; // apply rotation

			// apply scale
			M = glm::scale(M, this->scale);

			isDirty = false;
			return M;
		}

		const dvec3 LocalToGlobal(const dvec3 p)
		{
			dvec4 s(p, 1.0);
			s = GetModelMatrix()*s;
			return dvec3(s.x,s.y,s.z);
		}

		const dvec3 GlobalToLocal(const dvec3 p)
		{
			dvec4 s(p, 1.0);
			s = inverse(GetModelMatrix())*s;
			return dvec3(s.x, s.y, s.z);
		}


		// Contact related stuff
		//

		// returns the point with the highest dot product with p (needed for GJK and EPA algorithm)
		dvec3 GetSupport(dvec3 p)
		{
			// reverse model space rotation -> p is now a local direction
			p = transpose(mat3_cast(rotation))*p;
			
			// transform support to world coordinates
			return LocalToGlobal(shape->GetSupport(p));
		}

		MinowskiPoint GetMinowskiSupport(dvec3 D, RigidBody* B)
		{
			dvec3 s1 = GetSupport(D);
			dvec3 s2 = B->GetSupport(-D);
			return MinowskiPoint(s1 - s2, s1);
		}

		bool ComputeContactManifold(ContactManifold* cm)
		{
			// calculate new contact for this frame


			cm->UpdatePersistence();

			Contact* c = IntersectsWith(cm->bodyB);

			if (c != NULL)
			{
				cm->AddContact(c);
				return true;
			}
			else return false;


			/*std::cout << std::setprecision(6) << std::fixed;
			std::cout << "Timing update cm:             " << t1.mean() << std::endl;
			std::cout << "Timing inters:    " << t2.mean() << std::endl;
			std::cout << "Timing  add to cm:    " << t3.mean() << std::endl;
			std::cout << std::endl;*/
		}


		// GJK algorithm to check if intersection occurs:
		// https://en.wikipedia.org/wiki/Gilbert%E2%80%93Johnson%E2%80%93Keerthi_distance_algorithm
		Contact* IntersectsWith(RigidBody* B)
		{
			GJKSimplex s;
			dvec3 D(1,1,1); // start with some arbitrary direction
		
			MinowskiPoint wk = GetMinowskiSupport(D, B);
			s.PushVertex(wk);
			D = -wk.p;

			int maxIterations = 20;
			while(maxIterations-- > 0)
			{
				wk = GetMinowskiSupport(D, B);

				if (dot(wk.p, D) < 0)
				{
					return NULL;
				}

				//assert(dot(wk.p, D) != 0);

				s.PushVertex(wk);

				if (s.HasOriginInside(D))
				{
					return computeContact(s, B);
				}

				assert(dot(D,D) != 0);
			}

			if (maxIterations < 0) std::cout << "GJK did not converge" << std::endl;

			return NULL;
		}
	
		// EPA algorithm calculates penetration depth, location and position
		Contact* computeContact(GJKSimplex& s, RigidBody* B)
		{
			EPAPolytope p = s.ConvertToEPAPolytope();

			dvec3 closestToOrigin(FLT_MAX);
			double delta = 1;

			int maxIterations = 10000;
			while (maxIterations-- > 0)
			{

				// find closest face to origin of the polytope
				MinowskiTriangle *f;
				double depth;
				p.ClosestFaceToOrigin(depth, &f);
				dvec3 normal = f->normal;

				// get support point from the normal direction of the closest face
				MinowskiPoint nextPoint = GetMinowskiSupport(normal, B);

				delta = std::abs(dot(nextPoint.p - f->a.p, normal)); // check if nextPoint is in on the current closest triangle
				if (delta > 0.001)
				{
					// extend polytope by the support point
					p.AddPoint(nextPoint);
				}
				else
				{
					// create contact object
					Contact* c = ContactPool::GetInstance().Get();
					c->SetData(this, B, normal, f->InterpolateContact(), depth);
					
					return c;
				}
			}

			assert(false && "EPA did not converge");
		}

		// transforms the aabb of the shape to world coordinates
		inline void UpdateAABB()
		{
			aabb.Set(shape->GetAABB()); // reset to untransformed
			aabb.Transform(GetModelMatrix());
		}
};
int RigidBody::idCounter = 0;


// defined here because of dependency of rigidbody :/
void Contact::Update()
{
	vA = bodyA->GetPointVelocity(location);
	vB = bodyB->GetPointVelocity(location);

	vRel = dot(normal, vA - vB);	//! see ~baraff/sigcourse/notesd2.pdf, D42 equation (8-3)

	// this threshold makes the simulation comparably very very stable
	if (vRel > COLLISION_THRESHOLD) type = ContactType::Diverging; // moving away
	else type = ContactType::Colliding; // collision
}

void Contact::SetData(RigidBody* a, RigidBody* b, dvec3 normal, dvec3 loc, double depth)
{
	this->depth = depth;

	this->location = loc;
	this->locationB = location - normal * depth;

	this->localLocation = a->GlobalToLocal(location);
	this->localLocationB = b->GlobalToLocal(locationB);

	this->bodyA = a;
	this->bodyB = b;

	SetNormal(-normal);
	Update();
	ClearConstraint();
}

void Contact::PrintContact(){
	//std::cout << "  Contact between bodies: " << bodyA->GetId() << " and " << bodyB->GetId() << " localPos: " << "(" << localLocation[0] << ", " << localLocation[1] << ", " << localLocation[2] << "), resting = " << (type == ContactType::Resting) << std::endl;
	std::cout << "  Contact between bodies: " << bodyA->GetId() << " and " << bodyB->GetId() << " pos: " << "(" << location[0] << ", " << location[1] << ", " << location[2] << std::endl;
}

// checks if contacts did not move to far and are still colliding
void ContactManifold::UpdatePersistence()
{
	std::list<Contact*>::iterator i = contacts.begin();
	while (i != contacts.end())
	{
		Contact* c = (*i);

		dvec3 newLocA = bodyA->LocalToGlobal(c->localLocation);
		dvec3 newLocB = bodyB->LocalToGlobal(c->localLocationB);
		dvec3 diffAB = newLocB - newLocA;

		// how much did we move from original collision position?
		dvec3 diffLocA = c->location - newLocA;
		dvec3 diffLocB = c->locationB - newLocB;

		bool penetrating = dot(c->normal, diffAB) >= 0;
		
		bool diffLocASmallEnough = length2(diffLocA) < PERSISTANCE_THRESHOLD*PERSISTANCE_THRESHOLD;
		bool diffLocBSmallEnough = length2(diffLocB) < PERSISTANCE_THRESHOLD*PERSISTANCE_THRESHOLD;

		//std::cout << dot(c.normal, diffAB) << " " << penetrating << " " << to_string(diffLocA) << " " << to_string(diffLocB) << std::endl; 

		//TODO: find good heuristic
		if (diffLocASmallEnough && diffLocBSmallEnough && penetrating)
		{
			++i;
		}
		else
		{
			i = contacts.erase(i);
			ContactPool::GetInstance().Recycle(c);
		}
	}
}

