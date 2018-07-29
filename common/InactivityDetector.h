#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <vector>
#include <map>
#include <list>
#include <set>
#include <unordered_map>
#include <iostream>
#include <iomanip>

#include "timer.h"
#include "Helper.h"

/*
 * Detects connected (in contact) sets of sleeping bodies lying on the ground and sets them inactive
 * Via Reactivate they can be set active again with all other bodies in the same set
 */
class InactivityDetector
{

private:

	std::map<int, std::set<RigidBody*>> inactivitySets;
	std::set<RigidBody*> inactivityChecked;

	//int updateInterval = 30; // recompute only every i-th update
	//int updateTimer = 0; 

	double updateInterval = 1./2.;
	double updateTimer = 0;

public:

	InactivityDetector()
	{

	}	

	void Clear()
	{
		inactivitySets.clear();
		inactivityChecked.clear();
	}

	// reactivates a body and its inactivity set
	void Reactivate(RigidBody* a)
	{
		if (a->inactive)
		{
			auto it = inactivitySets.find(a->inactiveSetId);

			for (RigidBody* b : it->second)
			{
				b->inactive = false;
				b->sleeping = false;
				b->changeAverage = 20; // avoid immediate sleeping
				b->inactiveSetId = 0;
			}

			inactivitySets.erase(it);
		}
	}

	// find inactivity sets
	void Update(double dt, std::vector<RigidBody*>& bodies)
	{
		updateTimer -= dt;
		if (updateTimer > 0) return;
		else
		{
			updateTimer = updateInterval;
		}

		inactivityChecked.clear();
		inactivitySets.clear();

		int id = 0;


		for (RigidBody* ground : bodies)
		{
			ground->inactive = false;

			// we only start recursions from objects touching ground static object
			if (ground->isStatic)
			{
				for (const std::pair<int, ContactManifold*>& it : ground->manifolds)
				{
					RigidBody* a = it.second->GetOtherBody(ground);
					a->grounded = true;
					if (!a->sleeping) continue;

					// already checked ?
					if (inactivityChecked.find(a) != inactivityChecked.end()) continue;
					inactivityChecked.insert(a);

					std::set<RigidBody*> inactiveSet;
					inactiveSet.insert(a);

					if (recurseInactivitySet(a, inactiveSet))
					{
						inactivitySets[id++] = inactiveSet;
					}
				}
			}
		}

		for (const std::pair<int, std::set<RigidBody*>>& it : inactivitySets)
		{
			for (RigidBody * b : it.second)
			{
				b->inactive = true;
				b->inactiveSetId = it.first;
			}
		}

		for (RigidBody* body : bodies)
		{
			if (!body->inactive && !body->isStatic && !body->grounded)
			{
				body->RevalidateSleeping();
			}

			// clear for next update
			body->grounded = false;
		}
	}

protected:

	bool recurseInactivitySet(RigidBody* a, std::set<RigidBody*>& currentSet)
	{
		for (const std::pair<int, ContactManifold*>& it : a->manifolds)
		{
			RigidBody* b = it.second->GetOtherBody(a);

			if (b->isStatic) continue;

			b->grounded = true;

			if (inactivityChecked.find(b) != inactivityChecked.end())
			{
				// if b is checked but not in the set, the current set must be invalid
				if (currentSet.find(b) == currentSet.end()) return false;
				else continue; // be was checked already, so dont check it again
			}
			else  inactivityChecked.insert(b);

			if (b->sleeping)
			{
				currentSet.insert(b);
				if (!recurseInactivitySet(b, currentSet)) return false;
			}
			// only sleeping objects allowed
			else return false;
		}

		return true;
	}

};
