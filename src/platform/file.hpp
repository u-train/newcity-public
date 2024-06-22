#pragma once

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <vector>
#include <string>

bool compareStrings(const char* a, const char* b);

bool fileExists(const char* filename);
bool fileExists(std::string filename);
void makeDirectory(const char* dirName);
void makeDirectoryForFilename(const char* filename);
std::string getParentDirectory(std::string filepath);
bool copyDir(std::string dirPath, std::string destPath, std::experimental::filesystem::copy_options copyRule);
bool copyFile(std::string filePath, std::string destPath);
bool deleteFile(std::string filePath);
bool deleteDir(std::string filePath);
std::string getAbsolutePath(std::string relPath);
bool deleteFilesInDir(std::string dir, bool subDirs);
std::vector<char*> getSubfoldersInDir(std::string dir);
std::vector<std::string> getSubfoldersInDir(std::string dir, bool fullPath);

