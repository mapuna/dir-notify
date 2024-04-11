/**
 * Copyright 2024 Anupam Gupta <mapuna@gmail.com>
 **/

#include <iostream>
#include <string>
#include <windows.h>

class DirectoryMonitor {
private:
  std::wstring dir_path;
  HANDLE dir_handle;
  bool running;

  static DWORD WINAPI MonitorThread(LPVOID param) {
    DirectoryMonitor *monitor = static_cast<DirectoryMonitor *>(param);
    monitor->Monitor();
    return 0;
  }

  void Monitor() {
    const DWORD buffLen = 1024;
    BYTE buff[buffLen];
    DWORD bytes_returned;

    while (running) {
      BOOL result = ReadDirectoryChangesW(dir_handle, buff, buffLen, FALSE,
                                          FILE_NOTIFY_CHANGE_FILE_NAME,
                                          &bytes_returned, nullptr, nullptr);
      if (result && bytes_returned) {
        FILE_NOTIFY_INFORMATION *notify =
            reinterpret_cast<FILE_NOTIFY_INFORMATION *>(buff);
        do {
          if (notify->Action == FILE_ACTION_ADDED) {
            std::wstring file_name(notify->FileName,
                                   notify->FileNameLength / sizeof(WCHAR));
            std::wcout << L"File created: " << file_name << std::endl;
          }

          if (notify->NextEntryOffset == 0) {
            break;
          }

          notify = reinterpret_cast<FILE_NOTIFY_INFORMATION *>(
              reinterpret_cast<BYTE *>(notify) + notify->NextEntryOffset);
        } while (true);
      }
    }
  }

public:
  DirectoryMonitor(const std::wstring &path) : dir_path(path), running(false) {
    dir_handle = CreateFileW(
        dir_path.c_str(), FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

    if (dir_handle == INVALID_HANDLE_VALUE) {
      std::cerr << "Error: Unable to open directory for monitoring."
                << std::endl;
    }
  }

  ~DirectoryMonitor() {
    if (dir_handle != INVALID_HANDLE_VALUE) {
      CloseHandle(dir_handle);
    }
  }

  void Start() {
    if (dir_handle != INVALID_HANDLE_VALUE) {
      running = true;
      CreateThread(nullptr, 0, MonitorThread, this, 0, nullptr);
    }
  }

  void Stop() { running = false; }
};

int main() {
  std::wstring dir_to_mon = L"C:\\Users\\Anupam\\AppData\\Local\\Temp";
  DirectoryMonitor monitor(dir_to_mon);
  monitor.Start();
  std::cout << "Press enter to stop monitoring...\n";
  std::cin.get();
  monitor.Stop();
  return 0;
}
