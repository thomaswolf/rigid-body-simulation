/*
* Simple mesh to store vertices, indices and textures based on: http://learnopengl.com/#!Model-Loading/Mesh
* Stores data in VBOs automatically
* Vertices are in its own local coordinate system around (0,0,0) and will be transformed by the model to world coordinates
*/

//TODO: implement mesh loader (from file)

#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "Shader.h"

struct Vertex {
	vec3 Position;
	vec3 Color;
	vec3 Normal;
	vec3 TexCoords;
};

struct Texture {
    GLuint id;
	std::string type;
};

enum ShapeType
{
	General, Point, Triangle, Plane, Box, Pyramid, Cylinder, Sphere, Lane
};

class Mesh {

	private:

		// our mesh consists of vertices. Either 3 consecutive vertices define a triangle or 
		// if indices are given, the triangles are created in the order of the indices
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;
		GLint drawMode = GL_TRIANGLES;

		ShapeType type;


    public:

		// opengl buffer handles
		GLuint 	VAO,    // vertex array name
			  	VBO,  	// vertex buffer
				EBO, 	// buffer for indices
				MBO; 	// buffer to store model matrices (multiple for instancing)

		std::vector<Vertex> GetVertices() { return vertices; }


        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, GLint drawMode= GL_TRIANGLES, ShapeType type = ShapeType::General)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;
			this->drawMode = drawMode;
			this->type = type;

			this->setupMesh();
		}

        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, GLint drawMode = GL_TRIANGLES, ShapeType type = ShapeType::General)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->drawMode = drawMode;
			this->type = type;

			this->setupMesh();
		}

		int GetNumberOfFaces()
		{
			return this->vertices.size() / 3;
		}

		Vertex GetVertex(int index)
		{
			return this->vertices[index];
		}

		void SetDrawMode(GLint mode) { this->drawMode = mode; }

        void Draw(Shader& shader, GLuint instances = 1)
		{
			GLuint diffuseNr = 1;
			GLuint specularNr = 1;
			for(GLuint i = 0; i < this->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
				// Retrieve texture number (the N in diffuse_textureN)
				std::stringstream ss;
				std::string number;
				std::string name = this->textures[i].type;
				if(name == "texture_diffuse")
					ss << diffuseNr++; // Transfer GLuint to stream
				else if(name == "texture_specular")
					ss << specularNr++; // Transfer GLuint to stream
				number = ss.str(); 

				glUniform1f(glGetUniformLocation(shader.Program, ("material." + name + number).c_str()), i);
				glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
			}
			if (this->textures.size() > 0) glActiveTexture(GL_TEXTURE0);

			// Draw mesh
			glBindVertexArray(this->VAO);

			if (this->indices.size() > 0)
			{
				// draw triangles by indices
				glDrawElementsInstanced(this->drawMode, this->indices.size(), GL_UNSIGNED_INT, 0, instances);
			}
			else
			{
				// draw triangles (every three vertices form a triangle)
				glDrawArraysInstanced(this->drawMode, 0, this->vertices.size(), instances);
			}

			glBindVertexArray(0);
		}
		
		ShapeType GetShapeType(){return type; }


    private:

        void setupMesh()
		{
			// create buffers
			glGenVertexArrays(1, &this->VAO);
			glGenBuffers(1, &this->VBO);
		  
			glBindVertexArray(this->VAO);
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO); // binds buffer to active vertex buffer

			// store vertex data
			glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), 
						 &this->vertices[0], GL_STATIC_DRAW);  

			if (this->indices.size() > 0)
			{
				glGenBuffers(1, &this->EBO);

				// store index data
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), 
						&this->indices[0], GL_STATIC_DRAW);
			}

			// bind data to the respective location in the vertex shader

			// Vertex Positions
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
								 (GLvoid*)0);
			// Vertex Normals
			glEnableVertexAttribArray(1);	
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
								 (GLvoid*)offsetof(Vertex, Normal));
			// Vertex Colors
			glEnableVertexAttribArray(2);	
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
								 (GLvoid*)offsetof(Vertex, Color));

			// Vertex Texture Coords
			glEnableVertexAttribArray(3);	
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
								 (GLvoid*)offsetof(Vertex, TexCoords));



			glBindVertexArray(0);

			glBindVertexArray(this->VAO);
			// create model instance buffer
			glGenBuffers(1, &this->MBO);
			glBindBuffer(GL_ARRAY_BUFFER, this->MBO);


			// model instance matrix
			glEnableVertexAttribArray(4); 
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
			glEnableVertexAttribArray(5); 
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(6); 
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(7); 
			glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);
			glVertexAttribDivisor(7, 1);

			glBindVertexArray(0);

	}
};

