#include "Core/AnimationManager.hpp"

void AnimationManager::addAnimationData(const std::string& animationID, std::unique_ptr <ReanimParser> parser) {
    animationData[animationID] = std::move(parser);
}
ReanimParser* AnimationManager::getAnimationData(const std::string& animationID) const {
    auto it = animationData.find(animationID);
    if (it != animationData.end()) return it -> second.get();
    
    return nullptr;
}