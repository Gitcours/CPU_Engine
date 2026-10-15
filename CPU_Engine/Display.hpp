#pragma once
#include <SDL/SDL.h>
#include "Framebuffer.hpp"

class Display
{
public:
    Display(int width, int height);
    ~Display();

    Display(const Display&) = delete;
    Display& operator=(const Display&) = delete;

    bool isOpen() const;
    void processEvents();
    void present(const Framebuffer& framebuffer);

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

    bool open = false;
};
