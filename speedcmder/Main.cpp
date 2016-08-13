#include "fontdrawer.h"
#include "const.h"
#include "effect.h"
#include "command.h"
#include "block.h"

class Field {
    class FieldCursor {
    public:
        FieldCursor(const int col = 0) { col_ = col; colPrev_ = col; }
        void moveLeft() { colPrev_ = col_;  col_ = Clamp(col_ - 1, 0, W - 1); }
        void moveRight() { colPrev_ = col_;  col_ = Clamp(col_ + 1, 0, W - 1); }
        const int col() { return col_; }
        const int colPrev() { return colPrev_; }
    private:
        int col_;
        int colPrev_;
    };

public:
    Field(const int mb, const int ml, Effect& effect) : effect_(effect) {
        field_.resize(W);
        for (auto& col : field_) {
            col.reserve(H);
        }
        showCursor_ = true;
        hold_.reserve(100);
        marginBottom_ = mb;
        marginLeft_ = ml;
    }

    //  [row]
    //    3
    //    2
    //    1
    //    0
    //    - 0 1 2 3 [col]
    const Vec2 blockPos(const int row, const int col) {
        return Vec2(marginLeft_ + col * BSZ + BSZ / 2, WH - marginBottom_ - row * BSZ - BSZ / 2);
    }

    // col列目にブロックidを落とす
    bool addBlock(const int col, const int id) {
        if (field_[col].size() < H) {
            field_[col].push_back(Block(id, blockPos(field_[col].size(), col)));

            // add effect
            for (int i = 0; i < 6; i++) {
                effect_.add<BubbleEffect>(
                    Vec2(
                        marginLeft_ + BSZ*col - 8 + Random(1.0*BSZ + 8*2),
                        WH - marginBottom_ - BSZ*(field_[col].size() - 1) + Random(-8.0, 8.0)));
            }

            return true;
        }
        return false;
    }

    void clear() {
        for (auto& col : field_) {
            col.clear();
        }
        hold_.clear();
    }

    void drawCursor(const int anim_t) {
        int x1 = marginLeft_ + cur_.colPrev()*BSZ + BSZ / 2;
        int x2 = marginLeft_ + cur_.col()*BSZ + BSZ / 2;

        Rect(BSZ, BSZ*H)
            .setCenter({ EaseOut(x1, x2, Easing::Quint, min(1.0,anim_t/ANIM_T)), WH - marginBottom_ - BSZ*H / 2 })
            .drawFrame(1.0, 1.0, { 255,255,200,((System::FrameCount() / 2) % 2) * 255U });

        // hold
        for (int i = 0; i < hold_.size(); i++) {
            Rect(BSZ, 4)
                .setCenter({ EaseOut(x1, x2, Easing::Quint, min(1.0,anim_t/ANIM_T)), WH - marginBottom_ - BSZ*H - 5 * i - 6 })
                .draw(COLORS[hold_[i]]);
        }
    }

    void draw(const int anim_t) {
        // bg, wall
        TextureAsset(L"fbg").drawAt({ marginLeft_ + BSZ*W / 2, WH - marginBottom_ - BSZ*H / 2 });
        Rect(BSZ*W, BSZ*H)
            .setCenter({ marginLeft_ + BSZ*W / 2, WH - marginBottom_ - BSZ*H / 2 })
            .draw({ 0U, 0U, 0U, 180U });
            //.drawFrame(0.0, 4.0, Palette::Gray);

        // blocks
        for (auto& col : field_) {
            for (auto& b : col) {
                b.draw();
            }
        }

        // cursor
        if (showCursor_) {
            drawCursor(anim_t);
        }
    }

    void hold() {
        if (field_[cur_.col()].size() > 0) {
            hold_.push_back(field_[cur_.col()].back().id());

            effect_.add<BlockEraseEffect>(field_[cur_.col()].back().pos(), field_[cur_.col()].back().id());

            field_[cur_.col()].pop_back();
        }
    }

