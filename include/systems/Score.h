#pragma once
class Score 
{
public:
    void reset() { value_ = 0; }
    void add(int p) { value_ += p; }
    int value() const { return value_; }
private:
    int value_{ 0 };
};