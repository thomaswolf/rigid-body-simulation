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
#include "InactivityDetector.h"

/*
 * creates/updates ContactManifolds between all bodies
 */
class CollisionDetector
{
	friend class PhysicManager;

protected:
	std::vector<RigidBody*> bodies;

	std::unordered_map<std::pair<int,int>, ContactManifold*> contactManifolds; // cache of all created manifolds
	std::unordered_map<std::pair<int,int>, ContactManifold*> activeContactManifolds; // currently active manifolds

	InactivityDetector* inactivityDetector;

public:

	CollisionDetector(InactivityDetector* inactivityDetector) : inactivityDetector(inactivityDetector)
	{
		assert(inactivityDetector != NULL);

		contactManifolds.reserve(3000);
		activeContactManifolds.reserve(3000);
	}

	~CollisionDetector()
	{

	}
	
	virtual void AddBody(RigidBody* body)
	{
		bodies.push_back(body);
	}

	virtual void FindCollisions() { }

	virtual void Clear()
	{
		for (std::pair<const std::pair<int,int>, ContactManifold*>& i : contactManifolds)
		{
			delete i.second;
		}
		contactManifolds.clear();
		activeContactManifolds.clear();
		bodies.clear();

		ContactManifoldPool::GetInstance().Clear();
		ContactPool::GetInstance().Clear();
	}

protected:

	void narrowPhase(RigidBody* a, RigidBody* b)
	{
		ContactManifold* manifold = NULL;
		std::pair<int,int> pairIndex = getPairIndex(a,b);	
		manifold = contactManifolds[pairIndex];

		// reuse old contact if both bodies are sleeping
		if (a->sleeping	&& b->sleeping && manifold != NULL)
		{
			manifold->persistent = true;
			activeContactManifolds[pairIndex] = manifold;
			a->manifolds[b->id] = manifold;
			b->manifolds[a->id] = manifold;
			return;
		}

		// get manifold from pool
		if (manifold == NULL)
		{
			manifold = ContactManifoldPool::GetInstance().Get();
			manifold->bodyA = a;
			manifold->bodyB = b;
			contactManifolds[pairIndex] = manifold;
		}

		// actual contact computation
		if (a->ComputeContactManifold(manifold))
		{
			inactivityDetector->Reactivate(a);
			inactivityDetector->Reactivate(b);
			manifold->persistent = true;
			activeContactManifolds[pairIndex] = manifold;
			a->manifolds[b->id] = manifold;
			b->manifolds[a->id] = manifold;
		}
	}


	virtual void prepare()
	{
		removeNonPersistentManifolds();
		activeContactManifolds.clear();
	}

	// cleaning of contactManifolds; removes all manifolds that are non persistant (those that were not used in the last iteration)
	void removeNonPersistentManifolds()
	{
		for (auto it = contactManifolds.begin(); it != contactManifolds.end();)
		{
			ContactManifold* m = it->second;

			if (!it->second->persistent)
			{
				// clear maps in bodies
				m->bodyA->manifolds.erase(m->bodyB->id);
				m->bodyB->manifolds.erase(m->bodyA->id);

				// clear our map
				ContactManifoldPool::GetInstance().Recycle(m);
				it = contactManifolds.erase(it);
			}
			else
			{
				m->persistent = false,
				++it;
			}
		}
	}

	std::pair<int,int> getPairIndex(RigidBody* a, RigidBody *b )
	{
		if (a < b) 	return std::make_pair(a->id, b->id);
		else	 	return std::make_pair(b->id, a->id);
	}
};

class NaiveCollisionDetector : public CollisionDetector
{
public:
	NaiveCollisionDetector(InactivityDetector* inactivityDetector) : CollisionDetector(inactivityDetector)
	{
	}

	virtual void FindCollisions()
	{
		prepare();

		for (RigidBody* b :bodies)
		{
			for (RigidBody* a : bodies)
			{
				// dont compare static
				if (a->inverseMass == 0 && b->inverseMass == 0) continue;

				// compare only once
				if (a->id >= b->id) continue; 

				// broad collision detection
				if (a->aabb.IntersectsWith(b->aabb))
				{
					narrowPhase(a,b);
				}
			}
		}
	}
};

