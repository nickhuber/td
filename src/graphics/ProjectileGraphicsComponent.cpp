#include "ProjectileGraphicsComponent.h"
#include "../engine/Projectile.h"

ProjectileGraphicsComponent::ProjectileGraphicsComponent()
        : GraphicsComponent() {
    /* Do init-type stuff here */
}

ProjectileGraphicsComponent::~ProjectileGraphicsComponent() {}

void ProjectileGraphicsComponent::update(GameObject* obj) {
    Projectile* projectile = (Projectile*)obj;
    //should cast it to projectile
    if (!projectile->getDirtyStatus()) //checks if object is dirty.
        return;
    projectile->setToClean();

    QGraphicsPixmapItem* itm = getPixmapItem();

    if (itm != NULL) {
        DrawParams* dp = new DrawParams();
        dp->pos     = projectile->getPos();
        dp->moving  = 1; //is always moving
        //player->getVelocity().length() != 0;
        dp->scale   = projectile->getScale(); // will likely be a constant value here
        dp->degrees = projectile->getOrientation();
        emit signalDraw(dp, this);
    }
}

void ProjectileGraphicsComponent::initPixmaps() {
    //TODO: add animation images here
    pixmapImgs = new QPixmap[PIX_PROJECTILE_MAX];
    pixmapIndex = 0;
    pixmapImgs[pixmapIndex++] = PIX_PROJECTILE_0;

    pixmapIndex = 0; //sets image back to start
}

