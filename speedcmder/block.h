#pragma once

#include "const.h"


class Block {
public:
    Block(const int id, const Vec2& pos) {
        id_ = id;
        pos_ = pos;
    }

    void draw() {
        RectF r = RectF(BSZ, BSZ).setCenter(pos_);
        r(TextureAsset(BLOCK_TEX_FN[id_])).draw();
    }

    const int id() { return id_; }
    const Vec2& pos() { return pos_; }

private:
    int id_;
    Vec2 pos_;
};

