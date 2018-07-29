#pragma once

#include "constraint/Constraint.h"

/* 
 * Enforces the distance between two rigidBody 
 * similar to p6: http://www.bulletphysics.com/ftp/pub/test/physics/papers/IterativeDynamics.pdf
 */
class SoftTwoBodyDistanceConstraint : public Constraint
{
public:
	/// introduce offset from body in local coordinates
	RigidBody* bodyA;
	RigidBody* bodyB;
	double L;
	/// offsets, replace them for correct behaviour
	dvec3 rA = dvec3(0);
	dvec3 rB = dvec3(0);

	/// rAloc is offset of constraint in local coordinates of body A
	SoftTwoBodyDistanceConstraint(RigidBody* bodyA_In, RigidBody* bodyB_In, vec3 rAloc = vec3(0,0,0), vec3 rBloc = vec3(0,0,0))
	{
		this->bodyA = bodyA_In;
		this->bodyB = bodyB_In;
		this->bodyA->SetSleepingEnabled(false);
		this->bodyB->SetSleepingEnabled(false);

		// rA, rB before rotation is applied, local
		this->rA = rAloc;
		this->rB = rBloc;
		this->L = length(bodyB->LocalToGlobal(rB) - bodyA->LocalToGlobal(rA)); // save initial distance
	}
	/// Constraint: 1/2((p2- p1)^2 - L^2)
	virtual void Solve(double dt)
	{
		const dvec3 d = bodyB->position - bodyA->position;
		
		// create J:
		// J = (J1, J2, J3, J4)
		/// TODO: do we need all factors ? Symmetric...
		/// I think J2 and J4 are needed for angular momentum
		const dvec3 J1 = - d;
		const dvec3 J2 = -cross(bodyA->LocalToGlobal(rA) - bodyA->position, d);
		const dvec3 J3 = d;
		const dvec3 J4 = cross(bodyB->LocalToGlobal(rB) - bodyB->position, d);
		
		// get V
		// (vA, omegaA, vB, omegaB)
		const dvec3 vA = bodyA->velocity;
		const dvec3 omegaA = bodyA->angularVelocity;
		const dvec3 vB = bodyB->velocity;
		const dvec3 omegaB = bodyB->angularVelocity;
		
		// soft parameter
		// http://www.ode.org/ode-latest-userguide.html#sec_3_8_0
		double CFM = 0.00001;
		
		// get m_c
		const double effectiveMass = 1./( 
									bodyA->GetEffectiveMassInverse(J1,J2) + 
									bodyB->GetEffectiveMassInverse(J3,J4)
									+ CFM/dt);
		
		int size = 10;
		DebugRenderer::Instance()->AddDebugPoint(bodyA->LocalToGlobal(rA), dvec3(0,0.5,0.5), size);
		DebugRenderer::Instance()->AddDebugPoint(bodyB->LocalToGlobal(rB), dvec3(0.5,0.5,0), size);
		
		// baumgarte stabilization
		const double beta = 0.1;
		//const double C = 1./2.*(length2(bodyA->LocalToGlobal(rA) - bodyB->LocalToGlobal(rB)) - std::pow(L,2));
		const double C = (length(bodyA->LocalToGlobal(rA) - bodyB->LocalToGlobal(rB)) - L);
		
		// solve (dot(J,V)+b)
		const double deltaV = dot(vA, J1) + dot(omegaA, J2) + dot(vB, J3) + dot(omegaB, J4) + beta*C;
		const double lambda = -effectiveMass * deltaV;
									
		// get impulse
		const dvec3 impulse1 = J1*lambda;
		const dvec3 impulse2 = J2*lambda;
		const dvec3 impulse3 = J3*lambda;
		const dvec3 impulse4 = J4*lambda;

		// apply forces to the two bodies
		bodyA->ApplyLinearMomentum(impulse1);
		bodyA->ApplyAngularMomentum(impulse2);
		
		bodyB->ApplyLinearMomentum(impulse3);
		bodyB->ApplyAngularMomentum(impulse4);
	}
};
