#include "GameEntity.h"

GameEntity::GameEntity(
	std::shared_ptr<Mesh> mesh,
	std::shared_ptr<Material> material)
	:
	_mesh(mesh),
	_material(material)
{
	_transform = std::make_shared<Transform>();
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
    return _mesh;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return _material;
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
    return _transform;
}

void GameEntity::SetMesh(std::shared_ptr<Mesh> mesh)
{
    _mesh = mesh;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> material)
{
	_material = material;
}
