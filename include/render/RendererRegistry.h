#pragma once

namespace render
{
    struct RendererRegistry
    {
        bool background = true;
        bool grid = true;
        bool portals = true;
        bool items = true;
        bool snake = true;
        bool gate = true;
        bool confuseFx = true;
        bool ui = true;

        void toggleBackground() { background = !background; }
        void toggleGrid() { grid = !grid; }
        void togglePortals() { portals = !portals; }
        void toggleItems() { items = !items; }
        void toggleSnake() { snake = !snake; }
        void toggleGate() { gate = !gate; }
        void toggleConfuseFx() { confuseFx = !confuseFx; }
        void toggleUI() { ui = !ui; }
    };
}