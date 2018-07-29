#pragma once

#include "RigidBody.h"
#include "constraint/Constraint.h"
#include "collision/Contact.h"

/*
 * Calculates impulses for to resolve collisions
 * Constraint of form C=JV+b>=0
 * idea from: http://twvideo01.ubm-us.net/o1/vault/gdc09/slides/04-GDC09_Catto_Erin_Solver.pdf and
 * http://www.bulletphysics.com/ftp/pub/test/physics/papers/IterativeDynamics.pdf
 */

class ContactConstraint : public Constraint
{

public:

	double normalImpulseSum;
	double tangent1ImpulseSum;
	double tangent2ImpulseSum;
	Contact* contact;

	bool warm = false;

	ContactConstraint(Contact* c)
	{
		SetContact(c);
	}

	void SetContact(Contact* c)
	{
		contact = c;
		Clear();
	}

	void Clear()
	{
		normalImpulseSum = 0;
		tangent1ImpulseSum = 0;
		tangent2ImpulseSum = 0;
		warm = false;
	}

	
	// warm start, reuse lambda from last iteration as initial guess
	virtual void Apply(double dt)
	{
		if (!warm) return;

		contact->Update();
		if (contact->type != ContactType::Colliding)
		{
			Clear();
			return;
		}

		double warmStartFactor = 0.7;

		// reuse last value completely if bodies inactive (body is completely stable)

		// apply normal impulse
		normalImpulseSum *= warmStartFactor;

		Contact &c = *contact;

		dvec3 ra = c.location - c.bodyA->position;
		dvec3 rb = c.location - c.bodyB->position;
		dvec3 raCrossN = cross(ra,c.normal);
		dvec3 rbCrossN = cross(rb,c.normal);

		dvec3 force = c.normal*normalImpulseSum;
		c.bodyA->ApplyLinearMomentum(force);
		c.bodyB->ApplyLinearMomentum(-force);

		c.bodyA->ApplyAngularMomentum(raCrossN * normalImpulseSum);
		c.bodyB->ApplyAngularMomentum(-rbCrossN * normalImpulseSum);
		

		// apply friction

		tangent1ImpulseSum *= warmStartFactor;
		tangent2ImpulseSum *= warmStartFactor;

		force = c.tangent1*tangent1ImpulseSum + c.tangent2*tangent2ImpulseSum;

		dvec3 raCrossT1 = cross(ra,c.tangent1);
		dvec3 rbCrossT1 = cross(rb,c.tangent1);
		dvec3 raCrossT2 = cross(ra,c.tangent2);
		dvec3 rbCrossT2 = cross(rb,c.tangent2);

		c.bodyA->ApplyLinearMomentum(force);
		c.bodyB->ApplyLinearMomentum(-force);

		c.bodyA->ApplyAngularMomentum( raCrossT1 * tangent1ImpulseSum  +  raCrossT2*tangent2ImpulseSum);
		c.bodyB->ApplyAngularMomentum(-rbCrossT1 * tangent1ImpulseSum  + -rbCrossT2*tangent2ImpulseSum);


		warm = false;
	}

	virtual void Solve(double dt)
	{
		contact->Update();
		if (contact->type != ContactType::Colliding) return;

		// normal impulse
		solveNormal(dt);

		// friction
		//solveTangent(contact->tangent1, tangent1ImpulseSum);
		//solveTangent(contact->tangent2, tangent2ImpulseSum);
		solveTangentCoupled();

		warm = true;
	}

