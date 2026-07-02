#ifndef ANIMATION_MANAGER_HPP
#define ANIMATION_MANAGER_HPP

#include "Core/ReanimParser.hpp"

class AnimationManager {
private:
    std::map <std::string, std::unique_ptr<ReanimParser>> animationData;
public:
    AnimationManager() = default;

    void addAnimationData(const std :: string& animationID, std::unique_ptr <ReanimParser> parser);
    ReanimParser* getAnimationData(const std :: string& animationID) const;
};

#endif