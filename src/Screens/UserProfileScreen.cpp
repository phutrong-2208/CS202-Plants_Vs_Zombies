#include <Screens/UserProfileScreen.hpp>
#include <Core/UserProfileManager.hpp>

namespace {
    constexpr Rectangle PANEL = {190.0f, 65.0f, 420.0f, 470.0f};
    constexpr Rectangle HEADER = {306.5f, 38.0f, 187.0f, 64.0f};
    constexpr Rectangle TITLE = {235.0f, 105.0f, 330.0f, 42.0f};
    constexpr Rectangle LIST = {235.0f, 158.0f, 330.0f, 210.0f};
    constexpr Rectangle RENAME = {212.0f, 438.0f, 88.0f, 42.0f};
    constexpr Rectangle DELETE_BUTTON = {304.0f, 438.0f, 88.0f, 42.0f};
    constexpr Rectangle OK = {408.0f, 438.0f, 76.0f, 42.0f};
    constexpr Rectangle CANCEL = {488.0f, 438.0f, 100.0f, 42.0f};
    constexpr Rectangle PROMPT = {235.0f, 190.0f, 330.0f, 58.0f};
    constexpr Rectangle EDIT_FIELD = {255.0f, 270.0f, 290.0f, 48.0f};
    constexpr float ROW_HEIGHT = 34.0f;
}

UserProfileScreen :: UserProfileScreen(
    int width,
    int height,
    AssetManager* manager
) : screenWidth(width), screenHeight(height) {
    setAssetManager(manager);
    if(!assetManager) return;

    TextureManager* textures = assetManager -> getTextureManager();
    textManager = assetManager -> getTextManager();
    if(!textures) return;

    TexturePackage* dialog = textures -> getPackage("UserProfile");
    TexturePackage* buttons = textures -> getPackage("GameResult");

    if(dialog) {
        dialogHeader = dialog -> GetTexture("DIALOG_HEADER");
        topLeft = dialog -> GetTexture("DIALOG_TOPLEFT");
        topMiddle = dialog -> GetTexture("DIALOG_TOPMIDDLE");
        topRight = dialog -> GetTexture("DIALOG_TOPRIGHT");
        centerLeft = dialog -> GetTexture("DIALOG_CENTERLEFT");
        centerMiddle = dialog -> GetTexture("DIALOG_CENTERMIDDLE");
        centerRight = dialog -> GetTexture("DIALOG_CENTERRIGHT");
        bigBottomLeft = dialog -> GetTexture("DIALOG_BIGBOTTOMLEFT");
        bigBottomMiddle = dialog -> GetTexture("DIALOG_BIGBOTTOMMIDDLE");
        bigBottomRight = dialog -> GetTexture("DIALOG_BIGBOTTOMRIGHT");
        editBox = dialog -> GetTexture("EDITBOX");
    }

    if(buttons) {
        buttonLeft = buttons -> GetTexture("BUTTON_LEFT");
        buttonMiddle = buttons -> GetTexture("BUTTON_MIDDLE");
        buttonRight = buttons -> GetTexture("BUTTON_RIGHT");
        buttonDownLeft = buttons -> GetTexture("BUTTON_DOWN_LEFT");
        buttonDownMiddle = buttons -> GetTexture("BUTTON_DOWN_MIDDLE");
        buttonDownRight = buttons -> GetTexture("BUTTON_DOWN_RIGHT");
    }
}

float UserProfileScreen :: getScale() const {
    return std :: min(
        static_cast<float>(screenWidth) / VIRTUAL_WIDTH,
        static_cast<float>(screenHeight) / VIRTUAL_HEIGHT
    );
}

Vector2 UserProfileScreen :: getOffset() const {
    const float scale = getScale();
    return {
        (screenWidth - VIRTUAL_WIDTH * scale) * 0.5f,
        (screenHeight - VIRTUAL_HEIGHT * scale) * 0.5f
    };
}

