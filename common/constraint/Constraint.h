#pragma once

#include "RigidBody.h"
#include "collision/Contact.h"


/*
 * Constraints of form C=JV+b=0
 * idea from: http://twvideo01.ubm-us.net/o1/vault/gdc09/slides/04-GDC09_Catto_Erin_Solver.pdf
 * http://www.bulletphysics.com/ftp/pub/test/physics/papers/IterativeDynamics.pdf
 */


class Constraint
{

public:
	virtual void Solve(double dt) {}
	virtual void Apply(double dt) {}

protected:
	double addAndClampSum(double &sum, double lambda)
	{
		double oldSum = sum;
		sum += lambda;
		if (sum < 0) sum = 0; 
		return sum - oldSum;
	}
	
	double addAndClampSum(double &sum, double lambda, double lowerBound, double upperBound)
	{
		double oldSum = sum;
		sum += lambda;
		if (sum < lowerBound) sum = lowerBound; 
		if (sum > upperBound) sum = upperBound; 
		return sum - oldSum;
	}
};