// creates some hardcoded meshes
class MeshGenerator
{

	public:

		static Mesh* CreatePoint(vec3 color = vec3(1.0f))
		{
			std::vector<Vertex> V;
			V.push_back({vec3( 0.0f,  0.0f,  0.0f), color, vec3(0.0f, 0.0f, 0.0f)});
			std::vector<GLuint> I;
			return new Mesh(V, I, GL_POINTS, ShapeType::Point);
		}

		static Mesh* CreateTriangle(vec3 color = vec3(1.0f))
		{
			std::vector<Vertex> V;
			V.push_back({vec3(-0.5f, -0.5f,  0.0f),	color, vec3(0.f, 0.f, 1.0f)});
			V.push_back({vec3( 0.5f, -0.5f,  0.0f), color, vec3(0.f, 0.f, 1.0f)});
			V.push_back({vec3( 0.0f,  0.5f,  0.0f), color, vec3(0.f, 0.f, 1.0f)});

			V.push_back({vec3(-0.5f, -0.5f,  0.0f),	color, vec3(0.f, 0.f, -1.0f)});
			V.push_back({vec3( 0.5f, -0.5f,  0.0f), color, vec3(0.f, 0.f, -1.0f)});
			V.push_back({vec3( 0.0f,  0.5f,  0.0f), color, vec3(0.f, 0.f, -1.0f)});

			std::vector<GLuint> I = {0,1,2};

			return new Mesh(V, I, GL_TRIANGLES, ShapeType::Triangle);
		}

		static Mesh* CreatePlane(vec3 color = vec3(1.0f))
		{
			std::vector<Vertex> V;
			V.push_back({vec3(-1,0, -1), color, vec3(0.f, 1.f, 0.0f) } );
			V.push_back({vec3( 1,0,  1),  color, vec3(0.f, 1.f, 0.0f)  } );
			V.push_back({vec3( 1,0, -1),  color, vec3(0.f, 1.f, 0.0f) } );

			V.push_back({vec3( 1,0,  1),  color, vec3(0.f, 1.f, 0.0f)  } );
			V.push_back({vec3(-1,0, -1), color, vec3(0.f, 1.f, 0.0f) } );
			V.push_back({vec3(-1,0,  1),  color, vec3(0.f, 1.f, 0.0f)  } );

			//std::vector<GLuint> I = {0,1,2, 2,3,0};
			std::vector<GLuint> I;

			return new Mesh(V, I, GL_TRIANGLES, ShapeType::Plane);
		}

		static Mesh* CreateRamp(vec3 color = vec3(1.0f))
		{
			std::vector<Vertex> V;
			V.push_back({vec3(-1,0, -1), color, vec3(-0.4f,	2.0f,	0.0f) } );
			V.push_back({vec3( 1,-0.8, -1),  color, vec3(-0.4f,	2.0f,	0.0f) } );
			V.push_back({vec3( 1,-0.8,  1),  color, vec3(-0.4f,	2.0f,	0.0f)  } );

			V.push_back({vec3( 1,-0.8,  1),  color, vec3(-0.4f,	2.0f,	0.0f)  } );
			V.push_back({vec3(-1,0,  1),  color, vec3(-0.4f,	2.0f,	0.0f)  } );
			V.push_back({vec3(-1,0, -1), color, vec3(-0.4f,	2.0f,	0.0f) } );

			//std::vector<GLuint> I = {0,1,2, 2,3,0};
			std::vector<GLuint> I;

			return new Mesh(V, I,GL_TRIANGLES, ShapeType::General);
		}

