#pragma once

#include "const.h"

struct BubbleEffect : IEffect {
    BubbleEffect(const Vec2& pos) {
        pos_ = pos;
        vel_ = Vec2::One.rotated(Random(Math::TwoPi)).setLength(Random(0.5, 2.0));
    }

    bool update(double t) override {
        pos_ += vel_;
        Circle(pos_, 8.0 / (1 + 8*t)).draw({ 255U,255U,255U,40U + 200U * ((System::FrameCount() / 2) % 2) });
        vel_ *= 0.950;
        return (t <= 0.8);
    }

    Vec2 pos_;
    Vec2 vel_;
};

struct FlashEffect : IEffect {
    FlashEffect() {}

    bool update(double t) override {
        Rect(0, 0, WW, WH).draw({48U,255U,48U,(uint32)(128U*(0.35-t)*2.0)});
        return (t < 0.3);
    }
};

struct BlockEraseEffect : IEffect {
    BlockEraseEffect(const Vec2& pos, const int id) {
        pos_ = pos;
        id_ = id;
    }

    bool update(double t) override {
        Color c = COLORS[id_];
        Color cf = Palette::White;
        c.a = cf.a = (uint32)(255U * (0.85-t)/0.85);
        RectF(BSZ, BSZ)
            .setCenter(pos_)
            .draw(c)
            .drawFrame(2.0, 0.0, cf);
        return (t < 0.8);
    }

    Vec2 pos_;
    int id_;
};

