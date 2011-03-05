#ifndef PLAYERPHYSICSCOMPONENT_H
#define PLAYERPHYSICSCOMPONENT_H

#include "PhysicsComponent.h"
#include "Player.h"

class PlayerPhysicsComponent : public PhysicsComponent {
public:
    PlayerPhysicsComponent();
    virtual ~PlayerPhysicsComponent();
    /**
     * Applies a force to the velocity
     * If force is 1 or -1:
     *      Acceleration applied on direction to velocity.
     *      Deceleration is added if velocity is approaching 0.
     * If force is 0:
     *      Deceleration is applied to velocity while velocity approaches 0.
     *
     * @author Marcel Vangrootheest
     * @param player, pointer to the player object
     */
    void applyForce(Player* player);
    /**
     * Applies a velocity to the position.
     * This function just adds the vector velocity to the point, pos
     *
     * @author Marcel Vangrootheest
     * @param player, pointer to the player object
     */
    void applyVelocity(Player* player);
    /**
     * Applies a direction to the position.
     * This function uses velocity to find angle, pos
     *
     * @author Joel Stewart
     * @param player, pointer to the player object
     */
    void applyDirection(Player* player);
    
    /**
     * This updates the physics properties of Player.
     * Applies force to velocity, applies velocity to position.
     *
     * @author Marcel Vangrootheest
     * @param player, pointer to the Unit(player object)
     */
    virtual void update(Unit* player);
    
private:
    /* data */
    float accel_;
    float decel_;
    float maxVelocity_;
};

#endif
