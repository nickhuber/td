#include "../physics/TowerPhysicsComponent.h"
#include "../engine/Tower.h"
 #include <QLineF>
#include <typeinfo>
#define PI 3.141592653589793238
#include <math.h>

namespace td {

TowerPhysicsComponent::TowerPhysicsComponent() {
    enemy_ = 0;
}

void TowerPhysicsComponent::update(GameObject *tower) {
    this->applyDirection((Tower*)tower);
}

void TowerPhysicsComponent::findTargets(GameObject* tower, int radius) {
    QLineF target;
    target.p1() = tower->getPos();
    Unit* n = new NPC();
    if(( n = getEnemy()) != NULL) {
        target.p2() = getEnemy()->getPos();
    } else {
        target.p2() = tower->getPos();
    }
    Unit* u = new NPC();
    setNPCs(tower, radius);
    QSet<Unit*> units = getNPCs();

    QSet<Unit*>::iterator iter;

    if(units.isEmpty()) {
        return;
    }
    for( iter = units.begin();iter != units.end(); ++iter){
        u = *iter;
        QLineF line;
        line.p1() = tower->getPos();
        line.p2() = u->getPos();
        if(getEnemy() != NULL && target.length() < radius && target.length() != 0) {
            return;
        } else {
            QString debug = typeid( *u ).name();
            if(debug.contains("NPC")) {
                qDebug(debug.toLatin1().data());
                setTarget(u);
            }
        }
    }
}

void TowerPhysicsComponent::applyDirection(GameObject* tower) {


    this->findTargets(tower, 5);
    if(getEnemy() == NULL) {
        return;
    }
    int angle = 0;
    int degree = 0;
    int velX = getEnemy()->getPos().x() - tower->getPos().x();
    int velY = getEnemy()->getPos().y() - tower->getPos().y();

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

}