class SweepAndPruneCollisionDetector : public CollisionDetector
{
private:
	std::vector<RigidBody*> axisx;
	std::vector<RigidBody*> axisy;
	std::vector<RigidBody*> axisz;
	std::set<std::pair<RigidBody*,RigidBody*>> broadCollisions; // cache of all created manifolds
	std::vector<RigidBody*> active;
	bool initial = true;

public:
	SweepAndPruneCollisionDetector(InactivityDetector* inactivityDetector) : CollisionDetector(inactivityDetector)
	{
	}

	virtual void AddBody(RigidBody* body)
	{
		CollisionDetector::AddBody(body);
		axisx.push_back(body);
		axisy.push_back(body);
		axisz.push_back(body);
	}

	virtual void Clear()
	{
		CollisionDetector::Clear();
		axisx.clear();
		axisy.clear();
		axisz.clear();
		initial = true;
		active.clear();
		broadCollisions.clear();
	}

	void sortAxis(std::vector<RigidBody*>& axis, int dim)
	{
		int n = axis.size();
		
		for (int i=0; i<n; ++i)
		{
			RigidBody* a = axis[i];	

			double val = a->aabb.min[dim];

			int j = i-1;

			// move body left until it is in the right position
			while(j >= 0 && axis[j]->aabb.min[dim] > val)
			{
				RigidBody* b = axis[j];

				// shift body b to the right
				axis[j+1] = b;
				j--;

				// dont compare static
				if (a->inverseMass == 0 && b->inverseMass == 0) continue;

				std::pair<RigidBody*, RigidBody*> p;
				if (a->id < b->id) 	p = std::make_pair(a, b);
				else	 			p = std::make_pair(b, a);

				if (a->aabb.IntersectsWith(b->aabb))
				{
					broadCollisions.insert(p);
				}
				else
				{
					broadCollisions.erase(p);
				}
			}
			axis[j+1] = a;
		}
		//std::cout << "shifts = " << shifts << std::endl;
	}

	void initialPhase()
	{
		std::cout << "initial" << std::endl;
		for (RigidBody* b :bodies)
		{
			for (RigidBody* a : bodies)
			{
				// dont compare static
				if (a->inverseMass == 0 && b->inverseMass == 0) continue;

				// compare only once
				if (a->id >= b->id) continue; 

				// broad collision detection
				if (a->aabb.IntersectsWith(b->aabb))
				{
					broadCollisions.insert(std::make_pair(a,b));
				}
			}
		}

		std::sort(axisx.begin(), axisx.end(), [](const RigidBody* a, const RigidBody* b){
			return a->aabb.min.x < b->aabb.min.x;
		});
		std::sort(axisy.begin(), axisy.end(), [](const RigidBody* a, const RigidBody* b){
			return a->aabb.min.y < b->aabb.min.y;
		});
		std::sort(axisz.begin(), axisz.end(), [](const RigidBody* a, const RigidBody* b){
			return a->aabb.min.z < b->aabb.min.z;
		});
	}

	virtual void FindCollisions()
	{
		//FindCollisionsAllAxis();
		FindCollisionsOneAxis();
	}
	
	virtual void FindCollisionsAllAxis()
	{

		if (initial)
		{
			initialPhase();
			initial = false;
		}
		else
		{
			sortAxis(axisx, 0);
			sortAxis(axisy, 1);
			sortAxis(axisz, 2);
		}

		std::cout << broadCollisions.size() << std::endl;

		// handle narrow collisions
		for (const std::pair<RigidBody*, RigidBody*>& coll : broadCollisions)
		{
			RigidBody* a = coll.first;
			RigidBody* b = coll.second;
			assert(a->id < b->id);
			narrowPhase(a,b);
		}
	}


	virtual void FindCollisionsOneAxis()
	{
		prepare();
		active.clear();

		std::sort(bodies.begin(), bodies.end(), [](const RigidBody* a, const RigidBody* b){
			return a->aabb.min.x < b->aabb.min.x;
		});


		for (RigidBody* a : bodies)
		{

			for (auto it = active.rbegin(); it != active.rend() ; )
			{
				RigidBody* b = (*it);

				if (a->aabb.min.x > b->aabb.max.x)
				{
					active.erase(std::next(it).base());
				}

				++it;

				if (a->aabb.IntersectsWith(b->aabb))
				{
					// dont compare static or with itself
					if (a->inverseMass == 0 && b->inverseMass == 0) continue;
					if (a->id == b->id) continue; 
					
					if (a->id > b->id)
						narrowPhase(b,a);
					else
						narrowPhase(a,b);
				}
			}

			active.push_back(a);
		}
	}
};

