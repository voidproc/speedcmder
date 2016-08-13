#pragma once

class Command {
public:
    Command() {
        reset();
    }

    void input() {
        if (!freeze_) {
            if (Input::KeyL.clicked) cmd_ += L"L";
            if (Input::KeyR.clicked) cmd_ += L"R";
            if (Input::KeyU.clicked) cmd_ += L"U";
            if (Input::KeyD.clicked) cmd_ += L"D";
            if (Input::KeyBackspace.clicked && !cmd_.isEmpty) cmd_.pop_back();
            if (Input::KeyEnter.clicked) freeze_ = true;
        }
    }

    const String toString() {
        return cmd_;
    }

    const wchar currentCommand() {
        return cmd_.at(idx_);
    }

    void next() {
        ++idx_;
    }

    const bool valid() {
        return !(idx_ >= (int)cmd_.length);
    }

    void reset() {
        cmd_ = L"";
        idx_ = 0;
        freeze_ = false;
    }

    const bool decide() { return freeze_; }

    const int idx() { return idx_; }

private:
    String cmd_;
    int idx_;
    bool freeze_;
};

