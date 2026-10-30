#include "componentstorage.h"

template<typename T>

ComponentStorage<T>::ComponentStorage() {
}

template<typename T>
void ComponentStorage<T>::add(int entity, T& value) {
	if (entity < 0 || entity >= 100)
		return;

	data[entity] = value;
	owned[entity] = true;
}
template<typename T>
void ComponentStorage<T>::remove(int entity) {
	if (entity < 0 || entity >= 100)
		return;

	owned[entity] = false;
}
template<typename T>
bool ComponentStorage<T>::has(int entity) {
	if (owned[entity]) {
		return true;
	}
}

template<typename T>
T& ComponentStorage<T>::get(int entity) {
	return owned[entity];
}