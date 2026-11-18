#pragma once

static const std::string GameExecutableFile = "default.xbe";
inline std::string GetMainGameFolder() { return (GetGamePath() / "game").string(); }
const std::string MainGameFolder = GetMainGameFolder();
static const std::string GameExecutable = MainGameFolder + "/" + GameExecutableFile;

struct FileSystem
{
    static std::filesystem::path ResolvePath(const std::string_view& path, bool checkForMods, bool logModFile = false);
};
