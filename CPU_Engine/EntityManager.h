#pragma once

#include <array>
#include <bitset>
#include <vector>

#include "Entity.h"
#include "componentstorage.h"

constexpr int MAX_COMPONENTS = 32;
constexpr int MAX_ENTITIES = 100;

class EntityManager
{
public:

    EntityManager()
        : nextEntityID(0)
    {
    }

    int nextEntityID;

    std::vector<int> freeEntityIDs;
    std::vector<IComponentStorage*> componentStorages;

  

    Entity CreateEntity()
    {
        int id;

        if (!freeEntityIDs.empty())
        {
            id = freeEntityIDs.back();
            freeEntityIDs.pop_back();
        }
        else
        {
            if (nextEntityID >= MAX_ENTITIES)
            {
                return Entity(-1);
            }

            id = nextEntityID++;
        }

        /*entityComponents[id].reset();*/

        return Entity(id);
    }

    

    void DestroyEntity(const Entity& entity)
    {
        int id = entity.getID();
        
        for (int componentID = 0;
            componentID < componentStorages.size();
            componentID++)
        {
            if (componentStorages[componentID]->Has(id))
            {
                componentStorages[componentID]->Remove(id);
            }
        }

        entityComponents[id].reset();


        freeEntityIDs.push_back(id);
    }

  

    void AddComponent(
        const Entity& entity,
        int componentID,
        void* value)
    {
        int id = entity.getID();

        componentStorages[componentID]->Add(id, value);

        entityComponents[id].set(componentID);
    }

    

    void RemoveComponent(
        const Entity& entity,
        int componentID)
    {
        int id = entity.getID();


        if (!entityComponents[id].test(componentID))
            return;

        componentStorages[componentID]->Remove(id);

        entityComponents[id].reset(componentID);
    }

    bool HasComponent(const Entity& entity, int componentID)
    {
        int id = entity.getID();

        if (componentID >= componentStorages.size())
            return false;

        return entityComponents[id].test(componentID);
    }

    void* GetComponent(const Entity& entity, int componentID)
    {
        int id = entity.getID();

        return componentStorages[componentID]->Get(id);
    }

    std::vector<Entity> FetchAllEntities(const std::vector<int>& componentIDs)
    {
        std::bitset<MAX_COMPONENTS> query;

        for (int componentID : componentIDs)
        {
            if (componentID < 0 || componentID >= MAX_COMPONENTS)
                return {};

            query.set(componentID);
        }

        std::vector<Entity> result;

        for (int id = 0; id < nextEntityID; id++)
        {
           

            if ((entityComponents[id] & query) == query)
            {
                result.push_back(Entity(id));
            }
        }

        return result;
    }

private:
    std::array<std::bitset<MAX_COMPONENTS>, MAX_ENTITIES>
        entityComponents{};
};