Rectangle UserProfileScreen :: toScreenBounds(Rectangle bounds) const {
    const float scale = getScale();
    const Vector2 offset = getOffset();
    return {
        offset.x + bounds.x * scale,
        offset.y + bounds.y * scale,
        bounds.width * scale,
        bounds.height * scale
    };
}

Rectangle UserProfileScreen :: getProfileRowBounds(int row) const {
    return {
        LIST.x + 12.0f,
        LIST.y + 8.0f + row * ROW_HEIGHT,
        LIST.width - 24.0f,
        ROW_HEIGHT
    };
}

Rectangle UserProfileScreen :: getCreateProfileBounds() const {
    const int count = userProfileManager ? std :: min(userProfileManager -> getProfileCount(), MAX_VISIBLE_PROFILES) : 0;
    return getProfileRowBounds(count);
}

void UserProfileScreen :: drawTexture(Texture2D* texture, Rectangle bounds) const {
    if(!texture) return;
    DrawTexturePro(
        *texture,
        {0.0f, 0.0f, static_cast<float>(texture -> width), static_cast<float>(texture -> height)},
        toScreenBounds(bounds),
        {0.0f, 0.0f}, 0.0f, WHITE
    );
}

void UserProfileScreen :: drawDialogRow(
    Texture2D* left,
    Texture2D* middle,
    Texture2D* right,
    Rectangle bounds
) const {
    constexpr float LEFT_WIDTH = 107.0f;
    constexpr float RIGHT_WIDTH = 120.0f;
    drawTexture(left, {bounds.x, bounds.y, LEFT_WIDTH, bounds.height});
    drawTexture(middle, {
        bounds.x + LEFT_WIDTH, bounds.y,
        std :: max(0.0f, bounds.width - LEFT_WIDTH - RIGHT_WIDTH), bounds.height
    });
    drawTexture(right, {
        bounds.x + bounds.width - RIGHT_WIDTH, bounds.y,
        RIGHT_WIDTH, bounds.height
    });
}

void UserProfileScreen :: drawDialogBackground() const {
    drawDialogRow(topLeft, topMiddle, topRight, {PANEL.x, PANEL.y, PANEL.width, 97.0f});
    drawDialogRow(centerLeft, centerMiddle, centerRight, {PANEL.x, PANEL.y + 97.0f, PANEL.width, 223.0f});
    drawDialogRow(bigBottomLeft, bigBottomMiddle, bigBottomRight, {PANEL.x, PANEL.y + 320.0f, PANEL.width, 150.0f});
    drawTexture(dialogHeader, HEADER);
}

void UserProfileScreen :: drawButton(
    Rectangle virtualBounds,
    const char* label,
    bool enabled
) const {
    const Rectangle bounds = toScreenBounds(virtualBounds);
    const bool hovered = enabled && CheckCollisionPointRec(GetMousePosition(), bounds);
    Texture2D* left = hovered && buttonDownLeft ? buttonDownLeft : buttonLeft;
    Texture2D* middle = hovered && buttonDownMiddle ? buttonDownMiddle : buttonMiddle;
    Texture2D* right = hovered && buttonDownRight ? buttonDownRight : buttonRight;

    if(left && middle && right) {
        const float scale = getScale();
        const float leftWidth = left -> width * scale;
        const float rightWidth = right -> width * scale;
        const float middleWidth = std :: max(0.0f, bounds.width - leftWidth - rightWidth);
        const Color tint = enabled ? WHITE : Color{130, 130, 130, 210};

        DrawTexturePro(*left,
            {0, 0, static_cast<float>(left -> width), static_cast<float>(left -> height)},
            {bounds.x, bounds.y, leftWidth, bounds.height}, {0, 0}, 0, tint);
        DrawTexturePro(*middle,
            {0, 0, static_cast<float>(middle -> width), static_cast<float>(middle -> height)},
            {bounds.x + leftWidth, bounds.y, middleWidth, bounds.height}, {0, 0}, 0, tint);
        DrawTexturePro(*right,
            {0, 0, static_cast<float>(right -> width), static_cast<float>(right -> height)},
            {bounds.x + bounds.width - rightWidth, bounds.y, rightWidth, bounds.height}, {0, 0}, 0, tint);
    }
    else {
        DrawRectangleRounded(
            bounds, 0.25f, 8,
            enabled ? (hovered ? Color{105, 112, 145, 255} : Color{70, 76, 105, 255}) : Color{55, 55, 65, 210}
        );
    }

    if(textManager) {
        textManager -> drawCenteredText(
            "LUCKIESTGUY-REGULAR", label, bounds,
            17.0f * getScale(), 0.5f * getScale(),
            enabled ? Color{255, 245, 201, 255} : Color{140, 140, 145, 255}
        );
    }
}

