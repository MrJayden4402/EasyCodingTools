#pragma once

#include <map>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <string>
#include <windows.h>
#include <set>
#include <thread>
#include <tlhelp32.h>

using namespace std;

// EasyCodingTools::EasyCodingInput

enum INPUT_TYPE
{
    EASY_INPUT_LOCKKEYBOARD,  // 锁定键盘
    EASY_INPUT_UNLOCKKEYBOARD // 不锁定键盘
};

enum EASY_MOUSE_KEY
{
    EASY_MOUSE_LEFT = MOUSE_MOVED,
    EASY_MOUSE_RIGHT = MOUSE_EVENT,
    EASY_MOUSE_MIDDLE = MOUSE_WHEELED,
};

void EasyCodingInputSetup(HWND window, int scheme); // 初始化

void EasyInputAddUnlockKey(int key); // 添加一个没有被锁定的按键

void EasyInputRemoveUnlockKey(int key); // 删除一个没有被锁定的按键

bool EasyKeyDown(int key); // 检测按键是否按下

inline void WaitKeyDown(int key); // 等待按键按下

inline void WaitKeyUp(int key); // 等待按键抬起

// EasyCodingTools::EasyCodingManagers
class __Easy_ManagersInitializer
{
public:
    __Easy_ManagersInitializer();
};

class EasyProcessManager
{
    friend __Easy_ManagersInitializer::__Easy_ManagersInitializer();

protected:
    EasyProcessManager() = default;

public:
    typedef DWORD ProcessID;

    bool Create(string exeName, string cmd = "", string workPath = "");
    void Kill(EasyProcessManager::ProcessID pid);
    void Kill(string exeName);
    void KillPowerly(EasyProcessManager::ProcessID pid);
    void KillPowerly(string exeName);
    EasyProcessManager::ProcessID GetProcessID(string exeName);
    bool Exist(EasyProcessManager::ProcessID pid);
    bool Exist(string exeName);
    void WaitExit(EasyProcessManager::ProcessID pid);
    void WaitExit(string exeName);
};

extern EasyProcessManager *easyProcessManager;

class EasyFileManager
{
    friend __Easy_ManagersInitializer::__Easy_ManagersInitializer();

protected:
    EasyFileManager() = default;

public:
    bool CreateAFile(string path);
    bool CreateFolder(string path);
    bool RemoveAFile(string path);
    bool RemoveFolder(string path);
    bool ExistFile(string path);
    bool ExistFolder(string path);
    bool CopyAFile(string src, string dst);
    bool CopyFolder(string src, string dst);
    bool MoveAFile(string src, string dst);
    bool RenameAFile(string src, string dst);
    long long GetFileSize(string path);
    struct FolderChildList
    {
        vector<string> files;
        vector<string> folders;
    };
    bool ListFolder(string path, FolderChildList &list);
};

extern EasyFileManager *easyFileManager;

class EasyCilpBoardManager
{
    friend __Easy_ManagersInitializer::__Easy_ManagersInitializer();

protected:
    EasyCilpBoardManager() = default;

public:
    string Get();
    bool Set(string text);
    bool Clear();
    bool Empty();
};

extern EasyCilpBoardManager *easyCilpBoardManager;

class EasyMouseManager
{
    friend __Easy_ManagersInitializer::__Easy_ManagersInitializer();

protected:
    EasyMouseManager() = default;

public:
    pair<int, int> GetPosition();
    pair<int, int> GetClientPosition(HWND hwnd);
    bool SetPosition(int x, int y);
    bool SetClientPosition(HWND hwnd, int x, int y);
    bool KeyDown(int button);
    void VirtualKeyDown(int button);
    void VirtualKeyUp(int button);
    void VirtualClick(int button);
    void VirtualMove(int dx, int dy);
};

extern EasyMouseManager *easyMouseManager;

class EasyKeyboardManager
{
    friend __Easy_ManagersInitializer::__Easy_ManagersInitializer();

protected:
    EasyKeyboardManager() = default;

public:
    bool KeyDown(int button);
    void VirtualKeyDown(int button);
    void VirtualKeyUp(int button);
    void VirtualKey(int button);
};

extern EasyKeyboardManager *easyKeyboardManager;

class EasyWindowManager
{
    friend __Easy_ManagersInitializer::__Easy_ManagersInitializer();

protected:
    EasyWindowManager() = default;

public:
    HWND GetForegroundWindow();
    HWND GetConsoleWindow();
    HWND FindWindowByTitle(string title);
    void SetForegroundWindow(HWND hwnd);
    void Minimize(HWND hwnd);
    void Maximize(HWND hwnd);
    void Show(HWND hwnd);
    void Hide(HWND hwnd);
    void TopMost(HWND hwnd);
    void Close(HWND hwnd);
    void SetPosition(HWND hwnd, int x, int y);
    void SetSize(HWND hwnd, int width, int height);
    struct WindowRect
    {
        int x;
        int y;
        int width;
        int height;
        WindowRect();
        WindowRect(int x, int y, int width, int height);
    };
    EasyWindowManager::WindowRect GetWindowRect(HWND hwnd);
    void SetWindowRect(HWND hwnd, EasyWindowManager::WindowRect rect);
    string GetTitle(HWND hwnd);
    void SetTitle(HWND hwnd, string title);
};

extern EasyWindowManager *easyWindowManager;
