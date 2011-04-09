#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <QVector2D>
#include <QPointF>

#include "Unit.h"
#include "Effect.h"

namespace td {

class Projectile : public Unit {
    Q_OBJECT

public:
    /**
     * Gets the unique class index for this object type.
     *
     * @author Darryl Pogue
     * @return The class index.
     */
    static unsigned char clsIdx() {
        return td::clsidx::kProjectile;
    }

private:
    enum {
        /* GameObject properties */
        kPosition       = (1 << 0),
        kOrientation    = (1 << 1),
        kScale          = (1 << 2),

        /* Projectile properties */
        kDamage         = (1 << 3),
        kStartPos       = (1 << 4),
        kEndPos         = (1 << 5),
        kType           = (1 << 6)
    };

public:
    Projectile(QObject* parent = 0);

    /**
     * Initializes the Projectile components based on type
     *
     * @author Marcel Vangrootheest
     */
    virtual void initComponents();

    /**
     * Sets path of the projectile for input
     *
     * @param source The source point of the path.
     * @param target The target point of the path.
     * @param enemy  The enemy unit firing at.
     *
     * @author Marcel Vangrootheest
     */
    void setPath(QPointF source, QPointF target, Unit* enemy);

    /**
     * Reads the object state from a network stream.
     * You should assign to variables directly inside this function, rather
     * than using mutator methods to change the values.
     *
     * @author Darryl Pogue
     * @param s The network stream.
     */
    virtual void networkRead(td::Stream* s);

    /**
     * Writes the object state to a network stream.
     *
     * @author Darryl Pogue
     * @param s The network stream.
     */
    virtual void networkWrite(td::Stream* s);

    /**
     * Sets the Projectile type (Arrow, Cannon, Fire, Tar, or Flak).
     *
     * @author Marcel Vangrootheest.
     * @param type Projectile type
     */
    void setType(int type) {
        type_ = type;
        setDirty(kType);
    }

    virtual void update();

    size_t getDamage(){
        return damage_;
    }

    void setDamage(size_t damage){
        damage_ = damage;
        setDirty(kDamage);
    }

    /**
     * Returns the original starting point of the projectile.
     *
     * @author Marcel Vangrootheest
     * @return the original starting position of the projectile
     */
    QPointF* getStartPoint(){
        return start_;
    }

    /**
     * Sets the original starting point of the projectile.
     *
     * @author Marcel Vangrootheest
     * @param point, the point to set the start location as
     */
    void setStartPoint(QPointF* point){
        start_ = point;
        setDirty(kStartPos);
    }

    /**
     * Returns the original ending point of the projectile.
     *
     * @author Marcel Vangrootheest
     * @return the original ending position of the projectile
     */
    QPointF* getEndPoint(){
        return end_;
    }

    /**
     * Sets the ending point of the projectile.
     *
     * @author Marcel Vangrootheest
     * @param point, the ending point set the end location as
     */
    void setEndPoint(QPointF* point){
        end_ = point;
        setDirty(kEndPos);
    }

    /**
     * Returns the line that makes up the current position and ending position
     *
     * @author Marcel Vangrootheest
     * @return the current path of the projectile
     */
    QLineF& getPath() {
        return path_;
    }

    /**
     * Sets the current path to path.
     *
     * @author Marcel Vangrootheest
     * @param path, the new path for the projectile
     */
    void setPath(QLineF& path) {
       path_ = path;
    }

    Unit* getEnemy(){
        return enemy_;
    }

    void setEnemy(Unit* enemy){
        enemy_ = enemy;
    }
    /**
     * Checks for collision between projectile and npcs
     * and applies effects to hit npcs.
     *
     * @author Daniel Wright
     * @param npcs, set of npcs to check collision with
     */
    void checkNPCCollision(QSet<Unit*> npcs);

    /**
    * Creates a bounding polygon based on the projectiles end point.
    *
    * @author Daniel Wright
    */
    void createBounds();

    int getWidth(){
        return width_;
    }

    void setWidth(int width){
        width_ = width;
    }

    int getHeight(){
        return height_;
    }

    void setHeight(int height){
        height_ = height;
    }

signals:
    /**
     * Signal to notify NPC of projectile collision.
     * For applying the effect specified to the NPC.
     *
     * @author Marcel Vangrootheest
     */
    void ProjectileCollision(Effect* effect);

private:
    size_t damage_;
    /**
     * Initial start position of the projectile.
     * This needs to be initialized before physics component is set.
     */
    QPointF* start_;
    /**
     * Initial end position of the projectile.
     * This needs to be initialized before physics component is set.
     */
    QPointF* end_;
    /**
     * Current path of the projectile. Second position should be set to start
     * or the current projectile position. First position should be set to end.
     */
    QLineF path_;

    QList<Effect*> effects_;

    Unit* enemy_;
    int height_;
    int width_;
    int type_;
};

} /* end namespace td */

#endif // PROJECTILE_H
