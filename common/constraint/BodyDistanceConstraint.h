#pragma once

#include "constraint/Constraint.h"

/* 
 * Enforces the distance between a rigidBody and a point
 * similar to (slide 25): http://twvideo01.ubm-us.net/o1/vault/gdc09/slides/04-GDC09_Catto_Erin_Solver.pdf
 */
class BodyDistanceConstraint : public Constraint
{

public:

	RigidBody* bodyA;
	RigidBody* bodyB;
	double L;

	BodyDistanceConstraint(RigidBody* bodyA, RigidBody* bodyB)
	{
		this->bodyA = bodyA;
		this->bodyB = bodyB;
		this->L = length(bodyB->position - bodyA->position); // save initial distance
	}

	virtual void Solve(double dt)
	{
		dvec3 dst = bodyB->position - bodyA->position;

		// get V
		// (vA, vB)
		dvec3 vA = bodyA->velocity;
		dvec3 vB = bodyB->velocity;

		// create J:
		// (dir,  -dir)
		dvec3 dir = normalize(dst);

		// b to correct the distance
		double b = -length(dst) + L;	/// TODO: thomaset: why the minus??? shouldn't it be b = lentgth(x) - L ?? baumgarte stabilization adds beta/dt * C

		// create effectiveMass = 1/(transpose(J)*MInverse*J)
		double effectiveMass = 1./(bodyA->inverseMass + bodyB->inverseMass);

		// solve (dot(J,V)+b)
		double deltaV = dot(vA, dir) - dot(vB, dir) + b;
		double lambda = -effectiveMass * deltaV;

		dvec3 force = dir*lambda;

		bodyA->ApplyLinearMomentum(force);
		bodyB->ApplyLinearMomentum(-force);
	}

};
