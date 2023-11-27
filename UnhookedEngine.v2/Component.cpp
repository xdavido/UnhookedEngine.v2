#include "Component.h"

Component::Component(GameObject* owner)
{
	type = ComponentType::NONE;

	isActive = false;

	this->mOwner = owner;


}

Component::~Component()
{

}

void Component::Update()
{

}

void Component::PrintInspector() {}

void Component::EnableComponent() {}

void Component::DisableComponent() {}


