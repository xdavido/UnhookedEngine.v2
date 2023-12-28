#include "OurPrimitive.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentMesh.h"

GameObject* PrimitivesGeomtriesLibrary::InstanciatePrimitiveGeometry(GeometryType type)
{
	
	if (type == GeometryType::EMPTY)
	{
        GameObject* gameObject = new GameObject(App->scene->root);
		gameObject->name = "Empty Object";
        gameObject->Stype = GeometryType::EMPTY;
		return gameObject;
	}


    if (type == GeometryType::CAMERA)
    {
        GameObject* gameObject = new GameObject(App->scene->root);
        gameObject->name = "Camera";
        ComponentCamera* cc = new ComponentCamera();
        gameObject->AddComponent(cc);
        return gameObject;
    }
    else
    {
        const char* aux = " ";
        GameObject* gameObject = new GameObject();
        switch (type)
        {
        case GeometryType::P_PLANE:
            gameObject = App->assimpMeshes->LoadFile("Assets/Models/plane.fbx");
            aux = "Plane";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_PLANE;
            return gameObject;
            break;
        case GeometryType::P_CUBE:
            gameObject = App->assimpMeshes->LoadFile("Assets/Models/cube.fbx");
            aux = "Cube";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_CUBE;
            return gameObject;

            break;
        case GeometryType::P_CYLINDER:
            gameObject = App->assimpMeshes->LoadFile("Assets/Models/cylinder.fbx");
            aux = "Cylinder";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_CYLINDER;
            return gameObject;
            break;
        case GeometryType::P_SPHERE:
            gameObject = App->assimpMeshes->LoadFile("Assets/Models/sphere.fbx");
            aux = "Sphere";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_SPHERE;
            return gameObject;
            break;

        case GeometryType::P_ICOSHPERE:
            gameObject = App->assimpMeshes->LoadFile("Assets/Models/icosphere.fbx");
            aux = "Icosphere";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_ICOSHPERE;
            return gameObject;
            break;

        case GeometryType::P_CONE:
            gameObject = App->assimpMeshes->LoadFile("Assets/Models/cone.fbx");
            aux = "Cone";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_CONE;
            return gameObject;
            break;

        case GeometryType::P_TORUS:
            gameObject = App->assimpMeshes->LoadFile("Assets/Models/torus.fbx");
            aux = "Torus";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_TORUS;
            return gameObject;
            break;
        case GeometryType::P_SUZANNE:
            gameObject = App->assimpMeshes->LoadFile("Assets/Models/suzanne.fbx");
            aux = "Suzanne";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_SUZANNE;
            return gameObject;
            break;

        default:

            return nullptr;

        }
    }
   
}