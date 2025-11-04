#include "systems/HighScores.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

void HighScores::sortDesc(std::vector<ScoreEntry>& v)
{
    std::sort(v.begin(), v.end(), [](auto& a, auto& b) { return a.score > b.score; });
}

bool HighScores::load(const std::string& path)
{
    data_.clear();
    std::ifstream in(path, std::ios::in);
    if (!in)
    {
        return true;
    }
    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        auto tab = line.rfind('\t');
        if (tab == std::string::npos) continue;

        ScoreEntry e;
        e.name = line.substr(0, tab);
        try 
        {
            e.score = std::stoi(line.substr(tab + 1));
        }
        catch (...) 
        {
            continue;
        }
        data_.push_back(std::move(e));
    }
    sortDesc(data_);
    return true;
}

bool HighScores::save(const std::string& path) const
{
    fs::create_directories(fs::path(path).parent_path());
    std::ofstream out(path, std::ios::out | std::ios::trunc);
    if (!out) return false;

    size_t limit = std::min<size_t>(data_.size(), 100);
    for (size_t i = 0; i < limit; ++i)
    {
        out << data_[i].name << '\t' << data_[i].score << '\n';
    }
    return true;
}

void HighScores::submit(const std::string& name, int score)
{
    data_.push_back(ScoreEntry{ name, score });
    sortDesc(data_);
    if (data_.size() > 100) data_.resize(100);
}

std::vector<ScoreEntry> HighScores::topN(size_t n) const
{
    std::vector<ScoreEntry> out;
    n = std::min(n, data_.size());
    out.reserve(n);
    for (size_t i = 0;i < n;++i) out.push_back(data_[i]);
    return out;
}