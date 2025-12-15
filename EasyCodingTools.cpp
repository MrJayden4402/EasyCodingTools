#include "EasyCodingTools.h"

// EasyCodingInput

namespace EasyCodingInput
{

    std::set<int> easyUnLockKey;

    int easyInputType;   // 特殊的,没有被锁定的按键
    HHOOK easyInputHook; // 锁定键盘还是不锁定键盘

    shared_mutex easyisKeyDownMutex;
    bool easyisKeyDown[500]; // 保存按键状态

    HWND easyInputWindow;

};
// 键盘事件处理函数
LRESULT CALLBACK __Easy_KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    using namespace EasyCodingInput;
    if (GetForegroundWindow() != easyInputWindow)
        return CallNextHookEx(easyInputHook, nCode, wParam, lParam);
    int KeyCode = -1;
    if (nCode == HC_ACTION)
    {
        // 只有当键盘事件有效时才处理
        KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT *)lParam;
        KeyCode = pKeyboard->vkCode;
        // 判断按键事件
        unique_lock<shared_mutex> lock(easyisKeyDownMutex);
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
            easyisKeyDown[KeyCode] = true;
        if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
            easyisKeyDown[KeyCode] = false;
    }

    // 继续传递消息
    if (easyInputType == EASY_INPUT_UNLOCKKEYBOARD || easyUnLockKey.find(KeyCode) != easyUnLockKey.end()) // 解锁键盘
        return CallNextHookEx(easyInputHook, nCode, wParam, lParam);
    return 1; // 返回1表示拦截消息
}

void EasyCodingInputSetup(HWND window, int scheme) // 初始化
{
    using namespace EasyCodingInput;
    easyInputType = scheme;
    easyInputHook = SetWindowsHookEx(WH_KEYBOARD_LL, __Easy_KeyboardProc, NULL, 0);
    easyInputWindow = window;
}

void EasyInputAddUnlockKey(int key) { EasyCodingInput::easyUnLockKey.insert(key); }
void EasyInputRemoveUnlockKey(int key) { EasyCodingInput::easyUnLockKey.erase(key); }

bool EasyKeyDown(int key)
{
    using namespace EasyCodingInput;
    if (GetForegroundWindow() != easyInputWindow)
        return false;
    switch (key)
    {
    case EASY_MOUSE_LEFT:
    case EASY_MOUSE_RIGHT:
    case EASY_MOUSE_MIDDLE: // 鼠标按键
        return ((GetAsyncKeyState(key) & 0x8000) ? 1 : 0);
    default: // 键盘按键
    {
        shared_lock lock(easyisKeyDownMutex);
        return easyisKeyDown[key];
    }
    }
}
inline void WaitKeyDown(int key)
{
    while (!EasyKeyDown(key))
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
}
inline void WaitKeyUp(int key)
{
    while (EasyKeyDown(key))
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
}

__Easy_ManagersInitializer __easyManagersInitializer;

EasyProcessManager *easyProcessManager;
EasyFileManager *easyFileManager;
EasyCilpboardManager *easyCilpboardManager;
EasyMouseManager *easyMouseManager;
EasyKeyboardManager *easyKeyboardManager;
EasyWindowManager *easyWindowManager;

__Easy_ManagersInitializer::__Easy_ManagersInitializer()
{
    static bool initialized = false;
    if (initialized)
        return;

    easyProcessManager = new EasyProcessManager();
    easyFileManager = new EasyFileManager();
    EasyCilpboardManager = new EasyCilpboardManager();
    easyMouseManager = new EasyMouseManager();
    easyKeyboardManager = new EasyKeyboardManager();
    easyWindowManager = new EasyWindowManager();
}

bool EasyProcessManager::Create(string exeName, string cmd, string workPath)
{
    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    ZeroMemory(&processInfo, sizeof(processInfo));
    startupInfo.cb = sizeof(startupInfo);

    LPSTR cmdLine = cmd.empty() ? NULL : cmd.data();

    LPSTR workDir = workPath.empty() ? NULL : workPath.data();

    WINBOOL ret = CreateProcessA(exeName.c_str(),
                                 cmdLine,
                                 NULL,
                                 NULL,
                                 FALSE,
                                 0,
                                 NULL,
                                 workDir,
                                 &startupInfo,
                                 &processInfo);

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    return ret;
}

void EasyProcessManager::Kill(EasyProcessManager::ProcessID pid)
{
    HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    TerminateProcess(handle, 0);
    CloseHandle(handle);
}

void EasyProcessManager::Kill(string exeName)
{
    Kill(GetProcessID(exeName));
}

