#include "core/Application.h"
#include <exception>
#include <cstdio>

int main()
{
    try 
    {
        Application app(800, 600);
        if (!app.init()) return 1;
        app.run();
        return 0;
    }
    catch (const std::exception& e) 
    {
        std::fprintf(stderr, "Fatal error: %s\n", e.what());
        return 2;
    }
    catch (...) 
    {
        std::fprintf(stderr, "Fatal unknown error\n");
        return 3;
    }
}