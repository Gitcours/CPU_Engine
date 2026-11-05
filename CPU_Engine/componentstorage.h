#pragma once
#include <iostream>
#include <array>

class IComponentStorage {

public :

public:
	virtual ~IComponentStorage() = default;

	virtual void Add(int entity, void* value) = 0;
	virtual void Remove(int entity) = 0;
	virtual bool Has(int entity) = 0;
	virtual void* Get(int entity) = 0;
		
};


template<typename T>
class ComponentStorage : public IComponentStorage
{

public:
	void Add(int entity, void* value) override
	{
		if (entity < 0 || entity >= 100)
			return;

		data[entity] = *static_cast<T*>(value);
		owned[entity] = true;
	}
	void Remove(int entity) override
	{
		if (entity < 0 || entity >= 100)
			return;

		owned[entity] = false;
	}
	bool Has(int entity) override
	{
		if (owned[entity]) {
			return true;
		}
	}
	void* Get(int entity) override
	{
		return &data[entity];
	}
private:
	std::array<T, 100> data;
	std::array<bool, 100> owned;
};

