# EasyCodingTools2.0 代码文档

## 1. 库概述

该库是一个工具库，目标是提供一些简化接口和实用工具。

声明包含在 EasyCodingTools.h 中。\
实现包含在 EasyCodingTools.cpp 中。

实现中包含一些以__Easy_开头的函数，这些函数是库的内部函数，除非你知道你在做什么，否则你不应该直接调用它们。

## 2. 库函数/类

### 2.1 EasyCodingInput 模块

这是一个提供给游戏实现键鼠输入的模块，可以用来锁定键盘，不允许其它程序/接口获得键盘状态。

#### 2.1.2 EasyCodingInputSetup 函数

用于初始化，头部如下：

```cpp
void EasyCodingInputSetup(HWND window, int scheme); 
```

**HWND window** 指定了输入窗口，只有焦点在这个窗口的时候才会响应输入。\
**int scheme** 指定了输入方案 EASY_INPUT_LOCKKEYBOARD 表示锁定键盘，EASY_INPUT_UNLOCKKEYBOARD 表示解锁键盘。

#### 2.1.2 EasyInputAddUnlockKey EasyInputRemoveUnlockKey 函数

用来添加或移除特殊解锁键，头部如下：

```cpp
void EasyInputAddUnlockKey(int key);
void EasyInputRemoveUnlockKey(int key);
```

**int key** 指定了要添加或移除的特殊解锁键的键值。

被加入的特殊解锁键不会被锁定，能被其它程序正常捕获到，可以实现类似解锁Alt+Tab的功能，不至于全部锁定。

#### 2.1.3 EasyKeyDown 函数

用来检测某个键是否被按下，头部如下：

```cpp
bool EasyKeyDown(int key);
```

**int key** 指定了要检测的键的键值。

可以指定EASY_MOUSE_LEFT，EASY_MOUSE_RIGHT，EASY_MOUSE_MIDDLE 来检测鼠标按键，否则视作键盘按键。


#### 2.1.4 WaitKeyDown WaitKeyUp 函数

用来等待某个键被按下或释放，头部如下：

```cpp
inline void WaitKeyDown(int key);
inline void WaitKeyUp(int key);
```

**int key** 指定了要等待的键的键值。


### 2.2 EasyCodingManagers 模块

这个模块提供了一些Manager类，用来操作系统的各类模块。

#### 2.2.1 EasyProcessManager 类

调用接口为easyProcessManager指针。

##### 2.2.1.1 EasyProcessManager::GetProcessID 函数

用来通过进程名获取某个进程的ID，头部如下：

```cpp
EasyProcessManager::ProcessID GetProcessID(string exeName);
```

##### 2.2.1.2 EasyProcessManager::Create 函数

用于创建一个进程，头部如下：

```cpp
bool Create(string exeName, string cmd = "", string workPath = "");
```

**string exeName** 指定了要创建的进程的exe文件名\
**string cmd** 指定了要创建的进程的命令行参数\
**string workPath** 指定了要创建的进程的工作目录。

会返回创建是否成功。

##### 2.2.1.3 EasyProcessManager::Kill 函数

用于结束一个进程，头部如下：

```cpp
bool Kill(EasyProcessManager::ProcessID pid);
bool Kill(string exeName);
```

传入pid或进程名，会返回结束是否成功。

##### 2.2.1.4 EasyProcessManager::KillPowerly 函数

用于强制结束一个进程，头部如下：

```cpp
bool KillPowerly(EasyProcessManager::ProcessID pid);
bool KillPowerly(string exeName);
```

传入pid或进程名，会返回结束是否成功。

注意，此函数会强制结束进程，比较危险，请谨慎使用。

##### 2.2.1.5 EasyProcessManager::Exist 函数

用于判断一个进程是否存在，头部如下：

```cpp
bool Exist(EasyProcessManager::ProcessID pid);
bool Exist(string exeName);
```

传入pid或进程名，会返回进程是否存在。

##### 2.2.1.6 EasyProcessManager::WaitExit 函数

用于等待一个进程退出，头部如下：

```cpp
void WaitExit(EasyProcessManager::ProcessID pid);
void WaitExit(string exeName);
```

传入pid或进程名，会阻塞当前线程，直到进程退出。

#### 2.2.2 EasyFileManager 类

EasyFileManager类用于管理文件，包括创建、删除、复制、移动等操作。

调用接口是easyFileManager指针。

##### 2.2.2.1 EasyFileManager::CreateAFile 函数


用于创建一个文件，头部如下：

```cpp
bool CreateAFile(string path);
```

传入文件路径，会返回创建是否成功。

若文件已存在，会返回true，并不进行任何操作。

##### 2.2.2.2 EasyFileManager::CreateFolder 函数