		static Mesh* CreateBoxOneSidedColored(vec3 color, vec3 color1)
		{
			vec3 V1 = vec3(-0.5f, -0.5f, -0.5f);
			vec3 V2 = vec3( 0.5f, -0.5f, -0.5f);
			vec3 V3 = vec3(-0.5f,  0.5f, -0.5f);
			vec3 V4 = vec3( 0.5f,  0.5f, -0.5f);
			vec3 V5 = vec3(-0.5f, -0.5f,  0.5f);
			vec3 V6 = vec3( 0.5f, -0.5f,  0.5f);
			vec3 V7 = vec3(-0.5f,  0.5f,  0.5f);
			vec3 V8 = vec3( 0.5f,  0.5f,  0.5f);
			
			Vertex vertices[] = {
				// back
				{V2, color1, vec3( 0.0f,  0.0f, -1.0f)},
				{V1, color1, vec3( 0.0f,  0.0f, -1.0f)},
				{V4, color1, vec3( 0.0f,  0.0f, -1.0f)},
				{V4, color1, vec3( 0.0f,  0.0f, -1.0f)},
				{V1, color1, vec3( 0.0f,  0.0f, -1.0f)},
				{V3, color1, vec3( 0.0f,  0.0f, -1.0f)},

				// front
				{V5, color1, vec3( 0.0f,  0.0f,  1.0f)},
				{V6, color1, vec3( 0.0f,  0.0f,  1.0f)},
				{V8, color1, vec3( 0.0f,  0.0f,  1.0f)},
				{V8, color1, vec3( 0.0f,  0.0f,  1.0f)},
				{V7, color1, vec3( 0.0f,  0.0f,  1.0f)},
				{V5, color1, vec3( 0.0f,  0.0f,  1.0f)},

				// right
				{V7, color1, vec3(-1.0f,  0.0f,  0.0f)},
				{V3, color1, vec3(-1.0f,  0.0f,  0.0f)},
				{V1, color1, vec3(-1.0f,  0.0f,  0.0f)},
				{V1, color1, vec3(-1.0f,  0.0f,  0.0f)},
				{V5, color1, vec3(-1.0f,  0.0f,  0.0f)},
				{V7, color1, vec3(-1.0f,  0.0f,  0.0f)},

				// left
				{V8, color, vec3( 1.0f,  0.0f,  0.0f)},
				{V2, color, vec3( 1.0f,  0.0f,  0.0f)},
				{V4, color, vec3( 1.0f,  0.0f,  0.0f)},
				{V6, color, vec3( 1.0f,  0.0f,  0.0f)},
				{V2, color, vec3( 1.0f,  0.0f,  0.0f)},
				{V8, color, vec3( 1.0f,  0.0f,  0.0f)},

				// top
				{V1, color, vec3( 0.0f, -1.0f,  0.0f)},
				{V2, color, vec3( 0.0f, -1.0f,  0.0f)},
				{V6, color, vec3( 0.0f, -1.0f,  0.0f)},
				{V6, color, vec3( 0.0f, -1.0f,  0.0f)},
				{V5, color, vec3( 0.0f, -1.0f,  0.0f)},
				{V1, color, vec3( 0.0f, -1.0f,  0.0f)},

				// bottom
				{V4, color, vec3( 0.0f,  1.0f,  0.0f)},
				{V3, color, vec3( 0.0f,  1.0f,  0.0f)},
				{V8, color, vec3( 0.0f,  1.0f,  0.0f)},
				{V7, color, vec3( 0.0f,  1.0f,  0.0f)},
				{V8, color, vec3( 0.0f,  1.0f,  0.0f)},
				{V3, color, vec3( 0.0f,  1.0f,  0.0f)}
			};

			std::vector<Vertex> V;
			V.assign(vertices, vertices + 36);
			std::vector<GLuint> I;

			return new Mesh(V,I,GL_TRIANGLES, ShapeType::Box);
		}

