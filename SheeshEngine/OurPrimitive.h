#pragma once

class Mesh;
class GameObject;

enum class GeometryType {
	EMPTY,
	P_PLANE,
	P_CUBE,
	P_SPHERE,
	P_CYLINDER,
	SUZANNE,
	ICOSHPERE,
	CONE,
	P_TORUS,
	NONE
};

class PrimitivesGeomtriesLibrary
{
public:
	static GameObject* InstanciatePrimitiveGeometry(GeometryType type);

private:

};


//holi : )
