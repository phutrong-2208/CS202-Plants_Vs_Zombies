#ifndef TEXT_MANAGER_HPP
#define TEXT_MANAGER_HPP

#include <Common.hpp>

class TextManager {
private:
    std :: map<std :: string, Font> fonts;
    std :: map<std :: string, std::string> aliases;

    std :: string normalizeKey(const std :: string& key) const;

public:
    TextManager() = default;
    ~TextManager();

    // Prevent copies — font GPU handles would double-free
    TextManager(const TextManager&) = delete;
    TextManager& operator=(const TextManager&) = delete;

    void addFont(const std :: string& key, const std :: string& path);
    void addAlias(const std :: string& alias, const std :: string& key);
    Font* getFont(const std :: string& key);

    Vector2 measureText(const std :: string& fontKey, const char* text, float fontSize, float spacing);
    Vector2 centerText(const std :: string& fontKey, const char* text, Rectangle rect, float fontSize, float spacing);
    void drawText(const std :: string& fontKey, const char* text, Vector2 position, float fontSize, float spacing, Color color);
    void drawCenteredText(const std :: string& fontKey, const char* text, Rectangle rect, float fontSize, float spacing, Color color);
};

#endif
