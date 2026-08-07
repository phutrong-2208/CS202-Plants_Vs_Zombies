#ifndef USER_PROFILE_SCREEN_HPP
#define USER_PROFILE_SCREEN_HPP

#include <Screens/Screen.hpp>

enum class ProfileDialogMode {
    SELECT,
    CREATE,
    RENAME,
    DELETE_CONFIRM
};

class UserProfileScreen : public Screen {
private:
    static constexpr float VIRTUAL_WIDTH = 800.0f;
    static constexpr float VIRTUAL_HEIGHT = 600.0f;
    static constexpr int MAX_VISIBLE_PROFILES = 5;
    static constexpr int MAX_PROFILE_NAME_LENGTH = 16;

    int screenWidth = 800;
    int screenHeight = 600;
    TextManager* textManager = nullptr;

    Texture2D* dialogHeader = nullptr;
    Texture2D* topLeft = nullptr;
    Texture2D* topMiddle = nullptr;
    Texture2D* topRight = nullptr;
    Texture2D* centerLeft = nullptr;
    Texture2D* centerMiddle = nullptr;
    Texture2D* centerRight = nullptr;
    Texture2D* bigBottomLeft = nullptr;
    Texture2D* bigBottomMiddle = nullptr;
    Texture2D* bigBottomRight = nullptr;
    Texture2D* editBox = nullptr;

    Texture2D* buttonLeft = nullptr;
    Texture2D* buttonMiddle = nullptr;
    Texture2D* buttonRight = nullptr;
    Texture2D* buttonDownLeft = nullptr;
    Texture2D* buttonDownMiddle = nullptr;
    Texture2D* buttonDownRight = nullptr;

    ProfileDialogMode mode = ProfileDialogMode :: SELECT;
    int selectedProfileIndex = 0;
    bool selectionInitialized = false;
    std :: string inputName;

    float getScale() const;
    Vector2 getOffset() const;
    Rectangle toScreenBounds(Rectangle virtualBounds) const;

    Rectangle getProfileRowBounds(int row) const;
    Rectangle getCreateProfileBounds() const;

    void drawTexture(Texture2D* texture, Rectangle virtualBounds) const;
    void drawDialogRow(
        Texture2D* left,
        Texture2D* middle,
        Texture2D* right,
        Rectangle virtualBounds
    ) const;
    void drawDialogBackground() const;
    void drawButton(Rectangle virtualBounds, const char* label, bool enabled = true) const;
    void drawProfileList() const;
    void drawTextEntry() const;
    void drawDeleteConfirmation() const;

    void beginCreateProfile();
    void beginRenameProfile();
    void commitTextEntry();
    void cancelModal();
    void deleteSelectedProfile();
    void handleKeyboard(KeyboardKey key);

public:
    UserProfileScreen(
        int screenWidth,
        int screenHeight,
        AssetManager* assetManager
    );

    void update(float dt) override;
    void draw() override;
    void handleInput(const RawInputEvent& inputEvent) override;
};

#endif
