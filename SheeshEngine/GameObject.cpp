#include "GameObject.h"
#include "Globals.h"

#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"


GameObject::GameObject(GameObject* parent)
{
    name = "Game Object";
    mParent = parent;
    
    if (parent != nullptr) { parent->mChildren.push_back(this); }

    
    transform = new ComponentTransform();
    mComponents.push_back(transform);
}


GameObject::~GameObject()
{
    // Delete all childrens
    RELEASE_VECTOR(mChildren, mChildren.size());
    mChildren.clear();

    // Delete all components
    RELEASE_VECTOR(mComponents, mComponents.size());
    mComponents.clear();

   

    transform = nullptr;
    mParent = nullptr;
}

void GameObject::Update()
{
    for (size_t i = 0; i < mChildren.size(); i++)
    {
        mChildren[i]->Update();
    }

    for (size_t i = 0; i < mComponents.size(); i++)
    {
        mComponents[i]->Update();
    }
}

GameObject* GameObject::GetParent() {
    return mParent;
}

void GameObject::SetParent(GameObject* parent) {
    mParent = parent;
}

std::vector<GameObject*> GameObject::GetChildren() {
    return mChildren;
}

void GameObject::AddChild(GameObject* child) {
    mChildren.push_back(child);
}

std::vector<Component*> GameObject::GetComponents() {
    return mComponents;
}

void GameObject::CreateComponent(ComponentType type) {
    
    Component* newComponent = new Component(this);

    newComponent->type = type;
    newComponent->isActive = true;

    mComponents.push_back(newComponent);

    delete newComponent;

}
void GameObject::DeleteChild(GameObject* child)
{
    for (int i = 0; i < GetChildren().size(); i++) {
        if (GetChildren()[i] == child) {
            GetChildren().erase(GetChildren().begin() + i);
            child->mParent = nullptr;
        }
    }
}


bool GameObject::ChangeParent(GameObject* NewParent)
{
    if (mParent != nullptr) {
        if (NewParent->CheckChildOf(this)) return false;

        mParent->DeleteChild(this);
    }

    mParent = NewParent;
    NewParent->GetChildren().push_back(this);

    return true;
}

bool GameObject::CheckChildOf(GameObject* parent)
{
    if (parent->GetChildren().empty()) return false;

    for (int i = 0; i < parent->GetChildren().size(); i++) {

        if (GetChildren()[i] == this) return true;

    }
    return false;
}

ComponentMesh* GameObject::GetComponentMesh()
{

    for (size_t i = 0; i < mComponents.size(); i++)
    {
        if (mComponents[i]->type == ComponentType::MESH) {
            return (ComponentMesh*)mComponents[i];
        }
    }

    return nullptr;
}
