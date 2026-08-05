#include "Screens/Screen.hpp"

void Screen :: setAssetManager(AssetManager* manager){
    assetManager = manager;
}

void Screen :: requestTransition(ScreenAction action, ScreenID target) {
    pendingTransition = {action, target};
}

ScreenTransition Screen :: consumeTransition() {
    const ScreenTransition transition = pendingTransition;
    pendingTransition = {};
    return transition;
}