		static Mesh* CreateBox(vec3 color = vec3(1.0f))
		{
			vec3 V1 = vec3(-0.5f, -0.5f, -0.5f);
			vec3 V2 = vec3( 0.5f, -0.5f, -0.5f);
			vec3 V3 = vec3(-0.5f,  0.5f, -0.5f);
			vec3 V4 = vec3( 0.5f,  0.5f, -0.5f);
			vec3 V5 = vec3(-0.5f, -0.5f,  0.5f);
			vec3 V6 = vec3( 0.5f, -0.5f,  0.5f);
			vec3 V7 = vec3(-0.5f,  0.5f,  0.5f);
			vec3 V8 = vec3( 0.5f,  0.5f,  0.5f);
			
			Vertex vertices[] = {
				// back
				{V2, color, vec3( 0.0f,  0.0f, -1.0f)},
				{V1, color, vec3( 0.0f,  0.0f, -1.0f)},
				{V4, color, vec3( 0.0f,  0.0f, -1.0f)},
				{V4, color, vec3( 0.0f,  0.0f, -1.0f)},
				{V1, color, vec3( 0.0f,  0.0f, -1.0f)},
				{V3, color, vec3( 0.0f,  0.0f, -1.0f)},

				// front
				{V5, color, vec3( 0.0f,  0.0f,  1.0f)},
				{V6, color, vec3( 0.0f,  0.0f,  1.0f)},
				{V8, color, vec3( 0.0f,  0.0f,  1.0f)},
				{V8, color, vec3( 0.0f,  0.0f,  1.0f)},
				{V7, color, vec3( 0.0f,  0.0f,  1.0f)},
				{V5, color, vec3( 0.0f,  0.0f,  1.0f)},

				// right
				{V7, color, vec3(-1.0f,  0.0f,  0.0f)},
				{V3, color, vec3(-1.0f,  0.0f,  0.0f)},
				{V1, color, vec3(-1.0f,  0.0f,  0.0f)},
				{V1, color, vec3(-1.0f,  0.0f,  0.0f)},
				{V5, color, vec3(-1.0f,  0.0f,  0.0f)},
				{V7, color, vec3(-1.0f,  0.0f,  0.0f)},

				// left
				{V8, color, vec3( 1.0f,  0.0f,  0.0f)},
				{V2, color, vec3( 1.0f,  0.0f,  0.0f)},
				{V4, color, vec3( 1.0f,  0.0f,  0.0f)},
				{V6, color, vec3( 1.0f,  0.0f,  0.0f)},
				{V2, color, vec3( 1.0f,  0.0f,  0.0f)},
				{V8, color, vec3( 1.0f,  0.0f,  0.0f)},

				// top
				{V1, color, vec3( 0.0f, -1.0f,  0.0f)},
				{V2, color, vec3( 0.0f, -1.0f,  0.0f)},
				{V6, color, vec3( 0.0f, -1.0f,  0.0f)},
				{V6, color, vec3( 0.0f, -1.0f,  0.0f)},
				{V5, color, vec3( 0.0f, -1.0f,  0.0f)},
				{V1, color, vec3( 0.0f, -1.0f,  0.0f)},

				// bottom
				{V4, color, vec3( 0.0f,  1.0f,  0.0f)},
				{V3, color, vec3( 0.0f,  1.0f,  0.0f)},
				{V8, color, vec3( 0.0f,  1.0f,  0.0f)},
				{V7, color, vec3( 0.0f,  1.0f,  0.0f)},
				{V8, color, vec3( 0.0f,  1.0f,  0.0f)},
				{V3, color, vec3( 0.0f,  1.0f,  0.0f)}
			};

			std::vector<Vertex> V;
			V.assign(vertices, vertices + 36);
			std::vector<GLuint> I;

			return new Mesh(V,I,GL_TRIANGLES, ShapeType::Box);
		}

		static Mesh* CreatePyramid(vec3 color = vec3(1.0f))
		{
			std::vector<Vertex> V;

			
			// bottom
			V.push_back({vec3( 0.5f,  0.5f, -0.5f), color, vec3( 0.0f,  1.0f,  0.0f)});
			V.push_back({vec3(-0.5f,  0.5f, -0.5f), color, vec3( 0.0f,  1.0f,  0.0f)});
			V.push_back({vec3( 0.5f,  0.5f,  0.5f), color, vec3( 0.0f,  1.0f,  0.0f)});
			V.push_back({vec3(-0.5f,  0.5f,  0.5f), color, vec3( 0.0f,  1.0f,  0.0f)});
			V.push_back({vec3( 0.5f,  0.5f,  0.5f), color, vec3( 0.0f,  1.0f,  0.0f)});
			V.push_back({vec3(-0.5f,  0.5f, -0.5f), color, vec3( 0.0f,  1.0f,  0.0f)});

			// front
			V.push_back({vec3( 0.5f,  0.5f,  0.5f), color, vec3(0.0f,-0.707f,0.707f)});
			V.push_back({vec3(-0.5f,  0.5f,  0.5f), color, vec3(0.0f,-0.707f,0.707f)});
			V.push_back({vec3( 0.0f,  0.0f,  0.0f), color, vec3(0.0f,-0.707f,0.707f)});

			// left
			V.push_back({vec3( 0.5f,  0.5f, -0.5f), color, vec3( 0.707f,-0.707f,0.0f)});
			V.push_back({vec3( 0.5f,  0.5f,  0.5f), color, vec3( 0.707f,-0.707f,0.0f)});
			V.push_back({vec3( 0.0f,  0.0f,  0.0f), color, vec3( 0.707f,-0.707f,0.0f)});

			// back
			V.push_back({vec3(-0.5f,  0.5f, -0.5f), color, vec3(0.0f, -0.707f,-0.707f)});
			V.push_back({vec3( 0.5f,  0.5f, -0.5f), color, vec3(0.0f, -0.707f,-0.707f)});
			V.push_back({vec3( 0.0f,  0.0f,  0.0f), color, vec3(0.0f, -0.707f,-0.707f)});

			// right
			V.push_back({vec3(-0.5f,  0.5f, -0.5f), color, vec3(0.707f,0.707f,0.0f)});
			V.push_back({vec3( 0.0f,  0.0f,  0.0f), color, vec3(0.707f,0.707f,0.0f)});
			V.push_back({vec3(-0.5f,  0.5f,  0.5f), color, vec3(0.707f,0.707f,0.0f)});

			std::vector<GLuint> I;

			return new Mesh(V,I,GL_TRIANGLES, ShapeType::Pyramid);
		}


