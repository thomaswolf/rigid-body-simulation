#pragma once

#include "Mesh.h"
#include "AABB.h"
#include <vector>
#include <list>


/*
 * Represents the actual physical form of an rigidbody for collision detection (container class).
 * We assume to shape to be convex, s.t. it is defined by its vertices.
 * The shape is in its own local coordinate system (around its own origin (0,0,0)) and will be transformed by the rigidbody if needed
 * (similar to Mesh and Model)
 */
class Shape
{

private:

	dvec3* vertices; // array of vertices 
	int nVertices; // number of elements in the array of vertices
	AABB aabb; // bounding box
	ShapeType type;

	void calculateAABB() 
	{
		vec3 min = vertices[0];
		vec3 max = min;

		for (size_t i=1; i<nVertices; ++i)
		{
			vec3 v = vertices[i];

			min.x = std::min(v.x, min.x);
			min.y = std::min(v.y, min.y);
			min.z = std::min(v.z, min.z);

			max.x = std::max(v.x, max.x);
			max.y = std::max(v.y, max.y);
			max.z = std::max(v.z, max.z);
		}
		
		aabb.Set(min, max);
	}
public:

	AABB& GetAABB() { return aabb; }
	ShapeType GetShapeType() { return type; }


	Shape(Mesh* mesh) 
	{
		std::vector<Vertex> meshVertices = mesh->GetVertices();

		nVertices = meshVertices.size();
		vertices = new dvec3[meshVertices.size()];

		for (size_t i=0; i<meshVertices.size(); ++i)
		{
			vertices[i] = meshVertices[i].Position;
		}

		this->type = mesh->GetShapeType();

		assert(nVertices >= 3 && "The shape needs to consist of at least 3 vertices.");

		calculateAABB();
	}

	~Shape()
	{
		delete[] vertices;
	}

	int GetNumberOfFaces()
	{
		return nVertices / 3;
	}

	// returns the vertices with the highest dot product with p, 
	// can be overwritten for implicit shape representations for example
	virtual void GetMultipleSupports(std::list<dvec4>& points, dvec3 p, double tol=1e-2)
	{
		dvec3 pointWithMaxProduct = GetSupport(p);
		GLfloat maxProduct = dot(pointWithMaxProduct, -p);

		// find with tolerance
		for (size_t i=0; i<nVertices; ++i)
		{
			GLfloat d = dot(vertices[i], p);
			if (d >= maxProduct - tol)
			{
				points.push_back(dvec4(vertices[i], 1.0));
			}
		}
	}

	// returns the vertex with the highest dot product with p, 
	// can be overwritten for implicit shape representations for example
	virtual dvec3 GetSupport(dvec3 p)
	{
		if (type == ShapeType::Sphere)
		{
			// the sphere is unit length -> we need only to normalize the direction
			return normalize(p);
		}
		else if (type == ShapeType::Box)
		{
			return dvec3((p.x>0 ? 1 : -1)*0.5,(p.y>0 ? 1 : -1)*0.5,(p.z>0 ? 1 : -1)*0.5);
		}

		GLfloat maxProduct = 0;
		vec3 pointWithMaxProduct;
	
		for (size_t i=0; i<nVertices; ++i)
		{
			GLfloat d = dot(vertices[i], p);
			if (d >= maxProduct)
			{
				maxProduct = d;
				pointWithMaxProduct = vertices[i];	
			}
		}

		// transform to world coordinates
		return pointWithMaxProduct;
	}

	dmat3 GetInertiaTensor(GLfloat mass, vec3 scale)
	{
		mat3 inertiaTensorBody;

		switch(type) 
		{
			case ShapeType::General:
			{
				/*// uniform scaling needed
				assert(scale[0] == scale[1] && scale[0] == scale[2] && "only uniform scaling is allowed for general meshes / shapes");
				GLfloat a = scale[0];
				// original inertiaTensor, e.g. precalculated
				inertiaTensorBody = mat3(1);	// TODO: how should we handle inertia tensors of general form? Specify them when importing the mesh? calculate them in a program. e.g. MeshLab, we could also use just the bounding box inertia tensor...
				// scaling by a^5, http://gazebosim.org/tutorials?tut=inertia&cat=
				inertiaTensorBody *= std::pow(a,5);
				break;*/
			//return IntegrateMesh(mesh);
				dvec3 com = CenterOfMass();
				dmat3 inertia =  Inertia(com);
				return inertia;

			}
			// https://en.wikipedia.org/wiki/List_of_moments_of_inertia
			case ShapeType::Point:	// point
			{
				inertiaTensorBody = mat3(1);
				break;
			}
			case ShapeType::Triangle :	// triangle
			{
				// not implemented / necessary
				// should we set them static?
				break;
			}
			case ShapeType::Plane :	// plane
			{
				// not implemented / necessary
				// should we set them static?
				break;
			}
			case ShapeType::Box :	// box / Solid cuboid 
			{
				GLfloat a = scale[0];
				GLfloat b = scale[2];
				GLfloat c = scale[1];
				vec3 d = {	mass*(b*b+c*c)/12.,
							mass*(a*a+c*c)/12.,
							mass*(a*a+b*b)/12.};
				inertiaTensorBody = diagonal3x3(d);
				break;
			}
			case ShapeType::Pyramid :	// pyramid
			{
				dvec3 com = CenterOfMass();
				inertiaTensorBody = Inertia(com);
				break;
			}
			// http://www.efunda.com/math/solids/solids_display.cfm?SolidName=EllipticalCylinder
			case ShapeType::Cylinder :	// cylinder or elliptic cylinder
			{
				GLfloat a = scale[0];
				GLfloat b = scale[2];
				GLfloat L = scale[1];
				vec3 d = {	mass*(b*b/4.+L*L/3.),
							mass*(a*a+b*b)/4.,
							mass*(a*a/4.+L*L/3.)};
				inertiaTensorBody = diagonal3x3(d);
				break;
			}
			case ShapeType::Sphere :	// sphere or ellipsoid
			{
				GLfloat a = scale[0];
				GLfloat b = scale[1];
				GLfloat c = scale[2];
				vec3 d = {	mass*(b*b + c*c)/5.,
							mass*(a*a + c*c)/5.,
							mass*(a*a + b*b)/5.};
				inertiaTensorBody = diagonal3x3(d);
				break;
			}
			case ShapeType::Lane :	// lane
			{
				GLfloat w = scale[2];
				GLfloat h = scale[1];
				GLfloat L = scale[0];
				vec3 d = {	mass*(w*w/4.+L*L/3.),
							mass*(h*h+w*w)/4.,
							mass*(h*h/4.+L*L/3.)};
				inertiaTensorBody = diagonal3x3(d);
				break;
			}
			default :	// not specialized bodies
			{
				assert(scale[0] == scale[1] && scale[0] == scale[2] && "inertia tensor of specialized body is not yet implemented");
				GLfloat a = scale[0];
				dvec3 com = CenterOfMass();
				inertiaTensorBody = Inertia(com);
				// scaling by a^5, http://gazebosim.org/tutorials?tut=inertia&cat=
				inertiaTensorBody *= std::pow(a,5);
				break;
			}
		}

		return inertiaTensorBody;
	}