    void addHoldBlock() {
        if (hold_.size() > 0 && field_[cur_.col()].size() < H) {
            addBlock(cur_.col(), hold_.back());
            hold_.pop_back();
        }
    }

    void makeField(vector<int>& q) {
        for (int i : q) {
            while (!addBlock(Random(0, W - 1), i)) {}
        }
    }

    void resetCursor() { cur_ = FieldCursor(Random(0, W - 1)); }
    void moveCursorLeft() { cur_.moveLeft(); }
    void moveCursorRight() { cur_.moveRight(); }
    void showCursor(const bool show) { showCursor_ = show; }

    const String toString() {
        String ret = L"";
        for (auto& col : field_) {
            String s = L"";
            for (auto& b : col) {
                s += Format(b.id());
            }
            ret += s.padRight(H, L' ');
        }
        return ret;
    }

private:
    vector<vector<Block>> field_;
    vector<int> hold_;
    FieldCursor cur_;
    bool showCursor_;
    int marginBottom_;
    int marginLeft_;
    Effect& effect_;
};

// 問題（ステージ）を作る
// Field::makeField()に渡す用
vector<int> makeQ(const int colors, const int n)
{
    vector<int> ids = { 1,2,3,4,5,6,7 };
    shuffle(ids.begin(), ids.end(), mt19937());

    vector<int> v;
    v.reserve(50);
    for (int i = 0; i < n; i++) {
        v.push_back(ids[Random(1, colors)-1]);
    }
    return v;
}

