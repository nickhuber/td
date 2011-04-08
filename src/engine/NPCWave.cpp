#include "NPCWave.h"
#include "Driver.h"
#include "../network/netmessages.h"

namespace td {

NPCWave::NPCWave(QObject* parent) : QObject(parent), start_(10),
        count_(5), type_(NPC_NORM), created_(0) {
}

NPCWave::NPCWave(QObject* parent, unsigned int start, unsigned int count,
        unsigned int type) : QObject(parent), start_(start), 
        count_(count), type_(type), created_(0) {
}

NPCWave::~NPCWave() { }

void NPCWave::createWave() {
    created_ = 1;

    NPC* npc = getDriver()->createNPC(type_);
    children_.insert(npc);

    connect(getDriver()->getTimer(), SIGNAL(timeout()),
            this, SLOT(update()));
}

void NPCWave::update() {
    static unsigned int tickmod = 0;
    if (created_ < count_ && (++tickmod % 3000) > 300) {
        created_++;
        NPC* npc = getDriver()->createNPC(type_);
        children_.insert(npc);
    }

    Stream s;
    s.writeInt(children_.size());
    foreach (NPC* npc, children_) {
        npc->update();
        npc->networkWrite(&s);
    }

    getDriver()->sendNetMessage(network::kNPCWave, s.data());
}

}