		static Mesh* CreateCylinder(vec3 color = vec3(1.0f), int noApprox = 100)
		{
			std::vector<Vertex> V;
			
			double x_old=0, z_old=1;
//			int noApprox = 100;
			for (int i = noApprox-1; i>=0; --i) {
				double angle = M_PI_2*(double(i)/noApprox);
				double x = std::cos(angle);
				double z = std::sin(angle);

				// first quarter
				V.push_back({vec3(x_old, 0.5f,z_old), color, vec3((x+x_old)/2,0.f,(z+z_old)/2)});
				V.push_back({vec3(x_old,-0.5f,z_old), color, vec3((x+x_old)/2,0.f,(z+z_old)/2)});
				V.push_back({vec3(x,-0.5f,z), color, vec3((x+x_old)/2,0.f,(z+z_old)/2)});
				
				V.push_back({vec3(x,-0.5f,z), color, vec3((x+x_old)/2,0.f,(z+z_old)/2)});
				V.push_back({vec3(x, 0.5f,z), color, vec3((x+x_old)/2,0.f,(z+z_old)/2)});
				V.push_back({vec3(x_old, 0.5f,z_old), color, vec3((x+x_old)/2,0.f,(z+z_old)/2)});


				// second quarter
				V.push_back({vec3(-x,-0.5f,z), color, vec3(-(x+x_old)/2,0.f,(z+z_old)/2)});
				V.push_back({vec3(-x_old,-0.5f,z_old), color, vec3(-(x+x_old)/2,0.f,(z+z_old)/2)});
				V.push_back({vec3(-x_old, 0.5f,z_old), color, vec3(-(x+x_old)/2,0.f,(z+z_old)/2)});
				
				V.push_back({vec3(-x_old, 0.5f,z_old), color, vec3(-(x+x_old)/2,0.f,(z+z_old)/2)});
				V.push_back({vec3(-x, 0.5f,z), color, vec3(-(x+x_old)/2,0.f,(z+z_old)/2)});
				V.push_back({vec3(-x,-0.5f,z), color, vec3(-(x+x_old)/2,0.f,(z+z_old)/2)});


				// third quarter
				V.push_back({vec3(x,-0.5f,-z), color, vec3((x+x_old)/2,0.f,-(z+z_old)/2)});
				V.push_back({vec3(x_old,-0.5f,-z_old), color, vec3((x+x_old)/2,0.f,-(z+z_old)/2)});
				V.push_back({vec3(x_old, 0.5f,-z_old), color, vec3((x+x_old)/2,0.f,-(z+z_old)/2)});
				
				V.push_back({vec3(x_old, 0.5f,-z_old), color, vec3((x+x_old)/2,0.f,-(z+z_old)/2)});
				V.push_back({vec3(x, 0.5f,-z), color, vec3((x+x_old)/2,0.f,-(z+z_old)/2)});
				V.push_back({vec3(x,-0.5f,-z), color, vec3((x+x_old)/2,0.f,-(z+z_old)/2)});


				// fourth quarter
				V.push_back({vec3(-x_old, 0.5f,-z_old), color, vec3(-(x+x_old)/2,0.f,-(z+z_old)/2)});
				V.push_back({vec3(-x_old,-0.5f,-z_old), color, vec3(-(x+x_old)/2,0.f,-(z+z_old)/2)});
				V.push_back({vec3(-x,-0.5f,-z), color, vec3(-(x+x_old)/2,0.f,-(z+z_old)/2)});
				
				V.push_back({vec3(-x,-0.5f,-z), color, vec3(-(x+x_old)/2,0.f,-(z+z_old)/2)});
				V.push_back({vec3(-x, 0.5f,-z), color, vec3(-(x+x_old)/2,0.f,-(z+z_old)/2)});
				V.push_back({vec3(-x_old, 0.5f,-z_old), color, vec3(-(x+x_old)/2,0.f,-(z+z_old)/2)});
				
				
				// bottom
				V.push_back({vec3(x_old, 0.5f,z_old), color, vec3(0.f, 1.f, 0.f)});
				V.push_back({vec3(x, 	 0.5f,z	  ), color, vec3(0.f, 1.f, 0.f)});
				V.push_back({vec3(0.f,	 0.5f,0.f  ), color, vec3(0.f, 1.f, 0.f)});

				V.push_back({vec3(-x, 	 0.5f,z	  ), color, vec3(0.f, 1.f, 0.f)});
				V.push_back({vec3(-x_old,0.5f,z_old), color, vec3(0.f, 1.f, 0.f)});
				V.push_back({vec3(0.f,	 0.5f,0.f  ), color, vec3(0.f, 1.f, 0.f)});

				V.push_back({vec3(x, 	 0.5f,-z	  ), color, vec3(0.f, 1.f, 0.f)});
				V.push_back({vec3(x_old, 0.5f,-z_old),color, vec3(0.f, 1.f, 0.f)});
				V.push_back({vec3(0.f,	 0.5f,0.f  ), color, vec3(0.f, 1.f, 0.f)});

				V.push_back({vec3(-x_old,0.5f,-z_old),color, vec3(0.f, 1.f, 0.f)});
				V.push_back({vec3(-x, 	 0.5f,-z	  ), color, vec3(0.f, 1.f, 0.f)});
				V.push_back({vec3(0.f,	 0.5f,0.f  ), color, vec3(0.f, 1.f, 0.f)});


				// top
				V.push_back({vec3(x, 	-0.5f,z	  ), color, vec3(0.f,-1.f, 0.f)});
				V.push_back({vec3(x_old,-0.5f,z_old), color, vec3(0.f,-1.f, 0.f)});
				V.push_back({vec3(0.f,	-0.5f,0.f  ), color, vec3(0.f,-1.f, 0.f)});

				V.push_back({vec3(-x_old,-0.5f,z_old),color, vec3(0.f,-1.f, 0.f)});
				V.push_back({vec3(-x, 	 -0.5f,z	  ), color, vec3(0.f,-1.f, 0.f)});
				V.push_back({vec3(0.f,	 -0.5f,0.f  ),color, vec3(0.f,-1.f, 0.f)});

				V.push_back({vec3(x_old,-0.5f,-z_old),color, vec3(0.f,-1.f, 0.f)});
				V.push_back({vec3(x, 	-0.5f,-z	  ), color, vec3(0.f,-1.f, 0.f)});
				V.push_back({vec3(0.f,	-0.5f,0.f  ), color, vec3(0.f,-1.f, 0.f)});

				V.push_back({vec3(-x, 	 -0.5f,-z   ), color, vec3(0.f,-1.f, 0.f)});
				V.push_back({vec3(-x_old,-0.5f,-z_old),color, vec3(0.f,-1.f, 0.f)});
				V.push_back({vec3(0.f,	 -0.5f,0.f  ), color, vec3(0.f,-1.f, 0.f)});


				x_old = x;
				z_old = z;				
			}

			std::vector<GLuint> I;

			return new Mesh(V,I,GL_TRIANGLES,ShapeType::Cylinder);
		}


