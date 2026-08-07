#include <UI/WaveHUD.hpp>

void WaveHUD::setTexturePackage(TexturePackage* package) {
    wavePackage = package;
}

Rectangle WaveHUD::getBarRect(int screenW, int screenH) const {
    return {
        (float)screenW - BAR_W - 12.0f,
        (float)screenH - BAR_H  - 12.0f,
        BAR_W,
        BAR_H
    };
}

void WaveHUD::draw(float progress, int currentWave, int totalWaves, bool finished,
                   int screenW, int screenH) const {
    const Rectangle barRect = getBarRect(screenW, screenH);

    if (finished) {
        // Draw brain trophy centered above the bar position
        Texture2D* brain = wavePackage ? wavePackage->GetTexture("BRAIN") : nullptr;
        if (brain) {
            const float sz = 48.0f;
            DrawTexturePro(*brain,
                {0, 0, (float)brain -> width, (float)brain -> height},
                {barRect.x + barRect.width * 0.5f - sz * 0.5f,
                 barRect.y - sz - 4.0f, sz, sz},
                {0, 0}, 0, WHITE);
        }
        return;
    }

    // Background bar
    Texture2D* meter = wavePackage ? wavePackage->GetTexture("FLAGMETER") : nullptr;
    if (meter) {
        // Full sprite is two rows: row 0 = grey empty, row 1 = green fill.
        float halfH = (float)meter->height * 0.5f;

        // Draw empty (grey) row — always full width
        DrawTexturePro(*meter,
            {0, 0, (float)meter->width, halfH},
            barRect, {0, 0}, 0, WHITE);

        // Draw green fill clipped to progress
        if (progress > 0.0f) {
            float fillW = barRect.width * progress;
            float srcFillW = (float)meter->width * progress;
            DrawTexturePro(*meter,
                {0, halfH, srcFillW, halfH},
                {barRect.x, barRect.y, fillW, barRect.height},
                {0, 0}, 0, WHITE);
        }
    } else {
        // Fallback if texture missing
        DrawRectangleRec(barRect, Color{50, 50, 50, 200});
        DrawRectangleRec({barRect.x, barRect.y, barRect.width * progress, barRect.height},
                         Color{80, 200, 80, 220});
        DrawRectangleLinesEx(barRect, 1.5f, WHITE);
    }

    // Parts: zombie head slider + wave flag markers
    Texture2D* parts = wavePackage ? wavePackage->GetTexture("FLAGMETERPARTS") : nullptr;
    if (parts) {
        // The sprite sheet has: zombie-head on the left, flag on the right.
        // Assuming roughly equal split; adjust if your texture differs.
        float partHalf = (float)parts->width * 0.5f;
        float ph = (float)parts->height;

        // Zombie head — at current progress position
        float headX = barRect.x + barRect.width * progress - ICON_SIZE * 0.5f;
        float headY = barRect.y + barRect.height * 0.5f - ICON_SIZE * 0.5f;
        DrawTexturePro(*parts,
            {0, 0, partHalf, ph},
            {headX, headY, ICON_SIZE, ICON_SIZE},
            {0, 0}, 0, WHITE);

        // Flag markers at each wave boundary
        if (totalWaves > 0) {
            for (int w = 1; w <= totalWaves; ++w) {
                float boundX = barRect.x + barRect.width * ((float)w / (float)totalWaves);
                float flagY  = barRect.y - ICON_SIZE * 0.5f;
                // Tint flags that have already passed
                Color flagTint = (w <= currentWave + 1) ? WHITE : Color{180, 180, 180, 200};
                DrawTexturePro(*parts,
                    {partHalf, 0, partHalf, ph},
                    {boundX - ICON_SIZE * 0.5f, flagY, ICON_SIZE, ICON_SIZE},
                    {0, 0}, 0, flagTint);
            }
        }
    }

    // Wave label
    const char* waveLabel = TextFormat("Wave %d/%d", currentWave + 1, totalWaves);
    int labelW = MeasureText(waveLabel, 12);
    DrawText(waveLabel,
             (int)(barRect.x + barRect.width * 0.5f - labelW * 0.5f),
             (int)(barRect.y - 16),
             12,
             Color{240, 240, 240, 230});
}
