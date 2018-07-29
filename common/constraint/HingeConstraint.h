#pragma once
#include <iomanip>
#include "constraint/Constraint.h"

/* 
 * Enforces a hinge between two rigidBody 
 * similar to p20 - ...: http://danielchappuis.ch/download/ConstraintsDerivationRigidBody3D.pdf
 */
class HingeConstraint : public Constraint
{
public:
	RigidBody* bodyA;	/// optional TODO: accept none rigidbody two allow single body hinges as in DistanceConstraint
	RigidBody* bodyB;
	
	dvec3 aA_loc;	// local axis of hinge of bodyA
	dvec3 aB_loc;	// local axis of hinge of bodyB
	dvec3 pA_loc;	// local anchor Point of bodyA
	dvec3 pB_loc;	// local anchor Point of bodyB

	/// a_global: global axis of hinge
	/// p_global: global anchor point of hinge
	HingeConstraint(RigidBody* bodyA_In, RigidBody* bodyB_In, dvec3 a_global, dvec3 p_global)
	{
		this->bodyA = bodyA_In;
		this->bodyB = bodyB_In;
		
		this->bodyA->SetSleepingEnabled(false);
		this->bodyB->SetSleepingEnabled(false);

		const dmat3 RA = glm::mat3_cast(bodyA->rotation);
		const dmat3 RB = glm::mat3_cast(bodyB->rotation);
		
		/// correct?
		this->aA_loc = normalize(inverse(RA)*a_global);
		this->aB_loc = normalize(inverse(RB)*a_global);
		
		this->pA_loc = bodyA->GlobalToLocal(p_global);
		this->pB_loc = bodyB->GlobalToLocal(p_global);
		
	}
	// Constraints
	// C_trans = x2+r2-x1-r1
	// C_rot = [dot(a1,b2); dot(a1,c1)]
	virtual void Solve(double dt)
	{
		// transform local coordinates back to global
		/// TODO: could be done more efficient with only the rotation matrix	
		const dvec3 x1 = bodyA->position;
		const dvec3 x2 = bodyB->position;
		const dvec3 a1 = normalize(bodyA->LocalToGlobal(this->aA_loc)-x1);
		const dvec3 a2 = normalize(bodyB->LocalToGlobal(this->aB_loc)-x2);
		const dvec3 r1 = bodyA->LocalToGlobal(this->pA_loc)-x1;	/// r1 is from body center to anchor point
		const dvec3 r2 = bodyB->LocalToGlobal(this->pB_loc)-x2;	/// r2 is from body center to anchor point
		
		// get normal vectors to a_2		
		const dvec3 b2 = GetAOrthogonalVector(a2);
		const dvec3 c2 = cross(a2, b2);
		
		// create J_trans:
		// J = [J1, J2, J3, J4]
		const dmat3 J1 = -dmat3(1);
		const dmat3 J2 = GetSkewCrossMatrix(r1);
		const dmat3 J3 = dmat3(1);
		const dmat3 J4 = -GetSkewCrossMatrix(r2);
		
		// create mass matrix for translation
		const dmat3 K_trans = this->bodyA->GetEffectiveMassInverse(J1,J2) + this->bodyB->GetEffectiveMassInverse(J3,J4);
		
		// create J_rot:
		// J = [	J11, J12, J13, J14;
		//			J21, J22, J23, J24;]
		const dvec3 J11 = dvec3(0);
		const dvec3 J12 = -cross(b2,a1);
		const dvec3 J13 = dvec3(0);
		const dvec3 J14 = cross(b2,a1);
		const dvec3 J21 = dvec3(0);
		const dvec3 J22 = -cross(c2,a1);
		const dvec3 J23 = dvec3(0);
		const dvec3 J24 = cross(c2,a1);
	
		// crete mass matrix for rotation
		const dmat2 K_rot = this->bodyA->GetEffectiveMassInverse(J11, J12, J21, J22) + this->bodyB->GetEffectiveMassInverse(J13,J14,J23,J24);

		// get V
		// (vA, omegaA, vB, omegaB)
		const dvec3 v1 = bodyA->velocity;
		const dvec3 omega1 = bodyA->angularVelocity;
		const dvec3 v2 = bodyB->velocity;
		const dvec3 omega2 = bodyB->angularVelocity;
		
		// baumgarte stabilization
		const double beta = 0.01;
		const dvec3 C_trans = x2+r2-x1-r1;
		const dvec2 C_rot(dot(a1,b2), dot(a1,c2));
		
		// --- solve translation constraints ---
		const dvec3 deltaVTrans = J1*v1 + J2*omega1 + J3*v2 + J4*omega2 + beta/dt*C_trans;
		const dvec3 lambdaTrans = -inverse(K_trans)*deltaVTrans;

		const dvec3 impulseLinear1 = J1*lambdaTrans;
		dvec3 impulseAngular1 = -J2*lambdaTrans;	/// thomaset: why the heck has a minus to be here? Only works like this...
		const dvec3 impulseLinear2 = J3*lambdaTrans;
		dvec3 impulseAngular2 = -J4*lambdaTrans;	/// thomaset: why the heck has a minus to be here? Only works like this...

		// --- solve rotation constraints ---
		const dvec2 deltaVRot(	dot(J12,omega1) + dot(J14,omega2) + C_rot[0],
							dot(J22,omega1) + dot(J24,omega2) + C_rot[1]);
		const dvec2 lambdaRot = -inverse(K_rot)*deltaVRot;
		
		impulseAngular1 += J12*lambdaRot[0] + J22*lambdaRot[1];
		impulseAngular2 += J14*lambdaRot[0] + J24*lambdaRot[1];
		
		// --- Apply the impulses --	
		bodyA->ApplyLinearMomentum(impulseLinear1);
		bodyA->ApplyAngularMomentum(impulseAngular1);
		
		bodyB->ApplyLinearMomentum(impulseLinear2);
		bodyB->ApplyAngularMomentum(impulseAngular2);
	}
	
	// retruns an "arbitrarly" orthogonal vector to v1
	dvec3 GetAOrthogonalVector(const dvec3 v1) const{
		const dvec3 tmp(v1[1], v1[2], v1[0]);		// prevent co-linearity
		return normalize(cross(tmp, v1));
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
