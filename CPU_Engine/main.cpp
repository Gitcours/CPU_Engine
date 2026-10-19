#include <cstdint>
#include <iostream>

#include "Framebuffer.hpp"
#include "Display.hpp"
#include "vec.h"
#include "mat.h"

int main()
{
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 600;
    constexpr std::uint32_t BLACK = 0xFF000000;
    const float R = 200.0f;

    Framebuffer framebuffer(WIDTH, HEIGHT);

    framebuffer.clear(BLACK);
    framebuffer.drawTriangleGradient({ 150,480 }, { 400,80 }, { 650,480 });

    framebuffer.drawCircle(WIDTH / 2, HEIGHT / 2, (int)R, framebuffer.rgb(60, 60, 80));

    Display display(WIDTH, HEIGHT);
    /*mat4 transform = mat4_mul(

    );*/
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
