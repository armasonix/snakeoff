#include "core/Application.h"

int main() 
{
    Application app(800, 600);
    if (!app.init()) return 1;
    app.run();
    return 0;
}