#pragma once


/*
 * Axis aligned bounding box for broad phase collision detection 
 */
class AABB
{
	
	// we also save the vertices in homogenious coordinates for easy transformation
	dvec4 vertices[8];

public:

	dvec3 min;
	dvec3 max;

	const dvec3 GetMin()
	{
		return min;
	}

	const dvec3 GetMax()
	{
		return max;
	}

	dvec3 GetPosition()
	{
		return (min + max) / 2.0;
	}

	dvec3 GetScale()
	{
		return (max - min);
	}

	void Set(const AABB &other)
	{
		this->min = other.min;
		this->max = other.max;

		vertices[0] = other.vertices[0];
		vertices[1] = other.vertices[1];
		vertices[2] = other.vertices[2];
		vertices[3] = other.vertices[3];
		vertices[4] = other.vertices[4];
		vertices[5] = other.vertices[5];
		vertices[6] = other.vertices[6];
		vertices[7] = other.vertices[7];
	}

	void Set(dvec3 min, dvec3 max)
	{
		this->min = min;
		this->max = max;

		vertices[0] = vec4(min.x, min.y, min.z, 1);
		vertices[1] = vec4(min.x, min.y, max.z, 1);
		vertices[2] = vec4(min.x, max.y, min.z, 1);
		vertices[3] = vec4(min.x, max.y, max.z, 1);
		vertices[4] = vec4(max.x, min.y, min.z, 1);
		vertices[5] = vec4(max.x, min.y, max.z, 1);
		vertices[6] = vec4(max.x, max.y, min.z, 1);
		vertices[7] = vec4(max.x, max.y, max.z, 1);
	}

	// transforms to space given by M
	void Transform(const dmat4& M)
	{
		for (size_t i=0; i<8; ++i)
		{
			vertices[i] = M*vertices[i];
		}
		
		min = vertices[0]; max = vertices[0];

		for (size_t i=1; i<8; ++i)
		{
			dvec4 v = vertices[i];

			min.x = std::min(v.x, min.x);
			min.y = std::min(v.y, min.y);
			min.z = std::min(v.z, min.z);

			max.x = std::max(v.x, max.x);
			max.y = std::max(v.y, max.y);
			max.z = std::max(v.z, max.z);
		}
	}

	bool IntersectsWith(AABB& b)
	{
		return !(b.min.x > max.x
			|| b.min.y > max.y
			|| b.min.z > max.z
			|| b.max.x < min.x
			|| b.max.y < min.y
			|| b.max.z < min.z);
	}
};
