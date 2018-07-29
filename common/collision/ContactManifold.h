/*
 * Container class representing multiple contacts between two bodies
 */

#pragma once

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "collision/Contact.h"

class RigidBody; // forward declaration

using namespace glm;

#define PERSISTANCE_THRESHOLD 0.01

class ContactManifold
{

public:
		std::list<Contact*> contacts;
		RigidBody* bodyA;
		RigidBody* bodyB;
		dvec3 normal;
		bool persistent = false;

		ContactManifold()
		{

		}

		~ContactManifold()
		{
			Clear();
		}

		void Clear()
		{
			persistent = false;
			for (Contact* c : contacts)
			{
				ContactPool::GetInstance().Recycle(c);
			}
			contacts.clear();
		}

		// returns the body that is not body a
		inline RigidBody* GetOtherBody(RigidBody* a)
		{
			if (bodyA == a) return bodyB;
			else return bodyA;
		}

		void UpdatePersistence(); // implemented in RigidBody.h because of dependency

		// adds new contact of current frame
		// similar to: http://allenchou.net/2014/01/game-physics-stability-warm-starting/
		void AddContact(Contact* newC)
		{
			// update normal
			normal = newC->normal;

			bool farEnough = true;

			for (Contact* c : contacts)
			{

				farEnough = farEnough && 
						length2(newC->location - c->location) > PERSISTANCE_THRESHOLD*PERSISTANCE_THRESHOLD &&
						length2(newC->locationB- c->locationB) > PERSISTANCE_THRESHOLD*PERSISTANCE_THRESHOLD;

				//c->normal = normal;
			}

			if (farEnough)
			{
				//nPersistant++;
				contacts.push_back(newC);
				//std::cout << "far enough " << to_string(newC.location) << std::endl;
			}

			if (contacts.size() <= 4) return;


			// find 4 good contacts:
			
			// deepest
			double maxDepth = 0;

			Contact* c1;
			for (Contact *c : contacts)
			{
				if (c->depth >= maxDepth)
				{
					maxDepth = c->depth;
					c1 = c;
				}	
			}

			// most far away from deepest
			double maxDst = 0;
			Contact* c2;
			for (Contact *c : contacts)
			{
				double currDst = length2(c->location - c1->location);
				if (currDst >= maxDst)
				{
					maxDst = currDst;	
					c2 = c;
				}
			}

			// furthest from line between c1 and c2
			maxDst = 0;
			Contact* c3;
			dvec3 n = normalize(c2->location - c1->location);

			for (Contact *c : contacts)
			{
				dvec3 q = c1->location - c->location;
				double currDst = length2(q - dot(q,n)*n);
				if (currDst >= maxDst)
				{
					maxDst = currDst;
					c3 = c;
				}
			}


			// furthest away from triangle c1,c2,c3
			Contact* c4;
			maxDst = 0;

			for (Contact *c : contacts)
			{
				//distance from triangle (c1,c2,c3) 

				dvec3 v0 = c2->location - c1->location;
				dvec3 v1 = c3->location - c1->location;
				dvec3 v2 = c->location - c1->location;

				double d00 = dot(v0, v0);
				double d01 = dot(v0, v1);
				double d11 = dot(v1, v1);
				double d20 = dot(v2, v0);
				double d21 = dot(v2, v1);
				double denom = d00 * d11 - d01 * d01;
				double v = (d11 * d20 - d01 * d21) / denom;
				double w = (d00 * d21 - d01 * d20) / denom;
				double u = 1.0f - v - w;

				Clamp(u,0,1);
				Clamp(v,0,1);
				Clamp(w,0,1);

				double currDst = length2(u*c->location + v*c->location + w*c->location - c->location);

				if (currDst >= maxDst)
				{
					maxDst = currDst;	
					c4 = c;
				}
			}


			for (auto it = contacts.begin(); it != contacts.end(); )
			{
				Contact *c = *it;
				if (c == c1 || c == c2 || c == c3 || c == c4)  // keep c1-c4
				{
					++it;
				}
				else
				{
					ContactPool::GetInstance().Recycle(c);
					it = contacts.erase(it);
				}
			}

			//PrintContacts();
		}

		int GetNumberOfContacts()
		{
			return contacts.size();
		}
		
		void PrintContacts()
		{
			std::cout << "ContactManifold:" << std::endl;
			for(Contact* c : contacts)
			{
				c->PrintContact();
			}
		}

		void Clamp(double &val, double min, double max)
		{
			if (val < min) val = min;	
			else if (val > max) val = max;	
		}
};


class ContactManifoldPool
{
	std::list<ContactManifold*> notUsed;
	int created = 0;

public:

	static ContactManifoldPool& GetInstance()
	{
		static ContactManifoldPool instance; // Guaranteed to be destroyed.
		return instance;
	}

	ContactManifoldPool()
	{
	}

	~ContactManifoldPool()
	{
		Clear();
	}

	void Clear()
	{
		for (ContactManifold* d : notUsed)
		{
			delete d;
		}
		notUsed.clear();
		created = 0;
	}

	void PrintInfo()
	{
		std::cout << "created: " << created << " not used: " << notUsed.size() << std::endl;
	}
	
	int Size()
	{
		return created;
	}

	ContactManifold* Get()
	{
		ContactManifold* p;
		if (notUsed.size() == 0) 
		{
			p = new ContactManifold();
			created++;
		}
		else
		{
			p = notUsed.back();
			notUsed.pop_back();
		}

		
		return p;
	}

	void Recycle(ContactManifold* c)
	{
		c->Clear();
		notUsed.push_back(c);
	}
};
