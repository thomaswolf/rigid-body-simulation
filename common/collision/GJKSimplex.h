#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <collision/MinowskiPoint.h>
#include <collision/EPAPolytope.h>


/* 
 * Simplex helper class of up to 4-Simplex for the GJK algorithm
 * Processing similar to: http://hacktank.net/blog/?p=93
 * https://www.toptal.com/game/video-game-physics-part-ii-collision-detection-for-solid-objects
 */
class GJKSimplex 
{

private:
	MinowskiPoint points[4];
	int dim = 0; // dimension of simplex

public:
	GJKSimplex() {}

	EPAPolytope ConvertToEPAPolytope()
	{
		assert(dim == 4); // we create the EPA tetrahedron only at a final state of the GJK simplex
		EPAPolytope s;
		s.AddTriangle(points[0], points[1], points[2]);
		s.AddTriangle(points[0], points[2], points[3]);
		s.AddTriangle(points[0], points[3], points[1]);
		s.AddTriangle(points[1], points[3], points[2]);
		return s;
	}

	void SetPoints(MinowskiPoint a) 
	{
		dim =1; points[0] = a; 
	}

	void SetPoints(MinowskiPoint a, MinowskiPoint b)
	{
		dim =2;
	   	points[0] = a; 
		points[1] = b;
	}

	void SetPoints(MinowskiPoint a, MinowskiPoint b, MinowskiPoint c)
	{
		dim =3;
	   	points[0] = a; 
		points[1] = b;
		points[2] = c;
	}

	void SetPoints(MinowskiPoint a, MinowskiPoint b, MinowskiPoint c, MinowskiPoint d)
	{
		dim = 4;
	   	points[0] = a; 
		points[1] = b;
		points[2] = c;
		points[3] = d;
	}

	void PushVertex(MinowskiPoint v)
	{
		for (int i=0; i<dim; ++i) assert(points[i].p != v.p);
		if (dim < 4)
		{
			// set most recent point at index 0, shift others
			dim++;
			for (int i=dim-1; i>0; --i) points[i] = points[i-1];
			points[0] = v;
		}
		else assert(false);
	}


	// finds the direction to the origin from the nearest point on the simplex from the origin on the simplex
	bool HasOriginInside(dvec3& dir)
	{
		dvec3 ao = -points[0].p; // vector from a to origin

		switch(dim)
		{
			case 0:  
			case 1:  // we have only a point
				return false;

			case 2:  // we have a line; choose the direction pointing to the origin
			{
				dvec3 ab = points[1].p - points[0].p; // vector from a to b

				dvec3 abao = cross(ab,ao);

				if (abao.x == 0 && abao.y == 0 && abao.z == 0) // the origin lies in out line between p0 and p1
				{
					// we choose any other direction
					abao.x += 1;
				}
				dir = cross(abao, ab); // dir is perpendicular to ab and parallel to ao

				return false;
			}

			case 4:
			{
				dvec3 ab = points[1].p - points[0].p; // vector from a to b
				dvec3 ac = points[2].p - points[0].p; // vector from a to c

				dvec3 abcn = cross(ab,ac); // normal of abc triangle
				if (dot(abcn, ao) > 0)
				{
					// origin is in front of abc
					dim = 3;
					HasOriginInside(dir);
					return false;
				}

				dvec3 ad = points[3].p - points[0].p; // vector from a to d
				dvec3 acdn = cross(ac,ad); // normal of acd triangle

				if (dot(acdn, ao) > 0)
				{
					// origin is in front of abc
					SetPoints(points[0], points[2], points[3]);
					HasOriginInside(dir);
					return false;
				}

				dvec3 adbn = cross(ad,ab); // normal of acd triangle

				if (dot(adbn, ao) > 0)
				{
					// origin is in front of abc
					SetPoints(points[0], points[3], points[1]);
					HasOriginInside(dir);
					return false;
				}
			
				// we are inside the tetraeder
				return true;
			}
			
			case 3: // we have a triangle
			{
				dvec3 ab = points[1].p - points[0].p; // vector from a to b
				dvec3 ac = points[2].p - points[0].p; // vector from a to c
				dvec3 n = cross(ab,ac); // normal of triangle
				
				dvec3 abn = cross(ab,n); // normal of side ab
				if (dot(abn, ao) > 0)
				{
					// origin is outside the triangle at side ab
					dim = 2; // continue only with a and b
					dir = cross(cross(ab, ao), ab);
					return false;
				}

				dvec3 acn = cross(n, ac); // normal of side ac
				if (dot(acn, ao) > 0)
				{
					// origin is outside the triangle at side ab
					SetPoints(points[0], points[2]); // continue only with a and c
					dir = cross(cross(ac, ao), ac);
					return false;
				}

				// origin is inside the triangle
				
				if (dot(n,ao) > 0) 
				{
					// origin is above the triangle
					dir = n;
				}
				else 
				{
					SetPoints(points[0], points[2], points[1]);
					dir = -n;
				}

				return false;
			}

		}

	}
};
