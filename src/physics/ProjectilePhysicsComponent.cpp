#include "ProjectilePhysicsComponent.h"
#include "../engine/Projectile.h"
#include "../engine/Map.h"
#include "../engine/Tile.h"
#include "../engine/CDriver.h"

namespace td {

ProjectilePhysicsComponent::ProjectilePhysicsComponent(double velocity) {
    maxVelocity_ = accel_ = decel_ = velocity;
    duration_ = -1;
    increment_ = 0;
}
ProjectilePhysicsComponent::~ProjectilePhysicsComponent() {}

void ProjectilePhysicsComponent::update(GameObject* projectile) {
    this->applyForce((Projectile*)projectile);
    this->applyVelocity((Projectile*)projectile);
    this->setScale((Projectile*)projectile);
}

void ProjectilePhysicsComponent::applyVelocity(Projectile* projectile)
{
    QPointF newPos = projectile->getPos()
                     + projectile->getVelocity().toPointF();

    projectile->setPos(newPos);

}

void ProjectilePhysicsComponent::applyForce(Projectile* projectile)
{
    float velX, velY;
    QVector2D force = projectile->getForce();
    QVector2D vector = force * projectile->getVelocity();
    QVector2D tempVector = projectile->getVelocity();

    if (vector.x() >= 0) {
        tempVector.setX(force.x() * accel_ + tempVector.x());
    } else {
        tempVector.setX(force.x() *(accel_ + decel_) + tempVector.x());
    }

    if (vector.y() >= 0) {
        tempVector.setY(force.y() * accel_ + tempVector.y());
    } else {
        tempVector.setY(force.y() *(accel_ + decel_) + tempVector.y());
    }
    if (tempVector.length() > maxVelocity_) {
        projectile->getVelocity().setX(tempVector.normalized().x()
                                       * maxVelocity_);
        projectile->getVelocity().setY(tempVector.normalized().y()
                                       * maxVelocity_);
    } else {
        projectile->getVelocity().setX(tempVector.x());
        projectile->getVelocity().setY(tempVector.y());
    }

    if (force.x() == 0) {
        // deceleration towards 0
        if ((velX = projectile->getVelocity().x()) > 0) {
            if ((velX - decel_) < 0) {
                projectile->getVelocity().setX(0);
            } else {
                projectile->getVelocity().setX(velX - decel_);
            }
        } else if ((velX = projectile->getVelocity().x()) < 0) {
            if ((velX + decel_) > 0) {
                projectile->getVelocity().setX(0);
            } else {
                projectile->getVelocity().setX(velX + decel_);
            }
        }
    }

    if (force.y() == 0) {
        // deceleration towards 0
        if ((velY = projectile->getVelocity().y()) > 0) {
            if ((velY - decel_) < 0) {
                projectile->getVelocity().setY(0);
            } else {
                projectile->getVelocity().setY(velY - decel_);
            }
        } else if ((velY = projectile->getVelocity().y()) < 0) {
            if ((velY + decel_) > 0) {
                projectile->getVelocity().setY(0);
            } else {
                projectile->getVelocity().setY(velY + decel_);
            }
        }
    }
}



} /* end namespace td */
