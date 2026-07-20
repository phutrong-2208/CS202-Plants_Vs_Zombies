#include <Worlds/DayMap.hpp>

DayMap :: DayMap() {
    const std::string backgroundDir = std::string(PROJECT_DIR) + "assets/texture/Maps/Day/background1.jpg";
    loadBackground(backgroundDir, {210, 0, 800, 600}, {250, 85, 750, 495});
}

// Rectangle DayMap :: getGridArea() const {
//     int sw = GetScreenWidth();
//     int sh = GetScreenHeight();
//     float scaleX = ((float)sw / crop.width);
//     float scaleY = ((float)sh / crop.height);

//     float dstX = (sw - crop.width * scaleX) / 2.0f;
//     float dstY = (sh - crop.height * scaleY) / 2.0f;

//     return {dstX + (lawnSrc.x - crop.x) * scaleX,
//             dstY + (lawnSrc.y - crop.y) * scaleY,
//             lawnSrc.width * scaleX, lawnSrc.height * scaleY};
// }
