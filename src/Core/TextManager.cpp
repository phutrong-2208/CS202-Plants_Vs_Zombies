#include <Core/TextManager.hpp>

void TextManager :: addFont(const std :: string& key, const std :: string& path) {
    std :: string fontKey = normalizeKey(key);
    if (fonts.find(fontKey) != fonts.end()) {
        return;
    }

    Font font = LoadFont(path.c_str());
    if (font.texture.id == 0) {
        std :: cerr << "Cannot load font: " << key << '\n';
        return;
    }

    fonts[fontKey] = font;
}

void TextManager :: addAlias(const std :: string& alias, const std :: string& key) {
    std :: string fontKey = normalizeKey(key);
    if (fonts.find(fontKey) == fonts.end()) {
        return;
    }

    aliases[normalizeKey(alias)] = fontKey;
}

Font* TextManager :: getFont(const std :: string& key) {
    std :: string fontKey = normalizeKey(key);
    auto alias = aliases.find(fontKey);
    if (alias != aliases.end()) {
        fontKey = alias -> second;
    }

    auto it = fonts.find(fontKey);
    if (it == fonts.end()) {
        return nullptr;
    }

    return &it -> second;
}

Vector2 TextManager :: measureText(const std::string& fontKey, const char* text, float fontSize, float spacing) {
    Font* font = getFont(fontKey);
    if (font) {
        return MeasureTextEx(*font, text, fontSize, spacing);
    }

    return {(float)MeasureText(text, (int)fontSize), fontSize};
}

Vector2 TextManager :: centerText(const std::string& fontKey, const char* text, Rectangle rect, float fontSize, float spacing) {
    Vector2 textSize = measureText(fontKey, text, fontSize, spacing);
    return {
        rect.x + (rect.width - textSize.x) / 2.0f,
        rect.y + (rect.height - textSize.y) / 2.0f
    };
}

void TextManager :: drawText(const std::string& fontKey, const char* text, Vector2 position, float fontSize, float spacing, Color color) {
    Font* font = getFont(fontKey);
    if (font) {
        DrawTextEx(*font, text, position, fontSize, spacing, color);
    } else {
        DrawText(text, (int)position.x, (int)position.y, (int)fontSize, color);
    }
}

void TextManager :: drawCenteredText(const std::string& fontKey, const char* text, Rectangle rect, float fontSize, float spacing, Color color) {
    drawText(fontKey, text, centerText(fontKey, text, rect, fontSize, spacing), fontSize, spacing, color);
}

std :: string TextManager :: normalizeKey(const std::string& key) const {
    std :: string normalized = key;
    for (char& c : normalized) {
        if (islower(c)) c = toupper(c);
    }
    return normalized;
}
