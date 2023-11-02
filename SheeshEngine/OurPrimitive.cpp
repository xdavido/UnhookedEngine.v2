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
    else
    {
        const char* aux = " ";
        GameObject* gameObject = new GameObject();
        switch (type)
        {
        case GeometryType::P_PLANE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/plane.fbx");
            aux = "Plane";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_PLANE;
            return gameObject;
            break;
        case GeometryType::P_CUBE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/cube.fbx");
            aux = "Cube";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_CUBE;
            return gameObject;

            break;
        case GeometryType::P_CYLINDER:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/cylinder.fbx");
            aux = "Cylinder";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_CYLINDER;
            return gameObject;
            break;
        case GeometryType::P_SPHERE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/sphere.fbx");
            aux = "Sphere";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_SPHERE;
            return gameObject;
            break;

        case GeometryType::P_ICOSHPERE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/icosphere.fbx");
            aux = "Icosphere";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_ICOSHPERE;
            return gameObject;
            break;

        case GeometryType::P_CONE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/cone.fbx");
            aux = "Cone";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_CONE;
            return gameObject;
            break;

        case GeometryType::P_TORUS:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/torus.fbx");
            aux = "Torus";
            gameObject->name = aux;
            gameObject->Stype = GeometryType::P_TORUS;
            return gameObject;
            break;
        case GeometryType::P_SUZANNE:
            gameObject = App->assimpMeshes->LoadMeshFromFile("Assets/Models/suzanne.fbx");
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