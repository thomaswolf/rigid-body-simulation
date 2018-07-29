/*
 * Container class representing a edge/edge or face/vertex collision between the shapes of rigidbodies
 */

#pragma once

class RigidBody; // forward declaration
class ContactConstraint; 

#define COLLISION_THRESHOLD 0.1 // max vrel until detected as colliding


enum ContactType
{
	Diverging, Colliding
};

//TODO: very rough design so far
class Contact 
{

public:
		RigidBody* bodyA;
		RigidBody* bodyB;

		dvec3 normal;
		dvec3 location; // location on a face of body a
		dvec3 localLocation; // location in shape space of body a

		dvec3 locationB; // location on a face of body b
		dvec3 localLocationB; // location in shape space of body b

		dvec3 tangent1;
		dvec3 tangent2;

		ContactConstraint* constraint;

		double depth;


		// calculated via Update()
		dvec3 vA;
		dvec3 vB;
		double vRel;
		ContactType type;

		// implemented in ContactConstraint because of dependency
		Contact();
		~Contact();
		void ClearConstraint();

		// calculates vA, vB, vRel and type and needs to be updated every time the velocity or angular velocity of one of the bodies changed
		void Update(); // defined in rigidbody.h
		void SetData(RigidBody* a, RigidBody* b, dvec3 normal, dvec3 loc, double depth); // defined in RigidBody.h
		
		void PrintContact(); // defined in RigidBody.h

		void SetNormal(dvec3 normal)
		{
			this->normal = normal;
			normal = normalize(normal);

			// calculate tangents (orthonormal basis with normal)
			// http://box2d.org/2014/02/computing-a-basis/
			
			if (std::abs(normal.x) >= 0.57735f)
				tangent1 = vec3(normal.y, -normal.x, 0.0f);
			else
				tangent1 = vec3(0.0f, normal.z, -normal.y);
			 
			tangent1 = normalize(tangent1);
			tangent2 = cross(normal,tangent1);
		}
};

class ContactPool
{
	std::list<Contact*> notUsed;
	int created = 0;

public:

	static ContactPool& GetInstance()
	{
		static ContactPool instance; // Guaranteed to be destroyed.
		return instance;
	}

	ContactPool()
	{
	}

	~ContactPool()
	{
		Clear();
	}

	void Clear()
	{
		for (Contact* d : notUsed)
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

	Contact* Get()
	{
		Contact* p;
		if (notUsed.size() == 0) 
		{
			p = new Contact();
			created++;
		}
		else
		{
			p = notUsed.back();
			notUsed.pop_back();
		}

		
		return p;
	}

	void Recycle(Contact* c)
	{
		notUsed.push_back(c);
	}
};