void UserProfileScreen :: drawProfileList() const {
    const Rectangle listBounds = toScreenBounds(LIST);
    DrawRectangleRounded(listBounds, 0.08f, 8, Color{22, 24, 37, 180});

    const int count = userProfileManager ? std :: min(userProfileManager -> getProfileCount(), MAX_VISIBLE_PROFILES) : 0;

    for(int index = 0; index < count; ++index) {
        const UserProfile* profile = userProfileManager -> getProfile(index);
        if(!profile) continue;

        const Rectangle row = toScreenBounds(getProfileRowBounds(index));
        const bool selected = index == selectedProfileIndex;
        const bool hovered = CheckCollisionPointRec(GetMousePosition(), row);
        if(selected || hovered) {
            DrawRectangleRounded(
                row, 0.18f, 6,
                selected ? Color{69, 80, 63, 205} : Color{55, 59, 72, 180}
            );
        }

        if(textManager) {
            std :: string label = selected ? "> " : "  ";
            label += profile -> getProfileName();
            textManager -> drawText(
                "LUCKIESTGUY-REGULAR", label.c_str(),
                {row.x + 12.0f * getScale(), row.y + 5.0f * getScale()},
                21.0f * getScale(), 0.5f * getScale(),
                selected ? LIME : Color{235, 235, 225, 255}
            );
        }
    }

    if(count < MAX_VISIBLE_PROFILES && textManager) {
        const Rectangle row = toScreenBounds(getCreateProfileBounds());
        const bool hovered = CheckCollisionPointRec(GetMousePosition(), row);
        if(hovered) DrawRectangleRounded(row, 0.18f, 6, Color{55, 59, 72, 180});
        textManager -> drawCenteredText(
            "LUCKIESTGUY-REGULAR", "(CREATE A NEW USER)", row,
            18.0f * getScale(), 0.5f * getScale(),
            hovered ? LIME : Color{220, 205, 140, 255}
        );
    }
}

void UserProfileScreen :: drawTextEntry() const {
    if(!textManager) return;
    const char* prompt = mode == ProfileDialogMode :: CREATE ? "PLEASE ENTER YOUR NAME:" : "ENTER A NEW NAME:";
    textManager -> drawCenteredText(
        "LUCKIESTGUY-REGULAR", prompt, toScreenBounds(PROMPT),
        22.0f * getScale(), 0.5f * getScale(), Color{255, 230, 150, 255}
    );

    const Rectangle field = toScreenBounds(EDIT_FIELD);
    if(editBox) {
        DrawTexturePro(
            *editBox,
            {0, 0, static_cast<float>(editBox -> width), static_cast<float>(editBox -> height)},
            field, {0, 0}, 0, WHITE
        );
    }
    else {
        DrawRectangleRec(field, Color{84, 48, 15, 255});
        DrawRectangleLinesEx(field, 3.0f * getScale(), Color{238, 195, 85, 255});
    }

    std :: string shown = inputName;
    if(static_cast<int>(GetTime() * 2.0) % 2 == 0 &&
       static_cast<int>(shown.size()) < MAX_PROFILE_NAME_LENGTH) shown += "|";
    textManager -> drawCenteredText(
        "LUCKIESTGUY-REGULAR", shown.c_str(), field,
        22.0f * getScale(), 0.5f * getScale(), Color{255, 245, 205, 255}
    );
}

