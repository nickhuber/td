#include "TowerPhysicsComponent.h"
#include "../engine/Tower.h"
#include "../engine/Player.h"
#include <typeinfo>
#define PI 3.141592653589793238
#include <math.h>

namespace td {

TowerPhysicsComponent::TowerPhysicsComponent(Tower* tower, size_t fireInterval, 
                                             int radius)
        : PhysicsComponent(), tower_(tower), enemies_(QSet<Unit*>()),
          target_(NULL), fireInterval_(fireInterval), radius_(radius), 
          fireCountdown_(0) {
}

TowerPhysicsComponent::~TowerPhysicsComponent() {}

/*
void TowerPhysicsComponent::update(GameObject *tower) {
    this->applyDirection((Tower*)tower);
    this->fire();
} */

void TowerPhysicsComponent::findTarget() {
    
    // check if there's an npc currently being tracked
    if (target_ != NULL) {
        projectilePath_.setP2(target_->getPos());
        // return if the npc is still within range

        if (projectilePath_.length() < radius_) {
            return;
        }
        disconnect(target_, SIGNAL(signalNPCDied()), this, SLOT(targetDied()));
        target_ = NULL;
    }
    projectilePath_.setP1(tower_->getPos());
    
    // get all npcs within range 
    Map* map = tower_->getDriver()->getGameMap();
    enemies_ = map->getUnits(tower_->getPos().x(), tower_->getPos().y(), 
                             ceil((double) radius_ / TILE_SIZE)); 
 
    if (enemies_.isEmpty()) {
        return;
    }

    QSet<Unit*>::iterator iter;

    for (iter = enemies_.begin(); iter != enemies_.end(); ++iter) {
        
        // this would be the place to add a priority algorithm if we need one
        // make sure that the unit is not a player
        if((((*iter)->getID()&0xFF000000)>>24) == NPC::clsIdx()) {
            projectilePath_.setP2((*iter)->getPos());

            // check that they're actually in range
            if (projectilePath_.length() < radius_) {
                target_ = *iter;
                connect(target_, SIGNAL(signalNPCDied()), 
                        this, SLOT(targetDied()));
                return;
            }
        }
    }
}

void TowerPhysicsComponent::applyDirection(GameObject* tower) {

    this->findTarget();
    if(target_ == NULL || enemies_.isEmpty()) {
        return;
    }
    int angle = 0;
    int degree = 0;
    int velX = target_->getPos().x() - tower->getPos().x();
    int velY = target_->getPos().y() - tower->getPos().y();

    if (velX == 0 && velY == 0) {
        return;
    }

    if (qAbs(velX) >= qAbs(velY)) {
        angle = atan(velY / (float)velX) * (180 / PI);

        if (velX > 0) {
            if (velY == 0) {
                degree = 0;
            } else if (velX == velY) {
                degree = 315;
            } else if (velX == (-velY)) {
                degree = 45;
            } else if (angle < 0) {
                degree =  (-angle);
            } else {
                degree = 360 - angle;
            }
        } else if (velX < 0) {
            if (velY == 0) {
                degree = 180;
            } else if (velX == velY) {
                degree = 135;
            } else if (velX == (-velY)) {
                degree = 225;
            } else {
                degree = 180 - angle;
            }
        }
    } else if (qAbs(velY) > qAbs(velX)) {
        angle = atan(velX / (float) velY) * (180 / PI);

        if (velY < 0) {
            if (velX == 0) {
                degree = 90;
            } else {
                degree = 90 + angle;
            }
        } else if (velY > 0) {
            if (velX == 0) {
                degree = 270;
            } else {
                degree = 270 + angle;
            }
        }
    }
    tower->setOrientation(degree);
}

void TowerPhysicsComponent::targetDied() {
    target_ = NULL;
}

}
