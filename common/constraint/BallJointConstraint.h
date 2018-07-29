#pragma once
#include <iomanip>
#include "constraint/Constraint.h"

/* 
 * Enforces a ball / socket joint between two rigidBody 
 * similar to p12 - ...: http://danielchappuis.ch/download/ConstraintsDerivationRigidBody3D.pdf
 */
class BallJointConstraint : public Constraint
{
public:
	RigidBody* bodyA;	/// optional TODO: accept none rigidbody two allow single body hinges as in DistanceConstraint
	RigidBody* bodyB;
	
	dvec3 pA_loc;	// local anchor Point of bodyA
	dvec3 pB_loc;	// local anchor Point of bodyB

	/// p_global: global anchor point of hinge
	BallJointConstraint(RigidBody* bodyA_In, RigidBody* bodyB_In, dvec3 p_global)
	{
		this->bodyA = bodyA_In;
		this->bodyB = bodyB_In;
		
		this->bodyA->SetSleepingEnabled(false);
		this->bodyB->SetSleepingEnabled(false);
		
		this->pA_loc = bodyA->GlobalToLocal(p_global);
		this->pB_loc = bodyB->GlobalToLocal(p_global);
		
	}
	// Constraints
	// C_trans = x2+r2-x1-r1
	virtual void Solve(double dt)
	{
		// transform local coordinates back to global
		/// TODO: could be done more efficient with only the rotation matrix	
		const dvec3 x1 = bodyA->position;
		const dvec3 x2 = bodyB->position;
		const dvec3 r1 = bodyA->LocalToGlobal(this->pA_loc)-x1;	/// r1 is from body center to anchor point
		const dvec3 r2 = bodyB->LocalToGlobal(this->pB_loc)-x2;	/// r2 is from body center to anchor point
		
		// create J_trans:
		// J = [J1, J2, J3, J4]
		const dmat3 J1 = -dmat3(1);
		const dmat3 J2 = GetSkewCrossMatrix(r1);
		const dmat3 J3 = dmat3(1);
		const dmat3 J4 = -GetSkewCrossMatrix(r2);
		
		// create mass matrix for translation
		const dmat3 K_trans = this->bodyA->GetEffectiveMassInverse(J1,J2) + this->bodyB->GetEffectiveMassInverse(J3,J4);
		
		// get V
		// (vA, omegaA, vB, omegaB)
		const dvec3 v1 = bodyA->velocity;
		const dvec3 omega1 = bodyA->angularVelocity;
		const dvec3 v2 = bodyB->velocity;
		const dvec3 omega2 = bodyB->angularVelocity;
		
		// baumgarte stabilization
		const double beta = 0.01;
		const dvec3 C_trans = x2+r2-x1-r1;
		
		// --- solve translation constraints ---
		const dvec3 deltaVTrans = J1*v1 + J2*omega1 + J3*v2 + J4*omega2 + beta/dt*C_trans;
		const dvec3 lambdaTrans = -inverse(K_trans)*deltaVTrans;

		const dvec3 impulseLinear1 = J1*lambdaTrans;
		const dvec3 impulseAngular1 = -J2*lambdaTrans;	/// thomaset: why the heck has a minus to be here? Only works like this...
		const dvec3 impulseLinear2 = J3*lambdaTrans;
		const dvec3 impulseAngular2 = -J4*lambdaTrans;	/// thomaset: why the heck has a minus to be here? Only works like this...
		
		// --- Apply the impulses --	
		bodyA->ApplyLinearMomentum(impulseLinear1);
		bodyA->ApplyAngularMomentum(impulseAngular1);
		
		bodyB->ApplyLinearMomentum(impulseLinear2);
		bodyB->ApplyAngularMomentum(impulseAngular2);
	}
		
	// https://en.wikipedia.org/wiki/Skew-symmetric_matrix#Cross_product
	dmat3 GetSkewCrossMatrix(const dvec3 v1) const{
		dmat3 result(0);
		result[1][0] = -v1[2];
		result[2][0] = v1[1];
		
		result[0][1] = v1[2];
		result[2][1] = -v1[0];
		
		result[0][2] = -v1[1];
		result[1][2] = v1[0];
		return result;
	}
};
