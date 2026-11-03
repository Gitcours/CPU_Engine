#include "Entity.h"

Entity::Entity(int id)
    : id(id)
{
}

int Entity::getID() const
{
    return id;
}