#pragma once

class FontDrawer {
public:
    FontDrawer(const String& name) {
        fontAssetName_ = name;
        //font_ = Font(size, name, FontStyle::BitmapBold);
    }

    void draw(const String& text, const Vec2& pos, const Color& color = Palette::White, const Color& shadow = {100U,100U,100U}) {
        FontAsset(fontAssetName_).draw(text, pos.movedBy(2.0, 2.0), shadow);
        FontAsset(fontAssetName_).draw(text, pos, color);
    }

    RectF region(const String& text, const Vec2& pos) {
        return FontAsset(fontAssetName_).region(text, pos);
    }

private:
    String fontAssetName_;
    //Font font_;
};