class SpatialPartitioningCollisionDetector : public CollisionDetector
{
private:
	dvec3 resolution; // 1 means unit volumes, 2 means 2^2 volumes per unit volume
	std::unordered_map<ivec3, std::vector<RigidBody*>, IVec3Op> map;
	std::set<std::pair<RigidBody*,RigidBody*>> broadCollisions;

	int numberOfUsedContacts = 0;
	int broadIntersections = 0;
	int maxSpan = 0;
	size_t maxBodiesPerVolume = 0;

public:

	SpatialPartitioningCollisionDetector(InactivityDetector* inactivityDetector) : CollisionDetector(inactivityDetector)
	{
		resolution = dvec3(0.9,1,0.9); // lower resolution in x and z because the floor needs a lot of space
	}

	virtual void Clear()
	{
		CollisionDetector::Clear();
		broadCollisions.clear();
		map.clear();
	}

	void FindCollisions()
	{
		prepare();

		// find broad collisions
		for (RigidBody* b : bodies)
		{
			addBodyToMap(b);
		}

		// handle narrow collisions
		for (const std::pair<RigidBody*, RigidBody*>& coll : broadCollisions)
		{
			RigidBody* a = coll.first;
			RigidBody* b = coll.second;
			narrowPhase(a,b);
		}
	}

	void PrintStats()
	{
		std::cout << "number of bodies: " <<  bodies.size() << std::endl;
		std::cout << "broad intersections: " <<  broadIntersections << std::endl;
		std::cout << "max b/v: " << maxBodiesPerVolume << std::endl;
		std::cout << "max span: " << maxSpan << std::endl;
		std::cout << "total contact manifolds: " << contactManifolds.size() << std::endl;
		std::cout << "active contact manifolds: " << activeContactManifolds.size() << std::endl;
		ContactPool::GetInstance().PrintInfo();
		std::cout << "number of used contacts: " << numberOfUsedContacts << std::endl;
	}

protected:

	virtual void prepare()
	{
		CollisionDetector::prepare();

		broadCollisions.clear();

		broadIntersections = 0;
		maxBodiesPerVolume = 0;
		maxSpan = 0;
		map.clear();
	}
private:

	void updateVolume(ivec3 index, RigidBody* a)
	{ 
		std::vector<RigidBody*>& bodies = map[index];
		maxBodiesPerVolume = std::max(bodies.size(), maxBodiesPerVolume);
		
		for (RigidBody* b : bodies)
		{
			if (a->id == b->id) continue; 

			// dont compare static
			if (a->inverseMass == 0 && b->inverseMass == 0) continue;

			// broad collision detection
			if (a->aabb.IntersectsWith(b->aabb))
			{
				std::pair<RigidBody*, RigidBody*> p;
				if (a->id < b->id) 	p = std::make_pair(a, b);
				else	 			p = std::make_pair(b, a);

				broadCollisions.insert(p);
				broadIntersections++;
			}
			
		}
		
		bodies.push_back(a);
	}

	void addBodyToMap(RigidBody* b)
	{
		AABB& box = b->aabb;

		dvec3 min = resolution * box.GetMin();
		dvec3 max = resolution * box.GetMax();

		int minx = floor(min.x);
		int miny = floor(min.y);
		int minz = floor(min.z);

		int maxx = floor(max.x);
		int maxy = floor(max.y);
		int maxz = floor(max.z);

		for (int i=minx; i<=maxx; ++i)
		{
			for (int j=miny; j<=maxy; ++j)
			{
				for (int k=minz; k<=maxz; ++k)
				{
					ivec3 index(i,j,k);
					updateVolume(index, b);
				}
			}
		}

		maxSpan = std::max(maxSpan, maxx-minx);
		maxSpan = std::max(maxSpan, maxy-miny);
		maxSpan = std::max(maxSpan, maxz-minz);
	}
};