void Main()
{
    Window::Resize(Size(WW, WH) * SCALE);
    Graphics2D::SetBlendState(BlendState::Default);
    Graphics2D::SetSamplerState(SamplerState::ClampPoint);
    RenderTexture render_texture{ Size(WW, WH), Palette::Black };

    // font
    FontManager::Register(L"Assets/FFFIXING_5.TTF");
    FontAsset::Register(L"fontM", 6, L"FF Fixing 5", FontStyle::Bitmap);
    FontDrawer font(L"fontM");

    // textures
    for (int i = 1; i <= 7; i++) {
        TextureAsset::Register(BLOCK_TEX_FN[i], L"Assets/" + BLOCK_TEX_FN[i]);
    }
    TextureAsset::Register(L"fbg", L"Assets/fbg.png");

    Effect effect;

    Field field(30, 25, effect);
    Field field_ans(30, 25+BSZ*W+35, effect);
    field_ans.showCursor(false);

    vector<int> q = makeQ(2, 3);
    field.makeField(q);
    field_ans.makeField(q);

    Command cmd;
    wchar currentCmd = L' ';
    
    State state = State::TITLE;
    int frame = 0; //1～ANIM_T-1までカーソルアニメーション用
    int remain = 181*60; //残り時間
    int stage = 1;
    bool titleKeyPress = false;

    while (System::Update()) {
        Graphics2D::SetRenderTarget(render_texture);

        const int F = System::FrameCount();

        // bg
        if (state != State::OVER) {
            Rect(WW, WH).draw({ 0U, 20U, (uint32)(50U + 30U * (1.0 + sin(F / 50.0))) });
            for (int i = 0; i < WH / 50 + 1; i++) {
                Line(0, i * 50 + F % 50, WW, i * 50 + F % 50).draw(1, { 20U, 60U, (uint32)(100U + 40U * (1.0 + sin(F / 4.0))) });
            }
            Rect(WW, 15).draw(Palette::Black);
        }
        else {
            Rect(WW, WH).draw({ (uint32)(30U + 30U * (1.0 + sin(F / 80.0))), 0U, 0U });
        }

        if (state == State::TITLE) {
            // bg
            Rect(WW, WH).draw({ 0U, 0U, 0U });

            font.draw(L"3 MINUTE SPEED COMMANDER v1.0", { 1, 1+15*0 }, Palette::Gainsboro);
            font.draw(L"@voidproc", { 1, 1+15*1 }, Palette::Gainsboro);

            if (Input::AnyKeyClicked()) {
                frame = 1;
                titleKeyPress = true;
            }

            if (titleKeyPress) {
                font.draw(L"READY", { 1, 1+15*3 }, Palette::Gainsboro);
                if (frame >= 70) {
                    state = State::COMMAND;
                    frame = 1;
                }
            }
            else {
                if ((F / 20) % 4 > 0) {
                    font.draw(L"PRESS ANY KEY TO CONTINUE", { 1, 1 + 15 * 3 }, Palette::Gainsboro);
                }
            }
        }
        else if (state == State::COMMAND) {
            // draw text cursor
            if ((frame / 10) % 2 == 0) {
                RectF region = font.region(cmd.toString(), { 100, 1 });
                RectF(region.x + region.w + 3, region.y, 2, region.h).draw(Palette::White);
            }

            // text: command
            font.draw(L"COMMAND > ", { 1, 1 }, Palette::White);
            font.draw(cmd.toString(), { 100, 1 }, Palette::White);

            // input
            cmd.input();
            if (cmd.decide()) {
                state = State::EXEC;
                frame = 101;
            }
        }
        else if (state == State::EXEC) {
            if (frame % ANIM_TI == 0) {
                if (!cmd.valid()) {
                    state = State::JUDGE;
                    frame = 101;
                }
                else {
                    currentCmd = cmd.currentCommand();
                    cmd.next();
                    
                    // execute command
                    if (currentCmd == L'L') {
                        field.moveCursorLeft();
                        frame = 1;//reset animation
                    }
                    else if (currentCmd == L'R') {
                        field.moveCursorRight();
                        frame = 1;//reset animation
                    }
                    else if (currentCmd == L'U') {
                        field.hold();
                    }
                    else if (currentCmd == L'D') {
                        field.addHoldBlock();
                    }
                }
            }

            // text: command
            font.draw(L"COMMAND > ", { 1, 1 }, Palette::Silver);
            font.draw(cmd.toString(), { 100, 1 }, Palette::Silver);

            // text: current command
            font.draw(cmd.toString().substr(0, cmd.idx()), { 100, 1 }, Palette::Yellow);
        }
        else if (state == State::JUDGE) {
            if (frame % 50 == 0) {
                if (field.toString() == field_ans.toString()) {
                    // to next stage
                    ++stage;
                    q = makeQ(min(7, 2+stage/4+Random(-1, 1)), min(12, 3+stage/5+Random(-1, 0)));
                    field.clear();
                    field.makeField(q);
                    field_ans.clear();
                    field_ans.makeField(q);

                    effect.add<FlashEffect>();
                }

                frame = 101;
                state = State::COMMAND;
                cmd.reset();
                currentCmd = L' ';
            }
        
            // text: judging
            font.draw(L"JUDGING...", { 1, 1 }, {255U,255U,0U,128U+127U*((System::FrameCount()/4)%2)});
        }
        else if (state == State::OVER) {
            // text: over
            font.draw(L"GAME OVER", { 1, 1 }, Palette::Red);
        }

        if (state != State::TITLE) {
            // text: remain
            int remain_sec = remain / 60;
            font.draw(Format(remain_sec, L" sec"), { WW - 70, WH - 15 }, (remain_sec > 10) ? Palette::Aliceblue : Palette::Red);

            // text: stage
            font.draw(Format(L"STAGE ", stage), { 1, WH - 15 }, Palette::Lime);

            // text: "MAKE THIS"
            font.draw(L"MAKE THIS", { 30 + BSZ*W + 60, WH - 30 - BSZ*H - 20 }, Palette::White);

            field.draw(frame);
            field_ans.draw(frame);

            if (remain <= 0) {
                state = State::OVER;
            }
            else {
                --remain;
            }
        }

        effect.update();

        Graphics2D::SetRenderTarget(Graphics::GetSwapChainTexture());
        render_texture.scale(SCALE).draw();

        ++frame;
    }
}