void UserProfileScreen :: drawDeleteConfirmation() const {
    if(!textManager || !userProfileManager) return;
    const UserProfile* profile = userProfileManager -> getProfile(selectedProfileIndex);
    std :: string message = "DELETE ";
    message += profile ? profile -> getProfileName() : "THIS USER";
    message += "?";
    textManager -> drawCenteredText(
        "LUCKIESTGUY-REGULAR", message.c_str(),
        toScreenBounds({230.0f, 205.0f, 340.0f, 80.0f}),
        25.0f * getScale(), 0.5f * getScale(), Color{255, 210, 125, 255}
    );
    textManager -> drawCenteredText(
        "LUCKIESTGUY-REGULAR", "THIS CANNOT BE UNDONE.",
        toScreenBounds({230.0f, 285.0f, 340.0f, 44.0f}),
        16.0f * getScale(), 0.5f * getScale(), Color{220, 220, 220, 255}
    );
}

void UserProfileScreen :: update(float) {
    if(selectionInitialized || !userProfileManager) return;
    selectedProfileIndex = std :: max(0, userProfileManager -> getActiveProfileIndex());
    selectionInitialized = true;
}

void UserProfileScreen :: draw() {
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, 120});
    drawDialogBackground();
    if(textManager) {
        textManager -> drawCenteredText(
            "LUCKIESTGUY-REGULAR", "WHO ARE YOU?", toScreenBounds(TITLE),
            30.0f * getScale(), getScale(), Color{255, 225, 120, 255}
        );
    }

    if(mode == ProfileDialogMode :: SELECT) {
        drawProfileList();
        const bool canEdit = userProfileManager && userProfileManager -> getProfileCount() > 0;
        const bool canDelete = userProfileManager && userProfileManager -> getProfileCount() > 1;
        drawButton(RENAME, "RENAME", canEdit);
        drawButton(DELETE_BUTTON, "DELETE", canDelete);
        drawButton(OK, "OK", canEdit);
        drawButton(CANCEL, "CANCEL");
    }
    else if(mode == ProfileDialogMode :: DELETE_CONFIRM) {
        drawDeleteConfirmation();
        drawButton(OK, "YES");
        drawButton(CANCEL, "NO");
    }
    else {
        drawTextEntry();
        drawButton(OK, "OK", !inputName.empty());
        drawButton(CANCEL, "CANCEL");
    }
}

void UserProfileScreen :: beginCreateProfile() {
    inputName.clear();
    mode = ProfileDialogMode :: CREATE;
}

void UserProfileScreen :: beginRenameProfile() {
    if(!userProfileManager) return;
    const UserProfile* profile = userProfileManager -> getProfile(selectedProfileIndex);
    if(!profile) return;
    inputName = profile -> getProfileName();
    mode = ProfileDialogMode :: RENAME;
}

void UserProfileScreen :: commitTextEntry() {
    if(!userProfileManager || inputName.empty()) return;
    if(mode == ProfileDialogMode :: CREATE) {
        if(userProfileManager -> getProfileCount() >= MAX_VISIBLE_PROFILES) return;
        userProfileManager -> addProfile(std :: make_unique<UserProfile>(inputName));
        selectedProfileIndex = userProfileManager -> getProfileCount() - 1;
    }
    else if(mode == ProfileDialogMode :: RENAME) {
        userProfileManager -> renameProfile(selectedProfileIndex, inputName);
    }
    cancelModal();
}

void UserProfileScreen :: cancelModal() {
    mode = ProfileDialogMode :: SELECT;
    inputName.clear();
}

