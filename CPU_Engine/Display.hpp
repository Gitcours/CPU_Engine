#pragma once
#if __has_include(<SDL.h>)
    #include <SDL.h>
#elif __has_include(<SDL/SDL.h>)
    #include <SDL/SDL.h>
#else
    #error "SDL2 introuvable : verifiez les chemins d'inclusion."
#endif
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
