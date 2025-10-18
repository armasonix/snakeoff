#include "Level.h"
#include <fstream>
#include <string>

Level Level::loadFromFile(const std::string& path, int w, int h) 
{
    Grid g(w, h);
    // loader: '#'-walls, '.'-emptyness. No file = close border.
    std::ifstream in(path);
    if (in) 
    {
        std::string line;
        int y = 0;
        while (std::getline(in, line) && y < h) 
        {
            for (int x = 0; x < std::min<int>((int)line.size(), w); ++x) 
            {
                if (line[x] == '#') g.set({ x,y }, CellType::Wall);
            }
            ++y;
        }
    }
    else 
    {
        // default border
        for (int x = 0;x < w;++x) { g.set({ x,0 }, CellType::Wall); g.set({ x,h - 1 }, CellType::Wall); }
        for (int y = 0;y < h;++y) { g.set({ 0,y }, CellType::Wall); g.set({ w - 1,y }, CellType::Wall); }
    }
    return Level(std::move(g));
}