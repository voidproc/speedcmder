#pragma once

// ウィンドウサイズ
extern const int WW;
extern const int WH;
extern const int SCALE;

// ブロックの大きさ
extern const int BSZ;

// フィールドの大きさ
extern const int W;
extern const int H;

// ブロックの色（暫定）
extern const Color COLORS[];

extern const String BLOCK_TEX_FN[];

extern const double ANIM_T;
extern const int ANIM_TI;

// ゲームの状態
enum class State {
    TITLE,
    COMMAND,
    EXEC,
    JUDGE,
    OVER,
};