		static Mesh* CreateSphere(vec3 color = vec3(1.0f), int noApprox = 50)
		{
			std::vector<Vertex> V;
			
			double x1_old=0, y1_old=0;
			double x2_old=0, y2_old=0;
			
			// iterate around the Sphere, at angle theta1 and theta2.
			// We need both angles in order to be able to calculate the triangles without having to store lots of old x- and y-values.
			//
			// ---------------x-------------------x--------------- z2
			//              P2_old				 P2
			//
			// --------------x---------------------x-------------- z1
			//              P1_old 				   P1
			//
			/*for (int i = noApprox; i>0; --i) {
				double theta1 = M_PI*(double(i)/noApprox);				
				double z1 = std::cos(theta1);

				double theta2 = M_PI*(double(i-1)/noApprox);				
				double z2 = std::cos(theta2);
				

				for (int j = noApprox; j>=0; --j) {
					double phi = 2*M_PI*(double(j)/noApprox);
					
					double x1 = std::sin(theta1) * std::cos(phi);
					double y1 = std::sin(theta1) * std::sin(phi);
					double x2 = std::sin(theta2) * std::cos(phi);
					double y2 = std::sin(theta2) * std::sin(phi);

					// two triangles which form a quadrilateral
					V.push_back({vec3(x1,y1,z1), color, vec3((x1+x2)/2,(y1+y2)/2,(z1+z2)/2)});
					V.push_back({vec3(x1_old, y1_old, z1), color, vec3((x1+x2)/2,(y1+y2)/2,(z1+z2)/2)});
					V.push_back({vec3(x2_old,y2_old,z2), color, vec3((x1+x2)/2,(y1+y2)/2,(z1+z2)/2)});

					V.push_back({vec3(x1,y1,z1), color, vec3((x1+x2)/2,(y1+y2)/2,(z1+z2)/2)});
					V.push_back({vec3(x2_old, y2_old, z2), color, vec3((x1+x2)/2,(y1+y2)/2,(z1+z2)/2)});
					V.push_back({vec3(x2,y2,z2), color, vec3((x1+x2)/2,(y1+y2)/2,(z1+z2)/2)});

					
					x1_old = x1;
					y1_old = y1;
					x2_old = x2;
					y2_old = y2;
				}
			}

			// smooth normals
			for (int i=0; i<	*/
	
			int rings = noApprox;
			int sectors = noApprox;
			float const R = 1./(float)(rings-1);
			float const S = 1./(float)(sectors-1);
			int r, s;
			float radius = 1;

			std::vector<GLuint> I;
			for(int r = 0; r < rings; ++r) {
				for(int s = 0; s < sectors; ++s) {
					float const y = sin( -M_PI_2 + M_PI * r * R );
					float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
					float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

					V.push_back({vec3(x,y,z) * radius, color, vec3(x,y,z)});

					if(r < rings-1)
					{
					    int curRow = r * sectors;
						int nextRow = (r+1) * sectors;
						int nextS = (s+1) % sectors;

						I.push_back(curRow + s);
						I.push_back(nextRow + s);
						I.push_back(nextRow + nextS);

						I.push_back(curRow + s);
						I.push_back(nextRow + nextS);
						I.push_back(curRow + nextS);
					}
				}
			}


			return new Mesh(V,I,GL_TRIANGLES, ShapeType::Sphere);
		}


