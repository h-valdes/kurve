#ifndef PLAYER_MESH_HPP
#define PLAYER_MESH_HPP

#include "mesh.hpp"
#include "user.hpp"

class PlayerMesh: public Mesh {
   protected:
    int parts = 6;
   public:
    PlayerMesh();
    virtual void draw() override;
};

#endif