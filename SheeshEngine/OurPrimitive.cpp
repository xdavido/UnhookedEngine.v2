#include "OurPrimitive.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentMesh.h"

GameObject* OurPrimitive::CreatePrimitive(ShapeType type)
{
	GameObject* gameObject = new GameObject();
	// Empty
	if (type == ShapeType::EMPTY)
	{
		gameObject->name = "Empty Object";
		return gameObject;
	}

	// Not Empty
	switch (type)
	{
    case ShapeType::P_PLANE:
		gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/plane.fbx");

		gameObject->name = "Plane";
		return gameObject;
		break;
	case ShapeType::P_CUBE:
		gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/cube.fbx");

		gameObject->name = "Cube";
		return gameObject;

		break;
    case ShapeType::P_SPHERE:
        gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/sphere.fbx");
        gameObject->name = "Sphere";
        return gameObject;

    case ShapeType::P_CYLINDER:
        gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/cylinder.fbx");
        gameObject->name = "Cylinder";
        return gameObject;

    case ShapeType::MONKEY:
        gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/monkey.fbx");
        gameObject->name = "Monkey";
        return gameObject;

    case ShapeType::ICOSHPERE:
        gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/icosphere.fbx");
        gameObject->name = "Icosphere";
        return gameObject;

    case ShapeType::CONE:
        gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/cone.fbx");
        gameObject->name = "Cone";
        return gameObject;

    case ShapeType::P_TORUS:
        gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/torus.fbx");
        gameObject->name = "Torus";
        return gameObject;

    default:
        // Handle the case when an unsupported shape type is provided.
        // You can throw an exception, log an error, or return a default object.
        // Here, we'll return nullptr to indicate an error.
        return nullptr;

	}
}