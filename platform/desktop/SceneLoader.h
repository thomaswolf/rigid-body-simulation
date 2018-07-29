#pragma once

// based on: http://cs.dvc.edu/HowTo_Cparse.html

#include <iostream>
#include <fstream>
#include <string>
#include <boost/regex.hpp>

#include "Scene.h"


#define MAX_CHARS_PER_LINE 455
#define MAX_TOKENS_PER_LINE 30
#define DELIMITER " /"

class SceneLoader
{

private:

	Scene* scene;

	std::vector<Vertex> meshVertices;
	std::vector<vec3> vertices;
	std::vector<vec3> normals;

	std::unordered_map<std::string, vec3> materials;

	char nameLine[MAX_CHARS_PER_LINE] = {};
	std::string materialName;
	const char* line[MAX_TOKENS_PER_LINE] = {};
	char buf[MAX_CHARS_PER_LINE];

	bool colorize = false;

public:


	SceneLoader(Scene* scene, bool colorize = false)
	{
		this->scene = scene;
		this->colorize = colorize;
	}

	bool LoadObj(std::string name)
	{
		loadMaterial(name  + ".mtl");
		loadScene(name + ".obj");

	}

private:
	void loadScene(std::string name)
	{
		scene->Clear();
		
		// create a file-reading object
		std::ifstream fin;
		fin.open(name); // open a file
		std::cout << "open scene file: " << name << std::endl;
		if (!fin.good()) assert(false && "file not found");

		// read each line of the file
		while (!fin.eof())
		{
			// read an entire line into memory
			clearBuf();
			fin.getline(buf, MAX_CHARS_PER_LINE);

			// parse the line into blank-delimited tokens
			int n = 0; // a for-loop index


			// parse the line
			line[0] = strtok(buf, DELIMITER); // first line
			if (line[0]) // zero if line is blank
			{
				for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
			  	{
					line[n] = strtok(0, DELIMITER); // subsequent tokens

					if (!line[n]) break; // no more tokens
			  	}

				if 		(strcmp(line[0],"o") == 0) parseObject();
				else if (strcmp(line[0],"g") == 0) parseGroup();
				else if	(strcmp(line[0],"v") == 0) parseVertex();
				else if (strcmp(line[0],"vn") == 0) parseNormal();
				else if (strcmp(line[0],"f") == 0) parseFace();
				else if (strcmp(line[0],"usemtl") == 0) parseUseMaterial();
			}
		}

		createMesh();

		fin.close();
	}


	void loadMaterial(std::string name)
	{
		// create a file-reading object
		std::ifstream fin;
		fin.open(name); // open a file
		std::cout << "open material file: " << name << std::endl;
		if (!fin.good())
		{
			std::cout << "material file not found";
			return;
		}

		// read each line of the file
		while (!fin.eof())
		{
			// read an entire line into memory
			clearBuf();
			fin.getline(buf, MAX_CHARS_PER_LINE);

			// parse the line into blank-delimited tokens
			int n = 0; // a for-loop index


			// parse the line
			line[0] = strtok(buf, DELIMITER); // first line
			if (line[0]) // zero if line is blank
			{
				for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
			  	{
					line[n] = strtok(0, DELIMITER); // subsequent tokens

					if (!line[n]) break; // no more tokens
			  	}

				if 		(strcmp(line[0],"newmtl") == 0) newMaterial();
				else if	(strcmp(line[0],"Kd") == 0) parseColor(); // we only parse diffuse color 
			}
		}

		fin.close();
	}


	void newMaterial()
	{
		materialName = line[1];
		std::cout << "read material " << materialName << std::endl;
	}


	void parseColor()
	{
		vec3 color(atof(line[1]), atof(line[2]), atof(line[3]));
		materials[materialName] = color;
		std::cout << to_string(materials[materialName]) << std::endl;
	}

	void parseGroup()
	{
		createMesh();
		strncpy(nameLine, line[1], MAX_CHARS_PER_LINE); // we parse the group name as name -> all objects have same attributes 
		meshVertices.clear();
	}

