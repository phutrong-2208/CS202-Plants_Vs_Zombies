#include "Gameplay/Zombies/NewspaperZombie.hpp"

void NewspaperZombie::onArmorBroken() {
    Zombie::onArmorBroken();
    if (!isAngry) {
        isAngry = true;
        speed = 36.0f; // Double speed sprint!
        animation.hideTrack("Zombie_paper_paper1");
        animation.hideTrack("Zombie_paper_paper2");
        animation.hideTrack("Zombie_paper_paper3");
        animation.hideTrack("Zombie_paper_glasses");
        animation.hideTrack("anim_paper");
    }
}