	// http://www.bulletphysics.com/ftp/pub/test/physics/papers/IterativeDynamics.pdf page 12
	void solveTangentCoupled()
	{
		Contact& c = *contact;

		// get V
		// (vA, omegaA, vB, omegaB)
		dvec3 vA = c.bodyA->velocity;
		dvec3 omegaA = c.bodyA->angularVelocity;
		dvec3 vB = c.bodyB->velocity;
		dvec3 omegaB = c.bodyB->angularVelocity;

		// J = (J1, J2)
		// J1 =(c.tangent1, raCrossT1, -c.tangent1, -rbCrossT1)'
		dvec3 ra = c.location - c.bodyA->position;
		dvec3 rb = c.location - c.bodyB->position;

		dvec3 raCrossT1 = cross(ra,c.tangent1);
		dvec3 rbCrossT1 = cross(rb,c.tangent1);
		
		// J2 =(c.tangent2, raCrossT2, -c.tangent2, -rbCrossT2)'
		dvec3 raCrossT2 = cross(ra,c.tangent2);
		dvec3 rbCrossT2 = cross(rb,c.tangent2);

		// b = 0

		dmat2 mEffInvA = c.bodyA->GetEffectiveMassInverse(c.tangent1, raCrossT1, c.tangent2, raCrossT2);
		dmat2 mEffInvB = c.bodyB->GetEffectiveMassInverse(-c.tangent1, -rbCrossT1, -c.tangent2, -rbCrossT2);
		dmat2 effectiveMass = inverse(mEffInvA + mEffInvB);

		// solve (dot(J,V)+b)
		double deltaV1 = dot(vA, c.tangent1) - dot(vB, c.tangent1) + dot(omegaA, raCrossT1) - dot(omegaB, rbCrossT1);
		double deltaV2 = dot(vA, c.tangent2) - dot(vB, c.tangent2) + dot(omegaA, raCrossT2) - dot(omegaB, rbCrossT2);
		dvec2 deltaV(deltaV1, deltaV2);

		dvec2 lambda = -effectiveMass * deltaV;

		// http://www.bulletphysics.com/ftp/pub/test/physics/papers/IterativeDynamics.pdf eqquations 24 and 25
		//double bound = 0.8*bodyA->frictionCoefficient*bodyB->frictionCoefficient; // should be somewhere about gravity
		double bound = normalImpulseSum*c.bodyA->friction*c.bodyB->friction;
		lambda.x = addAndClampSum(tangent1ImpulseSum, lambda.x, -bound, bound);
		lambda.y = addAndClampSum(tangent2ImpulseSum, lambda.y, -bound, bound);

		dvec3 force = c.tangent1*lambda.x + c.tangent2*lambda.y;

		c.bodyA->ApplyLinearMomentum(force);
		c.bodyB->ApplyLinearMomentum(-force);

		c.bodyA->ApplyAngularMomentum( raCrossT1 * lambda.x  +  raCrossT2*lambda.y);
		c.bodyB->ApplyAngularMomentum(-rbCrossT1 * lambda.x  + -rbCrossT2*lambda.y);
	}

	void solveTangent(dvec3 tangent, double& impulseSum)
	{
		Contact& c = *contact;

		// get V
		// (vA, omegaA, vB, omegaB)
		dvec3 vA = c.bodyA->velocity;
		dvec3 omegaA = c.bodyA->angularVelocity;
		dvec3 vB = c.bodyB->velocity;
		dvec3 omegaB = c.bodyB->angularVelocity;

		// create J:
		// (tangent, raCrossN, -tangent, -rbCrossN)
		dvec3 ra = c.location - c.bodyA->position;
		dvec3 rb = c.location - c.bodyB->position;
		dvec3 raCrossN = cross(ra,tangent);
		dvec3 rbCrossN = cross(rb,tangent);

		// b = 0

		// create effectiveMass = 1/(transpose(J)*MInverse*J)
		double mEffInvA = c.bodyA->GetEffectiveMassInverse(tangent, raCrossN);
		double mEffInvB = c.bodyB->GetEffectiveMassInverse(-tangent, -rbCrossN);
		double effectiveMass = 1./(mEffInvA + mEffInvB);

		// solve (dot(J,V)+b)
		double deltaV = dot(vA, tangent) - dot(vB, tangent) + dot(omegaA, raCrossN) - dot(omegaB, rbCrossN);
		double lambda = -effectiveMass * deltaV;

		double bound = normalImpulseSum*c.bodyA->friction*c.bodyB->friction;
		lambda = addAndClampSum(impulseSum, lambda, -bound, bound);

		dvec3 force = tangent*lambda;

		c.bodyA->ApplyLinearMomentum(force);
		c.bodyB->ApplyLinearMomentum(-force);

		c.bodyA->ApplyAngularMomentum(raCrossN * lambda);
		c.bodyB->ApplyAngularMomentum(-rbCrossN * lambda);
	}