	void createMesh()
	{
		if (meshVertices.size() > 0)
		{
			std::cout << "create " << nameLine << std::endl;

			// parse name parameters
			bool dynamic =  strstr(nameLine, "move") != NULL;
			bool deco = 	strstr(nameLine, "deco") != NULL;
			float friction = 	readFloatParam("f(\\d_\\d+)", 0.5);
			float restitution = readFloatParam("r(\\d_\\d+)", 0.7);
			float mass = 		readFloatParam("m(\\d+_\\d+)", 1);


			// center vertices
			vec3 pos(0,0,0);
			for (Vertex &v: meshVertices)
			{
				pos += v.Position;
			}
			pos /= meshVertices.size();

			// add color
			vec3 color(1);
			if (materials.find(materialName) == materials.end())
			{
				std::cout << "material " << materialName << " not found" << std::endl;

				if (colorize)
				{
					if (dynamic)
					{
						if (mass == 0.5)
						{
							color = vec3(0,0.3,0.7);
						}
						else
						{
							color = vec3(1,1,0);
						}
					}
				}
			}
			else
			{
				color = materials[materialName];			
			}
			
			for (Vertex &v: meshVertices)
			{
				v.Position -= pos;
				v.Color = color;
			}

			// calculate normals
			if (normals.size() == 0)
			{
				for (int i=0; i<meshVertices.size(); i=i+3)
				{
					vec3 v1 = meshVertices[i].Position;
					vec3 v2 = meshVertices[i+1].Position;
					vec3 v3 = meshVertices[i+2].Position;
					vec3 normal = cross(v2-v1,v3-v1);
					meshVertices[i].Normal = normal;
					meshVertices[i+1].Normal = normal;
					meshVertices[i+2].Normal = normal;
				}
			}


			std::cout << "mass="<< mass << " friction="<< friction << " restitution=" << restitution << std::endl;

			std::vector<GLuint> I;
			Mesh* mesh = new Mesh(meshVertices, I, GL_TRIANGLES, ShapeType::General);
			Model* model = NULL;

			if (dynamic)
			{
				std::cout << "dynamic" << std::endl;
				RigidBodyModel* temp = new RigidBodyModel(mesh, pos);
				temp->GetRigidBody()->SetFriction(friction);
				temp->GetRigidBody()->SetRestitution(restitution);
				temp->GetRigidBody()->SetMass(mass);
				model = temp;
			}
			else if (deco)
			{
				model = new Model(mesh, pos);
			}
			else
			{
				RigidBodyModel* temp = new RigidBodyModel(mesh, pos);
				temp->GetRigidBody()->SetStatic();
				temp->GetRigidBody()->SetFriction(friction);
				temp->GetRigidBody()->SetRestitution(restitution);
				model = temp;
			}


			model->SetRotation(vec3(0,0,0));
			scene->AddEntity(model);
		}
	}

	void parseObject()
	{
		createMesh(); // create mesh parsed until now
		strncpy(nameLine, line[1], MAX_CHARS_PER_LINE);
		meshVertices.clear();
	}

	void parseVertex()
	{
		dvec3 v(atof(line[1]), atof(line[2]), atof(line[3]));
		vertices.push_back(v);
	}

	void parseNormal()
	{
		dvec3 n(atof(line[1]), atof(line[2]), atof(line[3]));
		normals.push_back(n);
	}

	void parseUseMaterial()
	{
		materialName = line[1];
	}

	void parseFace()
	{

		for (int i=0; i<3; ++i)
		{
			int v1 = atoi(line[2*i+1]) - 1;
			int n1 = atoi(line[2*i+2]) - 1;

			Vertex v;
			v.Position = vertices[v1];

			if (normals.size() != 0)
			{
				v.Normal = normals[n1];
			}
			v.Color = vec3(1,1,1);
		
			meshVertices.push_back(v);
		}
	}

	// helper
	//
	
	float readFloatParam(std::string rex, float def)
	{
		boost::regex pattern(rex);
		boost::smatch result;
		if (boost::regex_search(std::string(nameLine), result, pattern))
		{
			std::string res = result[1].str();
			std::replace(res.begin(), res.end(), '_', '.');
			return stof(res);
		}
		return def;
	}

	void clearBuf()
	{
		memset(buf, 0, sizeof(buf));
	}
};
