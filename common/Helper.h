#pragma once

// hashing for pairs
// (source: https://www.quora.com/How-can-I-declare-an-unordered-set-of-pair-of-int-int-in-C++11)

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
 
namespace std
{
  template<typename S, typename T> struct hash<pair<S, T>>
  {
    inline size_t operator()(const pair<S, T> & v) const
    {
      size_t seed = 0;
      ::hash_combine(seed, v.first);
      ::hash_combine(seed, v.second);
      return seed;
    }
  };
}

struct Vec3Op
{
    size_t operator()(const vec3& k)const
    {
		size_t seed = 0;
		::hash_combine(seed, k.x);
		::hash_combine(seed, k.y);
		::hash_combine(seed, k.z);
		return seed;
    }

    bool operator()(const vec3& a, const vec3& b)const
    {
        return a == b;
    }
};

struct IVec3Op
{
    size_t operator()(const ivec3& k)const
    {
		size_t seed = 0;
		::hash_combine(seed, k.x);
		::hash_combine(seed, k.y);
		::hash_combine(seed, k.z);
		return seed;
    }

    bool operator()(const ivec3& a, const ivec3& b)const
    {
        return a == b;
    }
};



/*
 * Stores combinations of Rigid Bodies
 */
#include "RigidBody.h"
#include "RigidBodyModel.h"

#include <vector>

// just a few combos which are used often
class Combos
{
private:
public:
	static Entity* Lane ()
	{
		// Lane with Combo
		RigidBodyModel* rightborder = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.3,.8,.7)), vec3(0.55,2.3,0));
		rightborder->SetScale(vec3(0.1,.5,1));
		rightborder->SetStatic();

		RigidBodyModel* middle = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.3,.8,.7)), vec3(0,2,0));
		middle->SetScale(vec3(1,0.1,1));
		middle->SetStatic();
		
		RigidBodyModel* leftborder = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.3,.8,.7)), vec3(-0.55,2.3,0));
		leftborder->SetScale(vec3(0.1,.5,1));
		leftborder->SetStatic();

		Entity* combo = new Entity(vec3(0,0,0));
		combo->AddChild(rightborder);
		combo->AddChild(middle);
		combo->AddChild(leftborder);
		
		return combo;
	}


	static Entity* Lane1 ()
	{
		// Lane with Combo
		RigidBodyModel* rightborder = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.3,.8,.7)), vec3(0.55,2,0));
		rightborder->SetScale(vec3(0.1,1,6));
		rightborder->SetRotation(vec3(radians(25.0f),0.f,0.f));
		rightborder->SetStatic();

		RigidBodyModel* middle = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.3,.8,.7)), vec3(0,2,0));
		middle->SetScale(vec3(2.5,0.1,6));
		middle->SetRotation(vec3(radians(25.0f),0.f,0.f));
		middle->SetStatic();
		
		RigidBodyModel* leftborder = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.3,.8,.7)), vec3(-0.55,2,0));
		leftborder->SetScale(vec3(0.1,1,6));
		leftborder->SetRotation(vec3(radians(25.0f),0,0));
		leftborder->SetStatic();

		Entity* combo = new Entity(vec3(0,0,0));
		combo->AddChild(rightborder);
		combo->AddChild(middle);
		combo->AddChild(leftborder);
		
		return combo;
	}

	static Entity* Lane2 ()
	{
		// Lane with Combo
		RigidBodyModel* rightborder = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.3,.8,.7)), vec3(0,2,0.55));
		rightborder->SetScale(vec3(6,1,0.1));
		rightborder->SetRotation(vec3(0.f,0.f,radians(-25.0f)));
		rightborder->SetStatic();

		RigidBodyModel* middle = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.3,.8,.7)), vec3(0,2,0));
		middle->SetScale(vec3(6,0.1,2.5));
		middle->SetRotation(vec3(0.f,0.f,radians(-25.0f)));
		middle->SetStatic();
		
		RigidBodyModel* leftborder = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.3,.8,.7)), vec3(0,2,-0.55));
		leftborder->SetScale(vec3(6,1,0.1));
		leftborder->SetRotation(vec3(0,0,radians(-25.0f)));
		leftborder->SetStatic();

		Entity* combo = new Entity(vec3(0,0,0));
		combo->AddChild(rightborder);
		combo->AddChild(middle);
		combo->AddChild(leftborder);
		
		return combo;
	}

	static Entity* LaneIncline()
	{
		// Lane with Combo
		RigidBodyModel* rightramp = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.8,.7,.2)), vec3(0.75,0.3,0));
		rightramp->SetScale(vec3(6,0.1,14));
		rightramp->SetRotation(vec3(0.f,0.f,radians(25.0f)));
		rightramp->SetStatic();

		RigidBodyModel* middleramp = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.7,.1,.9)), vec3(0,0.09,0));
		middleramp->SetScale(vec3(3,0.1,14));
		middleramp->SetStatic();
		
		RigidBodyModel* leftramp = new RigidBodyModel(MeshGenerator::CreateBox(vec3(.4,.8,.2)), vec3(-0.75,0.3,0));
		leftramp->SetScale(vec3(6,0.1,14));
		leftramp->SetRotation(vec3(0,0,radians(-25.0f)));
		leftramp->SetStatic();

		Entity* combo = new Entity(vec3(0,0.5,2));
		combo->AddChild(rightramp);
		combo->AddChild(middleramp);
		combo->AddChild(leftramp);

		return combo;
	}
};
