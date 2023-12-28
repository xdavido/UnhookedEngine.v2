#pragma once

class Mesh;
class GameObject;

enum class GeometryType {
	EMPTY,
	P_PLANE,
	P_CUBE,
	P_SPHERE,
	P_CYLINDER,
	P_SUZANNE,
	P_ICOSHPERE,
	P_CONE,
	P_TORUS,
	CAMERA,
	EMITTER,
	NONE
};

class PrimitivesGeomtriesLibrary
{
public:
	static GameObject* InstanciatePrimitiveGeometry(GeometryType type);

private:

};


//holi : )
