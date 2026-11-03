#pragma once

class Entity
{
public:
    Entity(int id);

    int getID() const;

private:
    int id;
};