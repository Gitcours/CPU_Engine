#pragma once

#include <vector>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <cassert>
#include <utility>

#include "Entity.h"
#include "EntityManager.h"
#include "componentstorage.h"

class ECS
{
public:

    using System = std::function<void(EntityManager&)>;

    ECS() = default;

    ~ECS()
    {
        for (IComponentStorage* storage : componentStorages)
        {
            delete storage;
        }
    }

    ECS(const ECS&) = delete;
    ECS& operator=(const ECS&) = delete;

    Entity CreateEntity()
    {
        return entityManager.CreateEntity();
    }

    void DestroyEntity(const Entity& entity)
    {
        entityManager.DestroyEntity(entity);
    }

    EntityManager& GetEntityManager()
    {
        return entityManager;
    }

    void AddSystem(System system)
    {
        systems.push_back(std::move(system));
    }

    void ExecuteSystems()
    {
        for (System& system : systems)
        {
            system(entityManager);
        }
    }

    template<typename T>
    void RegisterComponent()
    {
        std::type_index type = std::type_index(typeid(T));

        if (componentIDs.find(type) != componentIDs.end())
            return;

        int componentID = static_cast<int>(componentStorages.size());

        ComponentStorage<T>* storage = new ComponentStorage<T>();

        entityManager.componentStorages.push_back(storage);
        componentIDs[type] = componentID;
        componentStorages.push_back(storage);
    }

    template<typename T>
    void AddComponent(const Entity& entity, const T& component)
    {
        entityManager.AddComponent(
            entity,
            GetComponentID<T>(),
            const_cast<T*>(&component)
        );
    }

    template<typename T>
    void RemoveComponent(const Entity& entity)
    {
        entityManager.RemoveComponent(
            entity,
            GetComponentID<T>()
        );
    }

    template<typename T>
    bool HasComponent(const Entity& entity)
    {
        return entityManager.HasComponent(
            entity,
            GetComponentID<T>()
        );
    }

    template<typename T>
    T* GetComponent(const Entity& entity)
    {
        return static_cast<T*>(
            entityManager.GetComponent(
                entity,
                GetComponentID<T>()
            )
            );
    }

    template<typename... ComponentTypes>
    std::vector<Entity> FetchAllEntities()
    {
        std::vector<int> componentIDsToFetch;

        const int componentIDsArray[] = {
            GetComponentID<ComponentTypes>()...
        };

        const int numberOfComponents = sizeof...(ComponentTypes);

        for (int i = 0; i < numberOfComponents; i++)
        {
            componentIDsToFetch.push_back(componentIDsArray[i]);
        }

        return entityManager.FetchAllEntities(componentIDsToFetch);
    }

private:

    template<typename T>
    int GetComponentID()
    {
        std::type_index type = std::type_index(typeid(T));

        auto it = componentIDs.find(type);

        assert(it != componentIDs.end());

        return it->second;
    }

    EntityManager entityManager;

    std::vector<System> systems;

    std::unordered_map<std::type_index, int> componentIDs;

    std::vector<IComponentStorage*> componentStorages;
};