void EasyProcessManager::KillPowerly(EasyProcessManager::ProcessID pid)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return;
    THREADENTRY32 te = {sizeof(te)};
    bool ret = Thread32First(hSnapshot, &te);
    while (ret)
    {
        if (te.th32OwnerProcessID == pid)
        {
            HANDLE hThread = OpenThread(THREAD_TERMINATE, FALSE, te.th32ThreadID);
            if (hThread != NULL)
            {
                TerminateThread(hThread, 0);
                CloseHandle(hThread);
            }
        }
        ret = Thread32Next(hSnapshot, &te);
    }
    CloseHandle(hSnapshot);
}

void EasyProcessManager::KillPowerly(string exeName)
{
    KillPowerly(GetProcessID(exeName));
}

EasyProcessManager::ProcessID EasyProcessManager::GetProcessID(string exeName)
{
    DWORD id = 0;
    PROCESSENTRY32 pe; // 进程信息
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // 获取系统进程列表
    if (Process32First(hSnapshot, &pe))
    {
        do
        {
            if (0 == _stricmp(pe.szExeFile, exeName.c_str()))
            {
                // 不区分大小写比较
                id = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return id;
}

bool EasyProcessManager::Exist(EasyProcessManager::ProcessID pid)
{
    // 尝试以最小权限打开进程
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

    if (!hProcess)
    {
        DWORD err = GetLastError();
        if (err == ERROR_INVALID_PARAMETER) // 进程不存在
            return false;
        // 其他错误（如权限不足）
        return true;
    }

    DWORD exitCode = 0;
    GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);

    // STILL_ACTIVE = 259 -> 进程正在运行
    return exitCode == STILL_ACTIVE;
}

bool EasyProcessManager::Exist(string exeName)
{
    return GetProcessID(exeName) != 0;
}

void EasyProcessManager::WaitExit(EasyProcessManager::ProcessID pid)
{
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (!hProcess)
        return;
    WaitForSingleObject(hProcess, INFINITE);
    CloseHandle(hProcess);
}

void EasyProcessManager::WaitExit(string exeName)
{
    WaitExit(GetProcessID(exeName));
}

bool EasyFileManager::CreateAFile(string path)
{
    if (path.empty())
        return false;

    HANDLE hFile = CreateFileA(
        path.c_str(),
        GENERIC_WRITE,   // 允许写入（创建文件需要）
        FILE_SHARE_READ, // 允许其他进程读
        NULL,
        OPEN_ALWAYS, // 文件不存在则创建，存在则打开
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    CloseHandle(hFile);
    return true;
}
bool EasyFileManager::CreateFolder(string path)
{
    if (CreateDirectoryA(path.c_str(), NULL))
        return true;
    else if (GetLastError() == ERROR_ALREADY_EXISTS)
        return true;
    else
        return false;
}
bool EasyFileManager::RemoveAFile(string path)
{
    return DeleteFileA(path.c_str()) != 0;
}
bool EasyFileManager::RemoveFolder(string path)
{
    if (path.empty())
        return false;
    for (auto &i : path)
        if (i == '/')
            i = '\\';
    if (!ExistFolder(path))
        return false;
    if (path.back() == '\\')
        path.pop_back();

    EasyFileManager::FolderChildList list;
    if (!this->ListFolder(path, list))
        return false;

    for (auto &i : list.files)
        if (!this->RemoveAFile(i))
            return false;

    for (auto &i : list.folders)
        if (!this->RemoveFolder(i))
            return false;

    if (RemoveDirectoryA(path.c_str()) == 0)
        return false;

    return true;
}
bool EasyFileManager::ExistFile(string path)
{
    return GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}
bool EasyFileManager::ExistFolder(string path)
{
    DWORD ret = GetFileAttributesA(path.c_str());
    return (ret != INVALID_FILE_ATTRIBUTES) &&
           (ret & FILE_ATTRIBUTE_DIRECTORY);
}
bool EasyFileManager::CopyAFile(string src, string dst)
{
    if (src.empty() || dst.empty())
        return false;

    return CopyFileA(src.c_str(), dst.c_str(), FALSE) != 0;
}
bool EasyFileManager::CopyFolder(string src, string dst)
{
    if (src.empty() || dst.empty())
        return false;
    for (auto &i : src)
        if (i == '/')
            i = '\\';
    for (auto &i : dst)
        if (i == '/')
            i = '\\';
    if (!ExistFolder(src) || !ExistFolder(dst))
        return false;
    if (src.back() == '\\')
        src.pop_back();
    if (dst.back() == '\\')
        dst.pop_back();
    size_t pos = src.find_last_of('\\');
    if (pos == string::npos)
        return false;
    dst += src.substr(pos);
    if (!this->CreateFolder(dst))
        return false;
    EasyFileManager::FolderChildList list;
    if (!this->ListFolder(src, list))
        return false;

    for (auto &i : list.files)
    {
        string filename = dst + i.substr(src.length());
        if (!this->CopyAFile(i, filename))
            return false;
    }

    for (auto &i : list.folders)
        if (!this->CopyFolder(i, dst))
            return false;

    return true;
}
bool EasyFileManager::MoveAFile(string src, string dst)
{
    if (src.empty() || dst.empty())
        return false;

    return MoveFileExA(
               src.c_str(),
               dst.c_str(),
               MOVEFILE_REPLACE_EXISTING // 允许覆盖
               ) != 0;
}
bool EasyFileManager::RenameAFile(string src, string dst)
{
    dst = src.substr(0, src.find_last_of('\\') + 1) + dst;
    return this->MoveAFile(src, dst);
}
long long EasyFileManager::GetFileSize(string path)
{
    if (path.empty())
        return -1;

    WIN32_FILE_ATTRIBUTE_DATA data;
    if (!GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &data))
        return -1;

    // 文件大小是 64 位的（高位 + 低位）
    ULONGLONG size =
        (static_cast<ULONGLONG>(data.nFileSizeHigh) << 32) |
        data.nFileSizeLow;

    return static_cast<long long>(size);
}
bool EasyFileManager::ListFolder(string path, EasyFileManager::FolderChildList &list)
{
    if (path.empty())
        return false;

    const string pattern = path + "\\*";

    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(pattern.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE)
        return false;
    do
    {
        const string file_name = string(fd.cFileName);
        if (file_name[0] == '.')
            continue;
        const string full_path = path + "\\" + file_name;
        DWORD file_attributes = GetFileAttributesA(full_path.c_str());
        if (file_attributes != INVALID_FILE_ATTRIBUTES)
        {
            if (file_attributes & FILE_ATTRIBUTE_DIRECTORY) // 是子文件夹
                list.folders.push_back(full_path);
            else // 是普通文件
                list.files.push_back(full_path);
        }
    } while (FindNextFileA(hFind, &fd) != 0);
    FindClose(hFind);
    return true;
}

string EasyCilpboardManager::Get()
{
    if (!OpenClipboard(nullptr))
        return "";

    HANDLE hData = GetClipboardData(CF_TEXT);

    if (hData == NULL)
    {
        CloseClipboard();
        return "";
    }

    char *text = static_cast<char *>(GlobalLock(hData));

    if (text == NULL)
    {
        CloseClipboard();
        return "";
    }

    string result(text);
    GlobalUnlock(hData);
    CloseClipboard();

    return result;
}
bool EasyCilpboardManager::Set(string text)
{
    if (!OpenClipboard(nullptr))
        return false;
    EmptyClipboard();
    HGLOBAL hClipboardData = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    char *pchData = static_cast<char *>(GlobalLock(hClipboardData));
    if (pchData == nullptr)
    {
        GlobalFree(hClipboardData);
        CloseClipboard();
    }
    strcpy(pchData, text.c_str());
    GlobalUnlock(hClipboardData);
    if (!SetClipboardData(CF_TEXT, hClipboardData))
    {
        GlobalFree(hClipboardData);
        CloseClipboard();
        return false;
    }
    CloseClipboard();
    return true;
}
bool EasyCilpboardManager::Clear()
{
    if (!OpenClipboard(nullptr))
        return false;
    EmptyClipboard();
    CloseClipboard();
    return true;
}
bool EasyCilpboardManager::Empty()
{
    return this->Get().empty();
}

pair<int, int> EasyMouseManager::GetPosition()
{
    POINT pt;
    if (GetCursorPos(&pt))
        return {pt.x, pt.y};
    else
        return {-1, -1};
}
pair<int, int> EasyMouseManager::GetClientPosition(HWND hwnd)
{
    POINT pt;
    if (GetCursorPos(&pt))
    {
        ScreenToClient(hwnd, &pt);
        return {pt.x, pt.y};
    }
    else
        return {-1, -1};
}
bool EasyMouseManager::SetPosition(int x, int y)
{
    return SetCursorPos(x, y);
}
bool EasyMouseManager::SetClientPosition(HWND hwnd, int x, int y)
{
    POINT pt = {x, y};
    ClientToScreen(hwnd, &pt);
    return SetCursorPos(pt.x, pt.y);
}
bool EasyMouseManager::KeyDown(int button)
{
    return ((GetAsyncKeyState(button) & 0x8000) ? 1 : 0);
}
void EasyMouseManager::VirtualKeyDown(int button)
{
    INPUT input = {0};
    input.type = INPUT_MOUSE;

    switch (button)
    {
    case EASY_MOUSE_LEFT:
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        break;
    case EASY_MOUSE_RIGHT:
        input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        break;
    case EASY_MOUSE_MIDDLE:
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
        break;
    default:
        return;
    }

    SendInput(1, &input, sizeof(INPUT));
}
void EasyMouseManager::VirtualKeyUp(int button)
{
    INPUT input = {0};
    input.type = INPUT_MOUSE;

    switch (button)
    {
    case EASY_MOUSE_LEFT:
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        break;
    case EASY_MOUSE_RIGHT:
        input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        break;
    case EASY_MOUSE_MIDDLE:
        input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
        break;
    default:
        return;
    }

    SendInput(1, &input, sizeof(INPUT));
}
void EasyMouseManager::VirtualClick(int button)
{
    VirtualKeyDown(button);
    VirtualKeyUp(button);
}
void EasyMouseManager::VirtualMove(int dx, int dy)
{
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.dx = dx;
    input.mi.dy = dy;

    SendInput(1, &input, sizeof(INPUT));
}

bool EasyKeyboardManager::KeyDown(int button)
{
    return ((GetAsyncKeyState(button) & 0x8000) ? 1 : 0);
}
void EasyKeyboardManager::VirtualKeyDown(int button)
{
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(button); // 虚拟键码，例如 VK_A、VK_SHIFT
    input.ki.dwFlags = 0;                     // 0 = 按下

    SendInput(1, &input, sizeof(INPUT));
}
void EasyKeyboardManager::VirtualKeyUp(int button)
{
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(button);
    input.ki.dwFlags = KEYEVENTF_KEYUP; // 松开

    SendInput(1, &input, sizeof(INPUT));
}
void EasyKeyboardManager::VirtualKey(int button)
{
    VirtualKeyDown(button);
    VirtualKeyUp(button);
}

EasyWindowManager::WindowRect::WindowRect()
{
    this->x = 0;
    this->y = 0;
    this->width = 0;
    this->height = 0;
}

EasyWindowManager::WindowRect::WindowRect(int x, int y, int width, int height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

HWND EasyWindowManager::FindWindowByTitle(string title)
{
    return FindWindow(NULL, title.c_str());
}
HWND EasyWindowManager::GetForegroundWindow()
{
    return ::GetForegroundWindow();
}
HWND EasyWindowManager::GetConsoleWindow()
{
    return ::GetConsoleWindow();
}
void EasyWindowManager::SetForegroundWindow(HWND hwnd)
{
    SetForegroundWindow(hwnd);
}
void EasyWindowManager::Minimize(HWND hwnd)
{
    ShowWindow(hwnd, SW_MINIMIZE);
}
void EasyWindowManager::Maximize(HWND hwnd)
{
    ShowWindow(hwnd, SW_MAXIMIZE);
}
void EasyWindowManager::Show(HWND hwnd)
{
    ShowWindow(hwnd, SW_SHOW);
}
void EasyWindowManager::Hide(HWND hwnd)
{
    ShowWindow(hwnd, SW_HIDE);
}
void EasyWindowManager::TopMost(HWND hwnd)
{
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
void EasyWindowManager::Close(HWND hwnd)
{
    SendMessage(hwnd, WM_CLOSE, 0, 0);
}
void EasyWindowManager::SetPosition(HWND hwnd, int x, int y)
{
    SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
void EasyWindowManager::SetSize(HWND hwnd, int width, int height)
{
    SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}
EasyWindowManager::WindowRect EasyWindowManager::GetWindowRect(HWND hwnd)
{
    RECT rect;
    ::GetWindowRect(hwnd, &rect);
    return WindowRect{rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top};
}
void EasyWindowManager::SetWindowRect(HWND hwnd, EasyWindowManager::WindowRect rect)
{
    ::SetWindowPos(hwnd, NULL, rect.x, rect.y, rect.width, rect.height, SWP_NOZORDER);
}
string EasyWindowManager::GetTitle(HWND hwnd)
{
    int len = GetWindowTextLength(hwnd) + 1;
    char *buf = new char[len];
    GetWindowText(hwnd, buf, len);
    string title = buf;
    delete[] buf;
    return title;
}
void EasyWindowManager::SetTitle(HWND hwnd, string title)
{
    SetWindowText(hwnd, title.c_str());
}