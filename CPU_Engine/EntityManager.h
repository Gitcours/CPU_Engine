#pragma once
#include <array>
#include <bitset>
#include <vector>

#include "componentstorage.h"

constexpr int MAX_COMPONENTS = 32;

class EntityManager
{
public:

	EntityManager();

	int nextEntityID;
	std::vector<int> freeEntityIDs;
	std::vector<IComponentStorage*> componentStorages;

	int CreateEntity()
	{
	}

	void DestroyEntity(int entity)
	{
	}

	void AddComponent(int entity, int componentID, void* value)
	{
	}

	void RemoveComponent(int entity, int componentID)
	{
	}


private:

};

