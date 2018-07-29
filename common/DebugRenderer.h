#pragma once

#include "Model.h"
#include "Mesh.h"
#include <functional>
#include <list>
#include <unordered_map>


/*
 * Model that is only a single point which is convienient to draw debug information.
 */
class DebugPoint : public Model
{
private:
		GLfloat size;
		vec3 color;

public:
	DebugPoint(vec3 pos, vec3 color, GLfloat size = 10) : Model(MeshGenerator::CreatePoint(color), pos)
	{
		this->size = size;
		this->color = color;
	}

	void SetSize(GLfloat size) { this->size = size; }

	void SetColor(vec3 c)
	{
		if (c != color)
		{
			assert(false);
			this->color = c;
			delete mesh;
			mesh = MeshGenerator::CreatePoint(c);
		}
	}
	
	virtual void Draw(Shader& shader) 
	{
		#ifdef PLATFORM_DESKTOP
		glPointSize(this->size);
		#endif
		Model::Draw(shader);
	}
};

class DebugBox : public Model
{
	vec3 color;
public:
	DebugBox(vec3 pos, vec3 color) : Model(MeshGenerator::CreateBox(color), pos)
	{
		this->mesh->SetDrawMode(GL_LINE_LOOP);
		this->color = color;
	}

	void SetColor(vec3 c)
	{
		if (c!= color)
		{
			assert(false);
			this->color = c;
			delete mesh;
			mesh = MeshGenerator::CreateBox(c);
			this->mesh->SetDrawMode(GL_LINE_LOOP);
		}
	}
};

template <class T>
class DebugModelPool
{
	std::list<T*> notUsed;
	std::list<T*> toDraw;

public:
	DebugModelPool()
	{
	}

	~DebugModelPool()
	{
		Clear();
	}

	void Clear()
	{
		for (T* d : notUsed)
		{
			delete d;
		}
		for (T* d : toDraw)
		{
			delete d;
		}
		notUsed.clear();
		toDraw.clear();
	}


	void Draw(Shader& shader)
	{
		for (T* p : toDraw)
		{
			p->Draw(shader);
			notUsed.push_back(p);
		}

		toDraw.clear();
	}	

	int Size()
	{
		return notUsed.size() + toDraw.size();
	}

	T* AddObject(std::function<T*()> alloc)
	{
		T* p;
		if (notUsed.size() == 0) 
		{
			p = alloc();
		}
		else
		{
			p = notUsed.back();
			notUsed.pop_back();
		}
		
		toDraw.push_back(p);

		return p;
	}
};


/*
 * Static helper class used by render manager to draw additional points in the scene
 */
class DebugRenderer
{
private:
	bool enabled = false;

	std::unordered_map<vec3, DebugModelPool<DebugPoint>*, Vec3Op> points;
	std::unordered_map<vec3, DebugModelPool<DebugBox>*, Vec3Op> boxes;

public:

    static DebugRenderer* Instance ()
    {
       static CGuard g;   // Speicherbereinigung
       if (!_instance)
          _instance = new DebugRenderer ();
       return _instance;
    }

	void Enable()
	{
		this->enabled = true;
	}

	void Disable()
	{
		this->enabled = false;
	}

	void AddDebugPoint(vec3 pos, vec3 color, GLfloat size)
	{
		#ifdef PLATFORM_DESKTOP
		if (!enabled) return;

		if (points[color] == NULL)
			points[color] = new DebugModelPool<DebugPoint>();

		DebugPoint* p = points[color]->AddObject([&]() { return new DebugPoint(pos, color, size); });
		p->SetPosition(pos);
		p->SetSize(size);
		p->SetColor(color);
		#endif
	}

	void AddDebugBox(vec3 pos, vec3 color, vec3 scale)
	{
		if (!enabled) return;

		if (boxes[color] == NULL)
			boxes[color] = new DebugModelPool<DebugBox>();

		DebugBox* p = boxes[color]->AddObject([&]() { return new DebugBox(pos, color); });
		p->SetPosition(pos);
		p->SetScale(scale);
		p->SetColor(color);
	}

	void Draw(Shader& shader)
	{
		if (!enabled) return;
		for (const std::pair<vec3, DebugModelPool<DebugPoint>*> p : points)
		{
			p.second->Draw(shader);
		}
		for (const std::pair<vec3, DebugModelPool<DebugBox>*> p : boxes)
		{
			p.second->Draw(shader);
		}

		//std::cout << "boxes = " << boxes.Size() << " points = " << points.Size() << std::endl;
	}


private:

	// singleton stuff
	//
	
    static DebugRenderer* _instance;
    DebugRenderer () { } /* verhindert, dass ein Objekt von außerhalb von DebugRenderer erzeugt wird. */
              // protected, wenn man von der Klasse noch erben möchte
    DebugRenderer ( const DebugRenderer& ); /* verhindert, dass eine weitere Instanz via
 Kopie-Konstruktor erstellt werden kann */
    ~DebugRenderer () 
	{ 
		for (const std::pair<vec3, DebugModelPool<DebugPoint>*> p : points)
		{
			delete p.second;
		}
		for (const std::pair<vec3, DebugModelPool<DebugBox>*> p : boxes)
		{
			delete p.second;
		}
		points.clear();
		boxes.clear();
	}

    class CGuard
    {
    public:
       ~CGuard()
       {
          if( NULL != DebugRenderer::_instance )
          {
             delete DebugRenderer::_instance;
             DebugRenderer::_instance = NULL;
          }
       }
    };
};
DebugRenderer* DebugRenderer::_instance = 0; /* statische Elemente einer Klasse müssen initialisiert werden. */
