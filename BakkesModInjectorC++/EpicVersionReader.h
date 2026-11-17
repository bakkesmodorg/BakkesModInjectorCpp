#pragma once

#include "json.hpp"
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

// Kept independent of Windows/Qt so all three manifest formats can be tested.
namespace EpicVersionReader
{
using Json = nlohmann::json;

inline std::string String(const Json& object, const char* key)
{
    const auto field = object.find(key);
    return field != object.end() && field->is_string() ? field->get<std::string>() : "";
}

inline std::vector<std::string> ReadInstallations(std::istream& input)
{
    std::map<std::string, Json> latest;
    std::string line;
    while (std::getline(input, line))
    {
        // Tolerate a UTF-8 BOM, blank lines and a partially written final line.
        if (line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line.erase(0, 3);
        auto record = Json::parse(line, nullptr, false);
        if (!record.is_object())
            continue;
        const auto id = String(record, "installationId");
        if (!id.empty())
            latest[id] = std::move(record);
    }

    std::vector<std::string> versions;
    for (const auto& entry : latest)
    {
        const auto& record = entry.second;
        const auto complete = record.find("complete");
        // Resolve repeats before filtering: an incomplete latest record must
        // invalidate an earlier complete record. pendingVersion is never installed.
        if (String(record, "artifactId") != "Sugar" ||
            String(record, "namespace") != "9773aa1aa54f4f7b80e44bef04986cea" ||
            complete == record.end() || !complete->is_boolean() || !complete->get<bool>())
            continue;
        const auto version = String(record, "version");
        if (!version.empty())
            versions.push_back(version);
    }
    return versions;
}

inline Json ReadJson(const std::filesystem::path& path)
{
    std::ifstream input(path);
    return Json::parse(input, nullptr, false);
}

inline std::vector<std::string> ReadVersions(const std::filesystem::path& installations,
    const std::filesystem::path& manifests, const std::filesystem::path& legendary)
{
    std::ifstream input(installations);
    auto versions = ReadInstallations(input);

    // Only the original ProgramData manifests are read. eglv2/import-scan
    // contains migration snapshots, not the current installation state.
    std::error_code error;
    std::filesystem::directory_iterator it(manifests, error), end;
    while (!error && it != end)
    {
        if (it->path().extension() == ".item" && it->is_regular_file(error))
        {
            const auto record = ReadJson(it->path());
            if (record.is_object() &&
                (String(record, "MandatoryAppFolderName") == "rocketleague" ||
                 String(record, "LaunchExecutable").find("RocketLeague") != std::string::npos))
            {
                const auto version = String(record, "AppVersionString");
                if (!version.empty())
                    versions.push_back(version);
            }
        }
        it.increment(error);
    }

    const auto record = ReadJson(legendary);
    if (record.is_object())
    {
        const auto sugar = record.find("Sugar");
        if (sugar != record.end() && sugar->is_object())
        {
            const auto version = String(*sugar, "version");
            if (!version.empty())
                versions.push_back(version);
        }
    }
    return versions;
}
}