	void solveNormal(double dt)
	{ 
		Contact& c = *contact;

		double restitution = c.bodyA->restitution * c.bodyB->restitution;
		double restitutionSlopp = 0.01; // http://allenchou.net/2014/01/game-physics-stability-slops/ removes energy to come faster to rest
		double pushFactor = 0.01; // pushes objects out of each other (http://www.bulletphysics.com/ftp/pub/test/physics/papers/IterativeDynamics.pdf, page 11);
		double pushSlopp = 0.01; // allowed penetration depth before pushing out

		// get V
		// (vA, omegaA, vB, omegaB)
		dvec3 vA = c.bodyA->velocity;
		dvec3 omegaA = c.bodyA->angularVelocity;
		dvec3 vB = c.bodyB->velocity;
		dvec3 omegaB = c.bodyB->angularVelocity;

		// create J:
		// (c.normal, raCrossN, -c.normal, -rbCrossN)
		dvec3 ra = c.location - c.bodyA->position;
		dvec3 rb = c.location - c.bodyB->position;
		dvec3 raCrossN = cross(ra,c.normal);
		dvec3 rbCrossN = cross(rb,c.normal);

		// create bias
		double b = restitution * std::min(c.vRel + restitutionSlopp, 0.0);

		// Baumgarte Stabilization: pushes body out of each other -> adds jiggle
		b -= pushFactor*std::max(c.depth-pushSlopp,0.0)/dt;


		// create Minverse
		double mEffInvA = c.bodyA->inverseMass + dot(raCrossN, c.bodyA->inertiaTensorInverse * raCrossN);
		double mEffInvB = c.bodyB->inverseMass + dot(rbCrossN, c.bodyB->inertiaTensorInverse * rbCrossN);

		//double mEffInvB = c.bodyB->GetEffectiveMassInverse(-c.normal, -rbCrossN);
		//double mEffInvA = c.bodyA->GetEffectiveMassInverse(c.normal, raCrossN);
		
		double effectiveMass = 1./(mEffInvA + mEffInvB);

		// solve (dot(J,V)+b)
		//double deltaV = dot(vA, c.normal) - dot(vB, c.normal) + dot(omegaA, raCrossN) - dot(omegaB, rbCrossN) + b;
		double deltaV = c.vRel + b;
		double lambda = -effectiveMass * deltaV;

		/*std::cout << "c.n= " << to_string(c.normal) << "l=" << lambda << " em= " << effectiveMass << 
				" va=" << to_string(vA) << " vb= " << to_string(vB) << " oA= " << to_string(omegaA) << " oB= " << to_string(omegaB)  << 
				" dv= " << deltaV << " mea= " << mEffInvA << " meb " << mEffInvB << std::endl;
		assert(!isnan(lambda));
		assert(!isnan(c.normal.x));*/

		lambda = addAndClampSum(normalImpulseSum, lambda);

		dvec3 force = c.normal*lambda;

		c.bodyA->ApplyLinearMomentum(force);
		c.bodyB->ApplyLinearMomentum(-force);

		c.bodyA->ApplyAngularMomentum(raCrossN * lambda);
		c.bodyB->ApplyAngularMomentum(-rbCrossN * lambda);
	}
};


Contact::Contact() {
	constraint = new ContactConstraint(this);
}

Contact::~Contact()
{
	delete constraint;
	constraint = 0;
}

void Contact::ClearConstraint()
{
	constraint->Clear();
}
