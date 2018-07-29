#pragma once

#include "constraint/Constraint.h"

/* 
 * Enforces the distance between a rigidBody and a point
 * similar to (slide 25): http://twvideo01.ubm-us.net/o1/vault/gdc09/slides/04-GDC09_Catto_Erin_Solver.pdf
 */
class SpringConstraint : public Constraint
{

public:

	dvec3 p;
	RigidBody* body;
	double L;

	SpringConstraint(RigidBody* body, dvec3 point)
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
		//double b = 0.5* std::pow(length(body->position - p) - L, 2);
		
		double diff = length(body->position - p) - L;
		double b = 0.5* std::pow(diff, 2);

		//
		//std::cout << b << std::endl;

		// effective mass
		double effectiveMass = 1./(body->inverseMass);

		double C= 0.1;

		// solve (dot(J,V)+b)
		double deltaV =  C*b ;
		double lambda = -effectiveMass * deltaV;

		dvec3 force = J*lambda;

		body->ApplyLinearMomentum(force);
	}

};
