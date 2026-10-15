
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
    constexpr std::uint32_t GREEN = 0xFF00FF00;
    constexpr std::uint32_t BLUE = 0xFF0000FF;

    framebuffer.clear(0xFF000000);
    framebuffer.drawTriangleGradient(
        150, 100, RED,
        550, 150, GREEN,
        300, 450, BLUE
    );


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
