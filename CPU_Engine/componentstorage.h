#pragma once
#include <iostream>

class IComponentStorage {

public :

	void add(int entity, void* value);
	void remove(int entity);
	bool has(int entity);
	void* get(int entity);
		
};

template<typename T>

class ComponentStorage : public IComponentStorage
{

public:

	ComponentStorage();

	void add(int entity, T& value);
	void remove(int entity);
	bool has(int entity);
	T& get(int entity);

private:
	std::array<T, 100> data;
	std::array<bool, 100> owned;


	
};

