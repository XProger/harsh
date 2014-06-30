  /*=================================================================}
 /  <<<<<<<<<<<<<<<<<<<<<--- Harsh Engine --->>>>>>>>>>>>>>>>>>>>>>  }
/  e-mail  : xproger@list.ru                                         }
{  github  : https://github.com/xproger/harsh                        }
{====================================================================}
{ LICENSE:                                                           }
{ Copyright (c) 2013, Timur "XProger" Gagiev                         }
{ All rights reserved.                                               }
{                                                                    }
{ Redistribution and use in source and binary forms, with or without /
{ modification, are permitted under the terms of the BSD License.   /
{=================================================================*/
#ifndef SCENE_H
#define SCENE_H

#include "utils.h"
#include "resource.h"

enum NodeType { NT_GROUP, NT_MODEL, NT_MESH, NT_WALKMAP, NT_LIGHT };

//{ Scene Node (base)
struct SceneNode {
    SceneNode *parent, *child, *prev, *next;

    Box rel_bbox, bbox;
    mat4 rel_matrix, matrix;
	bool visible;

    SceneNode(SceneNode *parent, Stream *stream);
    virtual ~SceneNode();

    void setParent(SceneNode *parent);
    void setMatrix(const mat4 &rel_matrix);
    void updateBounds();

    SceneNode *addChild(SceneNode *n);
    SceneNode *removeChild(SceneNode *n);

    virtual void update();
    virtual void render();

    void add(SceneNode *n);
    void remove();
};
//}

//{ Mesh Node
struct Mesh : public SceneNode {
protected:
//    static quat2 jointCache[64];
//    static Mesh *sortList;
//    Mesh *sortPrev, *sortNext;
public:
	MeshRes *res;
    Material *material;
//	JointIndex *jMap;

	Mesh(SceneNode *parent, Hash hash);
	virtual ~Mesh();
	virtual void render();
//	void remap();
//	void renderQuery();
//	static void sortRender();
};
//}

//{ Model Node
/*
struct Skeleton : public SceneNode {
	//
};

struct Model : public SceneNode {
	Model(SceneNode *parent, Hash hash);
};
*/
//}

//{ Camera
#define CAMERA_MODE_FREE    0
#define CAMERA_MODE_TARGET  1
#define CAMERA_MODE_LOOKAT  2
#define CAMERA_MODE_GAME	3

struct Camera : public SceneNode {
    int mode;
    float zNear, zFar, FOV, dist;

    vec3 pos, angle, target;
    vec4 planes[6];
	mat4 mView, mProj, mViewProj;

    Camera(Stream *stream = NULL);
    virtual ~Camera();

	virtual void update();
    void updateMatrix();
    void updatePlanes();
	void debugUpdate(float speed);
    void setup();
    bool checkVisible(const Box &v);
	vec3 toWorld(const vec2 &screenPos, const vec4 &plane);
	vec4 pProject(const vec3 &pos);
	vec3 pUnProject(const vec4 &pos);
};
//}

//{ Scene
struct Scene : public SceneNode {
    Camera *camera;

    Scene();
    virtual ~Scene();
	void load(const char *name);
	void checkVisible();
	virtual void update();
    virtual void render();
};
//}

#endif
