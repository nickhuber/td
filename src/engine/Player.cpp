#include "Player.h"
#include "Driver.h"
#include "CDriver.h"
#include "BuildingTower.h"
#include "tile.h"
#include "EffectTypes.h"
#include "../graphics/PlayerGraphicsComponent.h"
#include "../audio/SfxManager.h"
#include "../graphics/Console.h"
#include "Resource.h"

namespace td {

Player::Player(QObject* parent) 
        : Unit(parent), nickname_(""), harvesting_(RESOURCE_NONE), 
          harvestCountdown_(HARVEST_COUNTDOWN), harvestTime_(HARVEST_COUNTDOWN),
          resource_(RESOURCE_NONE), stunUpgrade_(false), upgrades_(PLAYER_NONE) 
{
    QVector2D force(0, 0);
    this->setForce(force);
}

Player::~Player()
{
    // Remove the unit from the map before deleting it
    this->getDriver()->getGameMap()->removeUnit(getPos().x(), getPos().y(), this);
}

void Player::networkRead(Stream* s) {
    Unit::networkRead(s);

    if (dirty_ & kNickname) {
        int len = s->readInt();
        nickname_ = QString(s->read(len));
    }
    if (dirty_ & kResource) {
        resource_ = s->readInt();
    }
    if (dirty_ & kMoving) {
        isMoving_ = s->readInt();
    }
}

void Player::networkWrite(Stream* s) {
    Unit::networkWrite(s);

    if (dirty_ & kNickname) {
        s->writeInt(nickname_.length());
        s->write(nickname_.toAscii());
    }
    if (dirty_ & kResource) {
        s->writeInt(resource_);
    }

    if (dirty_ & kMoving) {
        s->writeInt(isMoving_);
    }
}

void Player::initComponents() {
#ifndef SERVER
    /* Client-side has a Graphics Component */
    graphics_ = new PlayerGraphicsComponent(nickname_);
    graphics_->update(this);
#endif
}

void Player::update() {
    if (physics_ != NULL) {
        physics_->update(this);

        bool moving = velocity_.length() != 0;
        if (isMoving_ != moving) {
            isMoving_ = moving;
            setDirty(kMoving);
        }
    }
    if (isDirty()) {
        getDriver()->updateRT(this);
    }
    foreach (Effect* e, effects_) {
        e->update();
    }

    if (graphics_ != NULL) {
        graphics_->update(this);
    }
    if (harvesting_ != RESOURCE_NONE) {
        harvestResource();
    }

    if (physics_ != NULL) {
        tileThatPlayerIsOn_ = getDriver()->getGameMap()->getTile(getPos());
        int tileEffect = getDriver()->getGameMap()->getTile(getPos())->getTileEffect();
        switch(tileEffect) {
            case Tile::NONE:
                break;
            case Tile::SLOW:
                createEffect(EFFECT_SLOW);
                break;
            case Tile::FAST:
                createEffect(EFFECT_FAST);
                break;
            default:
                break;
        }
    }

    emit signalPlayerMovement(getPos());
}

void Player::createEffect(int effectType)
{
    // Check to see if this effect is already applied
    if (!effects_.contains(effectType))
    {
        Effect* effect;

        // Create the effect
        switch (effectType)
        {
        case EFFECT_FAST:
            if(effects_.contains(EFFECT_NPCPLAYER)) {
                return;
            }
            if(effects_.contains(EFFECT_FAST)) {
                return;
            }
            effect = new PlayerTerrainFastEffect(this);
            break;
        case EFFECT_SLOW:
            if(effects_.contains(EFFECT_NPCPLAYER)) {
                return;
            }
            if(effects_.contains(EFFECT_SLOW)) {
                return;
            }
            effect = new PlayerTerrainSlowEffect(this);
            break;
        case EFFECT_NPCPLAYER:
            foreach(Effect* e, effects_) {
                deleteEffect(e);
            }
            if (stunUpgrade_) {
                effect = new UpgradeNPCPlayerEffect(this);
            } else {
                effect = new NPCPlayerEffect(this);
            }
            break;
        default:
            return;
        }

        // Dean's sound signal thing
        emit signalEmptyEffectList();

        connect(effect, SIGNAL(effectFinished(Effect*)),
                     this, SLOT(deleteEffect(Effect*)));
        // Insert the effect into the map
        effects_.insert(effectType, effect);
    }
}

void Player::deleteEffect(int type)
{
    Effect* effect = effects_.take(type);
    delete effect;
    effect = NULL;
}

void Player::deleteEffect(Effect* effect)
{
    effects_.remove(effect->getType());
    delete effect;
    effect = NULL;
}

void Player::startHarvesting(int type) {
    if (resource_ != RESOURCE_NONE) {
        return;
    }
    harvesting_ = type;
    emit signalPlayerMovement(false);
    switch (type) {
        case RESOURCE_WOOD:
            PLAY_LOCAL_SFX(SfxManager::resourceWood);
            break;
        case RESOURCE_STONE:
            PLAY_LOCAL_SFX(SfxManager::resourceStone);
            break;
        case RESOURCE_BONE:
            PLAY_LOCAL_SFX(SfxManager::resourceBone);
            break;
        case RESOURCE_TAR:
            PLAY_LOCAL_SFX(SfxManager::resourceTar);
            break;
    }
}

void Player::stopHarvesting() {
    if (harvesting_ == RESOURCE_NONE) {
        return;
    }

    harvesting_ = RESOURCE_NONE;
    harvestCountdown_ = harvestTime_;
    emit signalPlayerMovement(true);
}

void Player::harvestResource() {
    if (--harvestCountdown_ <= 0) {
        resource_ = harvesting_;
        harvestCountdown_ = harvestTime_;
        setDirty(kResource);
        stopHarvesting();
        return;
    }
}

void Player::pickupCollectable(double x, double y, Unit* u) {
    Tile* t = getDriver()->getGameMap()->getTile(x, y);

    // First check to see if the collectable is a gem
    if(((Collectable*)u)->getType() == RESOURCE_GEM) {
        t->removeUnit(u);
        emit signalPickupCollectable(u->getID());
        return;
    }

    if (resource_ != RESOURCE_NONE) {
        return;
    }

    t->removeUnit(u);
    setResource(((Collectable*)u)->getType());
    emit signalPickupCollectable(u->getID());
}

} /* end namespace td */
