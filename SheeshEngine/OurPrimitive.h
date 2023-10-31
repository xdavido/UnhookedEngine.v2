#pragma once

class Mesh;
class GameObject;

enum class ShapeType {
	EMPTY,
	P_PLANE,
	P_CUBE,
	P_SPHERE,
	P_CYLINDER,
	MONKEY,
	ICOSHPERE,
	CONE,
	P_TORUS,
};

class OurPrimitive
{
public:
	static GameObject* CreatePrimitive(ShapeType type);

private:

};