	// volume integration adapted from http://melax.github.io/volint.html
	//
	double Volume()
	{
		// count is the number of triangles (tris) 
		double  volume=0;
		for(int i=0; i < GetNumberOfFaces(); i+=3)  // for each triangle
		{
			dvec3 v0 = vertices[i];
			dvec3 v1 = vertices[i+1];
			dvec3 v2 = vertices[i+2];
			volume += determinant(dmat3(v0,v1,v2)); //divide by 6 later for efficiency
		}
		return volume/6.0f;  // since the determinant give 6 times tetra volume
	}
 		

	// com integration adapted from http://melax.github.io/volint.html
	//
	dvec3 CenterOfMass()
	{
		// count is the number of triangles (tris) 
		dvec3 com(0,0,0);
		double  volume=0; // actually accumulates the volume*6

		for(int i=0; i < GetNumberOfFaces(); i+=3)  // for each triangle
		{
			dvec3 v0 = vertices[i];
			dvec3 v1 = vertices[i+1];
			dvec3 v2 = vertices[i+2];

			dmat3 A(v0,v1,v2);

			double vol=determinant(A);  // dont bother to divide by 6 

			//com += vol * (A.x+A.y+A.z);  // divide by 4 at end
			com += vol * (v0+v1+v2);
			volume+=vol;
		}
		com /= volume*4.0f; 

		return com;
	}

	// inertia integration adapted from http://melax.github.io/volint.html
	//
	dmat3 Inertia(dvec3 com)
	{
		// count is the number of triangles (tris) 
		// The moments are calculated based on the center of rotation com which you should calculate first
		// assume mass==1.0  you can multiply by mass later.
		// for improved accuracy the next 3 variables, the determinant d, and its calculation should be changed to double
		float  volume=0;                          // technically this variable accumulates the volume times 6
		dvec3 diag(0,0,0);                       // accumulate matrix main diagonal integrals [x*x, y*y, z*z]
		dvec3 offd(0,0,0);                       // accumulate matrix off-diagonal  integrals [y*z, x*z, x*y]

		for(int i=0; i < GetNumberOfFaces(); i+=3)  // for each triangle
		{
			dvec3 v0 = vertices[i];
			dvec3 v1 = vertices[i+1];
			dvec3 v2 = vertices[i+2];

			dmat3 A(v0-com, v1-com, v2-com); // matrix trick for volume calc by taking determinant
			float    d = determinant(A);  // vol of tiny parallelapiped= d * dr * ds * dt (the 3 partials of my tetral triple integral equasion)
			volume +=d;                   // add vol of current tetra (note it could be negative - that's ok we need that sometimes)

			for(int j=0;j < 3;j++)
			{
				int j1=(j+1)%3;   
				int j2=(j+2)%3;   
				diag[j] += (A[0][j]*A[1][j] + A[1][j]*A[2][j] + A[2][j]*A[0][j] + 
				            A[0][j]*A[0][j] + A[1][j]*A[1][j] + A[2][j]*A[2][j]  ) *d; // divide by 60.0f later;
				offd[j] += (A[0][j1]*A[1][j2]  + A[1][j1]*A[2][j2]  + A[2][j1]*A[0][j2]  +
				            A[0][j1]*A[2][j2]  + A[1][j1]*A[0][j2]  + A[2][j1]*A[1][j2]  +
				            A[0][j1]*A[0][j2]*2+ A[1][j1]*A[1][j2]*2+ A[2][j1]*A[2][j2]*2 ) *d; // divide by 120.0f later
			}
		}
		diag /= volume*(60.0f /6.0f);  // divide by total volume (vol/6) since density=1/volume
		offd /= volume*(120.0f/6.0f);
		return dmat3(diag.y+diag.z  , -offd.z      , -offd.y,
					-offd.z        , diag.x+diag.z, -offd.x,
					-offd.y        , -offd.x      , diag.x+diag.y );
	}

};
