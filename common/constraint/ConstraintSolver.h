#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include "Constraint.h"
#include "ContactConstraint.h"

#include <vector>


/* 
 * Sequential impulse constraint solving 	
 * reference: http://twvideo01.ubm-us.net/o1/vault/gdc09/slides/04-GDC09_Catto_Erin_Solver.pdf 
 * and http://allenchou.net/2013/12/game-physics-constraints-sequential-impulse/ 
 */
class ConstraintSolver
{

private:
	int iterations = 4;

	std::vector<Constraint*> persistantConstraints;
	std::vector<Constraint*> dynamicConstraints;

public:

	void SetIterations(int i) { this->iterations = i; }
	int GetIterations() { return this->iterations; }

	ConstraintSolver()
	{

	}

	~ConstraintSolver()
	{
		Clear();
	}

	void AddConstraint(Constraint* c)
	{
		this->persistantConstraints.push_back(c);
	}

	void Clear()
	{
		for (Constraint* c : persistantConstraints)
		{
			delete c;
		}
		persistantConstraints.clear();
	}


	void Solve(double dt, std::unordered_map<std::pair<int,int>, ContactManifold*>& activeContactManifolds)
	{
		// create constraints
		dynamicConstraints.clear();

		for (std::pair<const std::pair<int,int>, ContactManifold*>& i : activeContactManifolds)
		{
			for (Contact* c : i.second->contacts)
			{
				if (c->bodyA->inactive && c->bodyB->inactive) continue;
				else if (c->bodyA->inactive && c->bodyB->isStatic) continue;
				else if (c->bodyA->isStatic && c->bodyB->inactive) continue;

				dynamicConstraints.push_back(c->constraint);
			}
		}

		// warm start
		for (Constraint* c : dynamicConstraints)
		{
			c->Apply(dt);
		}
		for (Constraint* c : persistantConstraints)
		{
			c->Apply(dt);
		}

		int maxIterations = iterations; // very important for quality/performance
		do
		{
			maxIterations--;

			for (Constraint* c : dynamicConstraints)
			{
				c->Solve(dt);
			}
			for (Constraint* c : persistantConstraints)
			{
				c->Solve(dt);
			}
		}
		while (maxIterations > 0);
	}

};