用于创建一个文件夹，头部如下：

```cpp
bool CreateFolder(string path);
```

传入文件夹路径，会返回创建是否成功。

若文件夹已存在，会返回true，并不进行任何操作。

##### 2.2.2.3 EasyFileManager::RemoveAFile 函数

用于删除一个文件，头部如下：

```cpp
bool RemoveAFile(string path);
```

传入文件路径，会返回删除是否成功。

##### 2.2.2.4 EasyFileManager::RemoveFolder 函数

用于删除一个文件夹及其内部所有子目录，头部如下：

```cpp
bool RemoveFolder(string path);
```

传入文件夹路径，会返回删除是否成功。

请务必传入绝对路径。

##### ExistFile 函数

用于判断一个文件是否存在，头部如下：

```cpp
bool ExistFile(string path);
```

传入文件路径，会返回文件是否存在。

##### 2.2.2.6 ExistFolder 函数

用于判断一个文件夹是否存在，头部如下：

```cpp
bool ExistFolder(string path);
```

传入文件夹路径，会返回文件夹是否存在。

##### 2.2.2.7 EasyFileManager::CopyAFile 函数

用于复制一个文件，头部如下：

```cpp
bool CopyAFile(string src, string dst);
```

传入源文件路径和目标文件路径，会返回复制是否成功。

##### 2.2.2.8 EasyFileManager::CopyFolder 函数

用于复制一个文件夹及其内部所有子目录，头部如下：

```cpp
bool CopyFolder(string src, string dst);
```

传入源文件夹路径和目标文件夹路径，会返回复制是否成功。

注意，该函数是把src文件夹复制到dst的子目录下，即dst/src。

请务必传入绝对路径。

##### 2.2.2.9 EasyFileManager::MoveAFile 函数

用于移动一个文件，头部如下：

```cpp
bool MoveAFile(string src, string dst);
```

传入源文件路径和目标文件路径，会返回移动是否成功。

注意这个函数也能处理文件夹。

##### 2.2.2.10 EasyFileManager::RenameAFile 函数

用于重命名一个文件，头部如下：

```cpp
bool RenameAFile(string src, string dst);
```

传入源文件路径和目标文件名，会返回重命名是否成功。

注意文件名不包含路径。


##### 2.2.2.11 EasyFileManager::GetFileSize 函数

用于获取一个文件的大小，头部如下：

```cpp
long long GetFileSize(string path);
```

##### 2.2.2.12 EasyFileManager::ListFolder 函数

用于列出文件夹中的所有文件和子文件夹，头部如下：

```cpp
bool ListFolder(string path, EasyFileManager::FolderChildList &list);
```

此处的FolderChildList是一个结构体，有两个成员：

```cpp
vector<string> files;
vector<string> folders;
```

分别是文件列表和文件夹列表。


#### 2.2.3 EasyCilpboardManager 类

用于操作文本剪贴板，头部如下：

调用接口为easyClipboardManager指针。

##### 2.2.3.1 EasyCilpboardManager::Get 函数

用于获取剪贴板中的内容，头部如下：

```cpp
string Get();
```

##### 2.2.3.2 EasyCilpboardManager::Set 函数

用于设置剪贴板中的内容，头部如下：

```cpp
void Set(string content);
```

##### 2.2.3.3 EasyCilpboardManager::Clear 函数

用于清空剪贴板中的内容，头部如下：

```cpp
void Clear();
```

##### 2.2.3.4 EasyCilpboardManager::Empty 函数

用于判断剪贴板是否为空，头部如下：

```cpp
bool Empty();
```


#### 2.2.4 EasyMouseManager 类

用于操作鼠标，头部如下：

调用接口为easyMouseManager指针。

##### 2.2.4.1 EasyMouseManager::GetPosition 函数

用于获取鼠标的当前位置，头部如下：

```cpp
pair<int, int> GetPosition();
```

##### 2.2.4.2 EasyMouseManager::GetClientPosition 函数

用于获取鼠标在窗口中的位置，头部如下：

```cpp
pair<int, int> GetClientPosition();
```

##### 2.2.4.3 EasyMouseManager::SetPosition 函数

用于设置鼠标的位置，头部如下：

```cpp
void SetPosition(int x, int y);
```

##### 2.2.4.4 EasyMouseManager::SetClientPosition 函数

用于设置鼠标在窗口中的位置，头部如下：

```cpp
void SetClientPosition(int x, int y);
```

##### 2.2.4.5 EasyMouseManager::KeyDown 函数

用于判断鼠标按键是否按下，头部如下：

```cpp
bool KeyDown(int key);
```

可以有以下参数：

- EASY_MOUSE_LEFT：左键
- EASY_MOUSE_RIGHT：右键
- EASY_MOUSE_MIDDLE：中键