		static Mesh* CreateLane(vec3 color = vec3(1.0f))
		{
			std::vector<Vertex> V;
			
			double y1_old=0, z1_old=1;
			double y2_old=0, z2_old=1.1;
			int noApprox = 100;
			for (int i = noApprox-1; i>=0; --i) {
				double angle = M_PI_2*(double(i)/noApprox);
				double y1 = std::cos(angle);
				double z1 = std::sin(angle);
				double y2 = 1.1 * std::cos(angle);
				double z2 = 1.1 * std::sin(angle);

				// inside (y1, z1)
				// first quarter
				V.push_back({vec3( 0.5f,y1_old,z1_old), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});
				V.push_back({vec3(-0.5f,y1_old,z1_old), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});
				V.push_back({vec3(-0.5f,y1,z1), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});
				
				V.push_back({vec3(-0.5f,y1,z1), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});
				V.push_back({vec3(0.5f,y1,z1), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});
				V.push_back({vec3(0.5f,y1_old,z1_old), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});

				// second quarter
				V.push_back({vec3(- 0.5f,y1_old,-z1_old), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});
				V.push_back({vec3(0.5f,y1_old,-z1_old), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});
				V.push_back({vec3(0.5f,y1,-z1), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});
				
				V.push_back({vec3(0.5f,y1,-z1), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});
				V.push_back({vec3(-0.5f,y1,-z1), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});
				V.push_back({vec3(-0.5f,y1_old,-z1_old), color, vec3(0.0f,(y1+y1_old)/2,(z1+z1_old)/2)});


				// outside (y2, z2)
				// first quarter
				V.push_back({vec3( 0.5f,y2_old,z2_old), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});
				V.push_back({vec3(-0.5f,y2,z2), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});
				V.push_back({vec3(-0.5f,y2_old,z2_old), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});
				
				V.push_back({vec3(-0.5f,y2,z2), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});
				V.push_back({vec3(0.5f,y2_old,z2_old), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});
				V.push_back({vec3(0.5f,y2,z2), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});

				// second quarter
				V.push_back({vec3(- 0.5f,y2_old,-z2_old), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});
				V.push_back({vec3(0.5f,y2,-z2), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});
				V.push_back({vec3(0.5f,y2_old,-z2_old), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});
				
				V.push_back({vec3(0.5f,y2,-z2), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});
				V.push_back({vec3(-0.5f,y2_old,-z2_old), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});
				V.push_back({vec3(-0.5f,y2,-z2), color, -vec3(0.0f,(y2+y2_old)/2,(z2+z2_old)/2)});


				// bottom
				V.push_back({vec3(-0.5f,y2_old,z2_old), color, vec3(-1.f,0.f,0.f)});
				V.push_back({vec3(-0.5f,y2,z2), color, vec3(-1.f,0.f,0.f)});
				V.push_back({vec3(-0.5f,y1,z1), color, vec3(-1.f,0.f,0.f)});

				V.push_back({vec3(-0.5f,y1,z1), color, vec3(-1.f,0.f,0.f)});
				V.push_back({vec3(-0.5f,y1_old,z1_old), color, vec3(-1.f,0.f,0.f)});
				V.push_back({vec3(-0.5f,y2_old,z2_old), color, vec3(-1.f,0.f,0.f)});

				V.push_back({vec3(-0.5f,y2_old,-z2_old), color, vec3(-1.f,0.f,0.f)});
				V.push_back({vec3(-0.5f,y1,-z1), color, vec3(-1.f,0.f,0.f)});
				V.push_back({vec3(-0.5f,y2,-z2), color, vec3(-1.f,0.f,0.f)});

				V.push_back({vec3(-0.5f,y1,-z1), color, vec3(-1.f,0.f,0.f)});
				V.push_back({vec3(-0.5f,y2_old,-z2_old), color, vec3(-1.f,0.f,0.f)});
				V.push_back({vec3(-0.5f,y1_old,-z1_old), color, vec3(-1.f,0.f,0.f)});


				// top
				V.push_back({vec3(0.5f,y2_old,z2_old), color, vec3(1.f,0.f,0.f)});
				V.push_back({vec3(0.5f,y1,z1), color, vec3(1.f,0.f,0.f)});
				V.push_back({vec3(0.5f,y2,z2), color, vec3(1.f,0.f,0.f)});

				V.push_back({vec3(0.5f,y1,z1), color, vec3(1.f,0.f,0.f)});
				V.push_back({vec3(0.5f,y2_old,z2_old), color, vec3(1.f,0.f,0.f)});
				V.push_back({vec3(0.5f,y1_old,z1_old), color, vec3(1.f,0.f,0.f)});

				V.push_back({vec3(0.5f,y2_old,-z2_old), color, vec3(1.f,0.f,0.f)});
				V.push_back({vec3(0.5f,y2,-z2), color, vec3(1.f,0.f,0.f)});
				V.push_back({vec3(0.5f,y1,-z1), color, vec3(1.f,0.f,0.f)});

				V.push_back({vec3(0.5f,y1,-z1), color, vec3(1.f,0.f,0.f)});
				V.push_back({vec3(0.5f,y1_old,-z1_old), color, vec3(1.f,0.f,0.f)});
				V.push_back({vec3(0.5f,y2_old,-z2_old), color, vec3(1.f,0.f,0.f)});


				y1_old = y1;
				z1_old = z1;				
				y2_old = y2;
				z2_old = z2;				
			}
			
			
			// borders
			V.push_back({vec3(0.5f,0.0f,1.0f), color, vec3(0.f,1.f,0.f)});
			V.push_back({vec3(0.5f,0.0f,1.1f), color, vec3(0.f,1.f,0.f)});
			V.push_back({vec3(-0.5f,0.0f,1.0f), color, vec3(0.f,1.f,0.f)});

			V.push_back({vec3(0.5f,0.0f,1.1f), color, vec3(0.f,1.f,0.f)});
			V.push_back({vec3(-0.5f,0.0f,1.1f), color, vec3(0.f,1.f,0.f)});
			V.push_back({vec3(-0.5f,0.0f,1.0f), color, vec3(0.f,1.f,0.f)});

			V.push_back({vec3(0.5f,0.0f,-1.0f), color, vec3(0.f,1.f,0.f)});
			V.push_back({vec3(-0.5f,0.0f,-1.0f), color, vec3(0.f,1.f,0.f)});
			V.push_back({vec3(0.5f,0.0f,-1.1f), color, vec3(0.f,1.f,0.f)});

			V.push_back({vec3(0.5f,0.0f,-1.1f), color, vec3(0.f,1.f,0.f)});
			V.push_back({vec3(-0.5f,0.0f,-1.0f), color, vec3(0.f,1.f,0.f)});
			V.push_back({vec3(-0.5f,0.0f,-1.1f), color, vec3(0.f,1.f,0.f)});



			std::vector<GLuint> I;

			return new Mesh(V,I,GL_TRIANGLES,ShapeType::Lane);
		}

};
