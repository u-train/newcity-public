#pragma once

#include <cstdint>
#include <string>
#include <vector>

const uint32_t _lookupForceMod = 1 << 0;
const uint32_t _lookupExcludeWorkshop = 1 << 1;
const uint32_t _lookupExcludeWorkshopMods = 1 << 2;
const uint32_t _lookupExcludeBaseMods = 1 << 3;
const uint32_t _lookupExcludeBase = 1 << 4;

const uint32_t _lookupOnlyBase = _lookupExcludeWorkshop |
                                 _lookupExcludeWorkshopMods |
                                 _lookupExcludeBaseMods;

const uint32_t _lookupOnlyMods = _lookupExcludeWorkshop | _lookupExcludeBase;

const uint32_t _lookupExcludeMods =
    _lookupExcludeWorkshopMods | _lookupExcludeBaseMods;

/// Returns all candidate locations for a file,
/// including some that may not exist.
std::vector<std::string> lookupFileCandidates(std::string filename,
                                              uint32_t flags);

/// Returns all locations a file can be found,
/// and only those that actually exist.
std::vector<std::string> lookupFileVersions(std::string filename,
                                            uint32_t flags);

/// Returns an existing canonical location for a file.
/// If the file doesn't exist anywhere, returns filename.
std::string lookupFile(std::string filename, uint32_t flags);

/// Returns all existing files in every version of a directory,
/// sorted alphabetically, with no duplicates, case insensitive.
/// The filenames returned do not have the file extension.
std::vector<std::string> lookupDirectory(std::string dir, std::string ext,
                                         uint32_t flags);

/// Returns all existing designs, including design packages and legacy designs,
/// sorted alphabetically, with no duplicates, case insensitive.
/// Design packages end with '/', legacy designs do not.
/// The filenames returned do not have the file extension.
std::vector<std::string> lookupDesigns(uint32_t flags);

/// Returns all existing subdirectories in every version of a directory,
/// sorted alphabetically, with no duplicates, case insensitive.
/// The filenames returned do not have the file extension.
std::vector<std::string> lookupSubDirectories(std::string dir, uint32_t flags);

/// Returns the preferred save location for a file.
/// This is always the player's current modpack,
/// and never in the workshop directory.
std::string lookupSave(std::string filename);

/// Returns all existing modpacks.
std::vector<std::string> listModpacks(uint32_t flags);
