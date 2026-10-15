
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

    framebuffer.clear(BLACK);



    framebuffer.drawTriangle(
        100, 100,
        350, 100,
        225, 350,
        RED
    );

    framebuffer.drawTriangle(
        400, 100,
        700, 200,
        500, 450,
        GREEN
    );

    framebuffer.drawTriangle(
        100, 400,
        250, 500,
        350, 400,
        BLUE
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
