#ifndef SCREEN_TRANSITION_HPP
#define SCREEN_TRANSITION_HPP

enum class ScreenID {
    GAME_PLAY,
    MAIN_MENU,
    PAUSE_MENU,
    LOAD_MENU
};

enum class ScreenAction {
    NONE,
    PUSH,
    POP,
    REPLACE,
    CLEAR_AND_PUSH
};

struct ScreenTransition {
    ScreenAction action = ScreenAction :: NONE;
    ScreenID target = ScreenID :: MAIN_MENU;
};

#endif
