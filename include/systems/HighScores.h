#pragma once
#include <vector>
#include <string>

struct ScoreEntry 
{
    std::string name;
    int score{ 0 };
};

class HighScores 
{
public:

    bool load(const std::string& path);
    bool save(const std::string& path) const;

    void submit(const std::string& name, int score); // add and sort
    const std::vector<ScoreEntry>& entries() const { return data_; }
    std::vector<ScoreEntry> topN(size_t n) const;

private:
    std::vector<ScoreEntry> data_;
    static void sortDesc(std::vector<ScoreEntry>& v);
};