##### 2.2.4.6 EasyMouseManager::VirtualKeyDown VirtualKeyDown VirtualClick 函数

用于模拟鼠标按键按下、释放和点击，头部如下：

```cpp
void VirtualKeyDown(int key);
void VirtualKeyUp(int key);
void VirtualClick(int key);
```

可以有以下参数：

- EASY_MOUSE_LEFT：左键
- EASY_MOUSE_RIGHT：右键
- EASY_MOUSE_MIDDLE：中键

##### 2.2.4.7 EasyMouseManager::VirtualMove 函数

用于模拟相对移动，头部如下：

```cpp
void VirtualMove(int x, int y);
```

#### 2.2.5 EasyKeyboardManager 类

用于操作键盘。

调用接口为easyKeyboardManager。

##### 2.2.5.1 EasyKeyboardManager::KeyDown 函数

用于判断键盘按键是否按下，头部如下：

```cpp
bool KeyDown(int key);
```

传入参数为按键的虚拟键码。

##### 2.2.5.2 EasyKeyboardManager::VirtualKeyDown VirtualKeyUp VirtualKey 函数

用于模拟键盘按键按下、释放和点击，头部如下：

```cpp
void VirtualKeyDown(int key);
void VirtualKeyUp(int key);
void VirtualKey(int key);
```

传入参数为按键的虚拟键码。


#### 2.2.6 EasyWindowManager 类

用于操作窗口。

调用接口为easyWindowManager。

##### 2.2.6.1 EasyWindowManager::GetForegroundWindow 函数

用于获取当前活动窗口，头部如下：

```cpp
HWND GetForegroundWindow();
```

##### 2.2.6.2 EasyWindowManager::GetConsoleWindow 函数

用于获取控制台窗口，头部如下：

```cpp
HWND GetConsoleWindow();
```

##### 2.2.6.3 EasyWindowManager::SetForegroundWindow 函数

用于设置当前活动窗口，头部如下：

```cpp
void SetForegroundWindow(HWND hwnd);
```

##### 2.2.6.4 EasyWindowManager::Minimize 函数

用于最小化窗口，头部如下：

```cpp
void Minimize(HWND hwnd);
```

##### 2.2.6.5 EasyWindowManager::Maximize 函数

用于最大化窗口，头部如下：

```cpp
void Maximize(HWND hwnd);
```

##### 2.2.6.6 EasyWindowManager::Restore 函数

用于还原窗口，头部如下：

```cpp
void Restore(HWND hwnd);
```

##### 2.2.6.7 EasyWindowManager::Hide 函数

用于隐藏窗口，头部如下：

```cpp
void Hide(HWND hwnd);
```

##### 2.2.6.8 EasyWindowManager::Show 函数

用于显示窗口，头部如下：

```cpp
void Show(HWND hwnd);
```

##### 2.2.6.9 EasyWindowManager::TopMost 函数

用于置顶窗口，头部如下：

```cpp
void TopMost(HWND hwnd);
```

这个函数会一直置顶窗口。

##### 2.2.6.10 EasyWindowManager::Close 函数

用于关闭窗口，头部如下：

```cpp
void Close(HWND hwnd);
```

##### 2.2.6.11 EasyWindowManager::SetPosition 函数

用于设定窗口位置，头部如下：

```cpp
void SetPosition(HWND hwnd, int x, int y);
```

##### 2.2.6.12 EasyWindowManager::SetSize 函数

用于设定窗口大小，头部如下：

```cpp
void SetSize(HWND hwnd, int width, int height);
```

##### 2.2.6.13 EasyWindowManager::GetWindowRect 函数

用于获取窗口相对屏幕的矩形，头部如下：
```cpp
EasyWindowManager::WindowRect GetWindowRect(HWND hwnd);
```
其中，WindowRect 是一个结构体，有如下成员。
```cpp
int x;
int y;
int width;
int height;
```

##### 2.2.6.14 EasyWindowManager::SetWindowRect 函数

用于设置窗口相对屏幕的矩形，头部如下：

```cpp
void SetWindowRect(HWND hwnd,EasyWindowManager::WindowRect rect);
```

##### 2.2.6.15 EasyWindowManager::GetTitle 函数

用于获取窗口标题，头部如下：

```cpp
string GetTitle(HWND hwnd);
```

##### 2.2.6.16 EasyWindowManager::SetTitle 函数

用于设置窗口标题，头部如下：

```cpp
void SetTitle(HWND hwnd, string title);
```

## 3.历史

1.0 - 2024.12.4\
实现了PeekBoxSet模块以及EasyCodingInput模块，但是PeekBoxSet模块由于实用性很低，在2.0中移除。

By MrJayden.