void UserProfileScreen :: deleteSelectedProfile() {
    if(!userProfileManager) return;
    userProfileManager -> delProfile(selectedProfileIndex);
    selectedProfileIndex = std :: min(
        selectedProfileIndex,
        userProfileManager -> getProfileCount() - 1
    );
    cancelModal();
}

void UserProfileScreen :: handleKeyboard(KeyboardKey key) {
    if(mode == ProfileDialogMode :: SELECT) {
        if(key == KEY_ESCAPE) requestTransition(ScreenAction :: POP);
        return;
    }
    if(key == KEY_ESCAPE) {
        cancelModal();
        return;
    }
    if(key == KEY_ENTER) {
        if(mode == ProfileDialogMode :: DELETE_CONFIRM) deleteSelectedProfile();
        else commitTextEntry();
        return;
    }
    if(mode == ProfileDialogMode :: DELETE_CONFIRM) return;
    if(key == KEY_BACKSPACE) {
        if(!inputName.empty()) inputName.pop_back();
        return;
    }
    if(static_cast<int>(inputName.size()) >= MAX_PROFILE_NAME_LENGTH) return;

    char character = '\0';
    if(key >= KEY_A && key <= KEY_Z) {
        const bool upper = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        character = static_cast<char>((upper ? 'A' : 'a') + (key - KEY_A));
    }
    else if(key >= KEY_ZERO && key <= KEY_NINE) {
        character = static_cast<char>('0' + (key - KEY_ZERO));
    }
    else if(key == KEY_SPACE) character = ' ';
    else if(key == KEY_MINUS) character = '-';
    if(character != '\0') inputName += character;
}

void UserProfileScreen :: handleInput(const RawInputEvent& inputEvent) {
    if(inputEvent.inputType == RawInputEvent :: InputType :: KEY_PRESSED) {
        handleKeyboard(inputEvent.keySignal);
        return;
    }
    if(inputEvent.inputType != RawInputEvent :: InputType :: LEFT_MOUSE_CLICKED) return;

    if(mode == ProfileDialogMode :: SELECT) {
        const int count = userProfileManager
            ? std :: min(userProfileManager -> getProfileCount(), MAX_VISIBLE_PROFILES)
            : 0;
        for(int index = 0; index < count; ++index) {
            if(CheckCollisionPointRec(inputEvent.position, toScreenBounds(getProfileRowBounds(index)))) {
                selectedProfileIndex = index;
                return;
            }
        }
        if(count < MAX_VISIBLE_PROFILES &&
           CheckCollisionPointRec(inputEvent.position, toScreenBounds(getCreateProfileBounds()))) {
            beginCreateProfile();
            return;
        }
        if(CheckCollisionPointRec(inputEvent.position, toScreenBounds(RENAME))) {
            beginRenameProfile();
            return;
        }
        if(userProfileManager && userProfileManager -> getProfileCount() > 1 &&
           CheckCollisionPointRec(inputEvent.position, toScreenBounds(DELETE_BUTTON))) {
            mode = ProfileDialogMode :: DELETE_CONFIRM;
            return;
        }
        if(CheckCollisionPointRec(inputEvent.position, toScreenBounds(OK))) {
            if(userProfileManager && userProfileManager -> selectProfile(selectedProfileIndex)) {
                requestTransition(ScreenAction :: POP);
            }
            return;
        }
        if(CheckCollisionPointRec(inputEvent.position, toScreenBounds(CANCEL))) {
            requestTransition(ScreenAction :: POP);
        }
        return;
    }

    if(CheckCollisionPointRec(inputEvent.position, toScreenBounds(OK))) {
        if(mode == ProfileDialogMode :: DELETE_CONFIRM) deleteSelectedProfile();
        else commitTextEntry();
        return;
    }
    if(CheckCollisionPointRec(inputEvent.position, toScreenBounds(CANCEL))) cancelModal();
}
