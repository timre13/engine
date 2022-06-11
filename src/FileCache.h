#pragma once

#include <string>
#include <map>
#include <memory>
#include <cassert>
#include "Logger.h"

/*
 * Prevents files from being opened multiple times.
 *
 * Template params:
 *      T: The container that contains a file object
 *      rootFolder: The folder that is used to load the files
 *      placeholderFilename: If given, this file is opened in the constructor
 *                       and it is returned when a requested file can't be opened.
 *                       If not given, an error in `open()` will be fatal.
 */
template <typename T, const auto& rootFolder, const std::string_view& placeholderFilename>
class FileCache
{
private:
    using sharedPtr_t = std::shared_ptr<T>;
    //       Filename     File object
    std::map<std::string, sharedPtr_t> m_files;

public:
    FileCache()
    {
        if constexpr (placeholderFilename != "")
        {
            auto placeholderFile = std::make_shared<T>();
            if (placeholderFile->open(std::string(rootFolder)+"/"+std::string(placeholderFilename))) // Try to open placeholder file
            {
                Logger::err << "Failed to open placeholder file" << Logger::End;
                abort();
            }
            m_files.emplace("", placeholderFile);
        }
    }

    sharedPtr_t open(const std::string& filename)
    {
        sharedPtr_t file;

        auto it = m_files.find(filename);
        if (it == m_files.end()) // If the file is not in the cache
        {
            Logger::log << "File \"" << filename << "\" is NOT in the cache, loading (folder: \"" << rootFolder << "\")" << Logger::End;
            file = std::make_shared<T>();
            if (file->open(std::string(rootFolder)+"/"+std::string(filename))) // Try to open file
            {
                if constexpr (placeholderFilename != "") // If we have placeholder file, use it
                {
                    file = m_files.find("")->second;
                    Logger::warn << "Using placeholder file" << Logger::End;
                }
                else // No placeholder texture, so failing to open a file is fatal
                {
                    abort();
                }
            }
            else
            {
                m_files.emplace(filename, file);
            }

        }
        else // If the file is in the cache
        {
            Logger::verb << "File \"" << filename << "\" is in the cache" << Logger::End;
            file = it->second;
        }

        assert(file);
        return file;
    }
};
