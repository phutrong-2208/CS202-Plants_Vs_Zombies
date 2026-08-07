#include "Screens/Screen.hpp"

void Screen :: setAssetManager(AssetManager* manager){
    assetManager = manager;
}

void Screen :: setUserProfileManager(UserProfileManager* manager){
    userProfileManager = manager;
}

void Screen :: requestTransition(
    ScreenAction action,
    ScreenID target,
    ScreenData data
) {
    pendingTransition = {action, target, std :: move(data)};
}

ScreenTransition Screen :: consumeTransition() {
    const ScreenTransition transition = pendingTransition;
    pendingTransition = {};
    return transition;
}
