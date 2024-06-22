#include "lookup.hpp"

#include "mod.hpp"
#include "fcaseopen.hpp"
#include "file.hpp"

#include "../string_proxy.hpp"


#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <algorithm>
#include <string>
#include <vector>
#include <set>

using namespace std::experimental::filesystem;

std::string fixFileCase(std::string in) {
  char* result = fixFileCase(strdup_s(in.c_str()));
  std::string resultStr = result;
  free(result);
  return resultStr;
}

bool endsWith(std::string const &fullString, std::string const &ending) {
  int fsLength = fullString.length();
  int endLength = ending.length();
  if (fsLength >= endLength) {
    return (0 == fullString.compare(fsLength - endLength, endLength, ending));
  } else {
    return false;
  }
}

std::vector<std::string> lookupFileCandidates(std::string filename, uint32_t flags) {
  std::vector<std::string> candidates;
  bool ws = !(flags & _lookupExcludeWorkshop);
  bool anyMod = (flags & _lookupForceMod) || (
      isModsEnabled() && (
       !(flags & _lookupExcludeBaseMods) ||
       !(flags & _lookupExcludeWorkshopMods)));

  if (!(flags & _lookupExcludeWorkshop)) {
    candidates.push_back("workshop/" + filename);
  }

  if (anyMod) {
    std::string modFile = modDirectoryNonNull() + filename;
    if (!(flags & _lookupExcludeWorkshopMods)) {
      candidates.push_back("workshop/" + modFile);
    }
    if (!(flags & _lookupExcludeBaseMods)) {
      candidates.push_back(modFile);
    }
  }

  if (!(flags & _lookupExcludeBase)) candidates.push_back(filename);

  for (int i = 0; i < candidates.size(); i++) {
    candidates[i] = fixFileCase(candidates[i]);
  }

  return candidates;
}

std::vector<std::string> lookupFileVersions(std::string filename, uint32_t flags) {
  std::vector<std::string> results;
  std::vector<std::string> candidates = lookupFileCandidates(filename, flags);

  for (int i = 0; i < candidates.size(); i++) {
    if (fileExists(candidates[i])) {
      results.push_back(candidates[i]);
    }
  }

  return results;
}

std::string lookupFile(std::string filename, uint32_t flags) {
  std::vector<std::string> versions = lookupFileVersions(filename, flags);
  if (versions.size() == 0) return filename;
  return versions[0];
}

/*
bool compareStrings(std::string a, std::string b) {
  return strcmpi_s(a.c_str(), b.c_str()) < 0;
}
*/

bool compareStrings(const std::string& lhs,const std::string& rhs){

  std::string::size_type common_length = std::min(lhs.length(),rhs.length());

   for(std::string::size_type i=0;i<common_length;++i){
      if(toupper(lhs[i]) < toupper(rhs[i]))return true;
      if(toupper(lhs[i]) > toupper(rhs[i]))return false;
   }

   if(lhs.length()<rhs.length())return true;
   if(lhs.length()>rhs.length())return false;//can ignore

   return false;//equal should return false
}

struct lessString : std::binary_function <std::string, std::string, bool> {
  bool operator() (std::string const& x, std::string const& y) const {
    return compareStrings(x, y);
  }
};

std::vector<std::string> lookupDirectory(std::string dir, std::string ext, uint32_t flags) {
  std::vector<std::string> dirVersions = lookupFileVersions(dir, flags);
  int extLength = ext.length();
  lessString helper;

  std::set<std::string, lessString> resultSet;
  for (int i = 0; i < dirVersions.size(); i++) {
    std::error_code err;
    for(auto f: directory_iterator(dirVersions[i], err)) {
      std::string filename = f.path().filename().string();
      if (endsWith(filename, ext)) {
        std::string filenameWOExt =
          filename.substr(0, filename.length() - extLength);
        resultSet.insert(filenameWOExt);
      }
    }
  }

  std::vector<std::string> result(resultSet.begin(), resultSet.end());
  sort(result.begin(), result.end(), helper);
  return result;
}

std::vector<std::string> lookupSubDirectories(std::string dir, uint32_t flags) {
  std::vector<std::string> dirVersions = lookupFileVersions(dir, flags);
  lessString helper;

  std::set<std::string, lessString> resultSet;
  for (int i = 0; i < dirVersions.size(); i++) {
    std::error_code err;
    for(auto f: directory_iterator(dirVersions[i], err)) {
      if (is_directory(f.path().string())) {
        std::string filename = f.path().filename().string();
        resultSet.insert(filename);
      }
    }
  }

  std::vector<std::string> result(resultSet.begin(), resultSet.end());
  sort(result.begin(), result.end(), helper);
  return result;
}

std::string lookupSave(std::string filename) {
  return modDirectoryNonNull() + filename;
}

std::vector<std::string> listModpacks(uint32_t flags) {
  return lookupSubDirectories("modpacks",
      flags | _lookupExcludeBaseMods | _lookupExcludeWorkshopMods);
}

/// Returns all existing designs, including design packages and legacy designs,
/// sorted alphabetically, with no duplicates, case insensitive.
/// Design packages end with '/', legacy designs do not.
/// The filenames returned do not have the file extension.
std::vector<std::string> lookupDesigns(uint32_t flags) {
  // legacy
  std::vector<std::string> result = lookupDirectory("designs", ".design", flags);

  // packages
  std::vector<std::string> packages = lookupSubDirectories("designs", flags);
  for (int i = 0; i < packages.size(); i++) {
    std::string package = packages[i];
    std::string canonDesignFile = lookupFile("designs/" + package + "/design.design", flags);
    if (!fileExists(canonDesignFile)) continue;
    package += "/";
    result.push_back(package);
  }

  // sort
  lessString helper;
  sort(result.begin(), result.end(), helper);
  return result;
}

