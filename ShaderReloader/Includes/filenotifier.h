//============================================================================
// # ShaderReloader #
// Name: filenotifier.h
// Author: Fredrik Linde
// Created on: Apr 28, 2019
// Description: File notifier utility.
//============================================================================

// Based on the following implementations:
// How to create a looping thread : http://www.cplusplus.com/forum/general/188452/
// Writing a simple file watcher : https://solarianprogrammer.com/2019/01/13/cpp-17-filesystem-write-file-watcher-monitor/


#ifndef FILENOTIFIER_H
#define FILENOTIFIER_H

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

#include "enums.h"

#include "topic.h"

class FileNotifier : public Publisher
{
public:

  FileNotifier(std::string baseFolderPath) : _baseFolderPath(baseFolderPath)
  {
    // Store and record files from a base directory with their last modifiction time.
    for (auto &file : std::filesystem::recursive_directory_iterator(baseFolderPath))
    {
      _paths[file.path().string()] = std::filesystem::last_write_time(file);
    }

    // Start thread.
    thread = std::thread(&FileNotifier::execute, this);
  }

  void execute()
  {
    while (running)
    {
      // Halt this thread given a delay in milliseconds;
      using namespace std::literals;
      std::this_thread::sleep_for(200ms);

      // Check for files to be removed.
      auto it = _paths.begin();
      while (it != _paths.end())
      {
        if (!std::filesystem::exists(it->first))
        {
          handle(it->first, FileStatus::ERASED);
          it = _paths.erase(it);
        }
        else
        {
          it++;
        }
      }

      // Check if a file was created or modified.
	  // That includes checking if the user removed the currently tracked folder.
	  if (std::filesystem::exists(_baseFolderPath)) 
	  {
        for (auto &file : std::filesystem::recursive_directory_iterator(_baseFolderPath))
        {
          auto currentLastWriteTime = std::filesystem::last_write_time(file);
	    
          std::string currentPath = file.path().string();
	    
          // Creation.
          if (!contains(currentPath))
          {
            _paths[currentPath] = currentLastWriteTime;
            handle(currentPath, FileStatus::CREATED);
          }
          // Modification.
          else
          {
            if (_paths[currentPath] != currentLastWriteTime)
            {
              _paths[currentPath] = currentLastWriteTime;
              handle(currentPath, FileStatus::MODIFIED);
            }
          }
        }
	  }
	  else 
	  {
		  std::cout << "Base folder was removed. ShaderReloader is disconnected...";
	  }
    }
  }

  bool contains(const std::string &key)
  {
    auto el = _paths.find(key);
    return el != _paths.end();
  }

  void handle(std::string path, FileStatus status)
  {
	  switch (status)
	  {
	  case FileStatus::CREATED:
		  std::cout << "File created: " << path << std::endl;
		  break;
	  case FileStatus::MODIFIED:
		  std::cout << "File modified: " << path << std::endl;
		  notify(&path);
		  break;
	  case FileStatus::ERASED:
		  std::cout << "File erased: " << path << std::endl;
		  break;
	  default:
		  std::cout << "Error! Unknown file status!\n";
	  }
  }

  ~FileNotifier() { running = false; thread.join(); }

private:
  FileNotifier(const FileNotifier&) = delete;
  FileNotifier(FileNotifier&&) = delete;
  FileNotifier& operator= (FileNotifier) = delete;

private: 

  // Function pointer.
  // void(*function)(std::string folder, FileStatus status);

  // Base folder path.
  std::string _baseFolderPath;

  // Time interval to check the base folder for changes.
  // std::chrono::duration<int, std::milli> _delay;
  std::unordered_map<std::string, std::filesystem::file_time_type> _paths;

  std::atomic<bool>running{ true };
  std::thread thread;
};

#endif
