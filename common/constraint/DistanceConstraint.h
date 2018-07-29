#pragma once

#include "constraint/Constraint.h"

/* 
 * Enforces the distance between a rigidBody and a point
 * similar to (slide 25): http://twvideo01.ubm-us.net/o1/vault/gdc09/slides/04-GDC09_Catto_Erin_Solver.pdf
 */
class DistanceConstraint : public Constraint
{

public:

	dvec3 p;
	RigidBody* body;
	double L;

	DistanceConstraint(RigidBody* body, dvec3 point)
	{
		this->p = point;
		this->body = body;
		this->L = length(body->position - p); // save initial distance
		this->body->SetSleepingEnabled(false);
	}

	virtual void Solve(double dt)
	{
		// get V
		// (vA)
		dvec3 vA = body->velocity;

		// create J:
		// (x/norm(x))
		dvec3 J = normalize(body->position-p);

		// correct distance
		double b = length(body->position - p) - L;	
		// thomaset: from where do we get this factor b, it's not time dependent?!, according to slide 25: b = 0
		// wolftho: not in the slides, but this keeps the distance (like baumgarten)

		// effective mass
		double effectiveMass = 1./(body->inverseMass);


		// solve (dot(J,V)+b)
		double deltaV = dot(vA, J) + b;
		double lambda = -effectiveMass * deltaV;

		dvec3 force = J*lambda;

		body->ApplyLinearMomentum(force);
	}

};
