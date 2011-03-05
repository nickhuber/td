#include "Player.h"

Player::Player() : Unit() {
    QVector2D force(0, 0);
    this->setForce(force);

    this->setPos(100, 100);
}

void Player::update() {
    physics_->update(this);
    graphics_->update(this);
}

