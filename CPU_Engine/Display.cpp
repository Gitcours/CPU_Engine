#include "Display.hpp"

#include <iostream>

Display::Display(int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Erreur SDL_Init : "
            << SDL_GetError() << '\n';
        return;
    }

    window = SDL_CreateWindow(
        "3D CPU Rendering Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr)
    {
        std::cerr << "Erreur creation fenetre : "
            << SDL_GetError() << '\n';
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    // Repli si le rendu accelere n'est pas disponible.
    if (renderer == nullptr)
    {
        renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_SOFTWARE
        );
    }

    if (renderer == nullptr)
    {
        std::cerr << "Erreur creation renderer : "
            << SDL_GetError() << '\n';

        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
        return;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    if (texture == nullptr)
    {
        std::cerr << "Erreur creation texture : "
            << SDL_GetError() << '\n';

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        renderer = nullptr;
        window = nullptr;

        SDL_Quit();
        return;
    }

    open = true;
}

Display::~Display()
{
    if (texture != nullptr)
        SDL_DestroyTexture(texture);

    if (renderer != nullptr)
        SDL_DestroyRenderer(renderer);

    if (window != nullptr)
        SDL_DestroyWindow(window);

    SDL_Quit();
}

bool Display::isOpen() const
{
    return open;
}

void Display::processEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            open = false;
    }
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        open = false;
}

void Display::present(const Framebuffer& framebuffer)
{
    if (!open)
        return;

    const auto& pixels = framebuffer.getPixels();

    const int pitch =
        framebuffer.getWidth() * sizeof(std::uint32_t);

    if (SDL_UpdateTexture(
        texture, nullptr, pixels.data(), pitch) != 0)
    {
        std::cerr << "Erreur mise a jour texture : "
            << SDL_GetError() << '\n';
        open = false;
        return;
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
