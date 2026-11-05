#pragma once

class Entity
{
public:
    explicit Entity(int id)
        : id(id)
    {
    }

    int getID() const
    {
        return id;
    }

private:
    int id;
};