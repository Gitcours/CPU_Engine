
#include <cstdint>
#include <iostream>

#include "Framebuffer.hpp"
#include "Display.hpp"

int main()
{
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;

    Framebuffer framebuffer(WIDTH, HEIGHT);

    constexpr std::uint32_t BLACK = 0xFF000000;
    constexpr std::uint32_t RED = 0xFFFF0000;

    framebuffer.clear(BLACK);

    // Notre moteur CPU dessine un carre rouge de 11 x 11 pixels.
    // Cela permet de reperer facilement le pixel central a l'ecran.
    for (int y = 295; y <= 305; ++y)
    {
        for (int x = 395; x <= 405; ++x)
        {
            framebuffer.putPixel(x, y, RED);
        }
    }

    std::cout << "Framebuffer : "
        << framebuffer.getWidth() << " x "
        << framebuffer.getHeight() << '\n';

    std::cout << "Nombre de pixels : "
        << framebuffer.getPixels().size() << '\n';

    Display display(WIDTH, HEIGHT);

    if (!display.isOpen())
    {
        std::cerr << "Impossible d'initialiser l'affichage.\n";
        return 1;
    }

    while (display.isOpen())
    {
        display.processEvents();
        display.present(framebuffer);
    }

    return 0;
}
