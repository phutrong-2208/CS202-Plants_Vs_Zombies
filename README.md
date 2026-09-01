## How to Install and Run (Windows - MinGW)

### Prerequisites
- Windows OS
- Visual Studio Code
- MinGW (version 10.0 or newer) — ensure it is added to PATH
- CMake (version 3.10 or newer)

### 1. Download the project
```bash
git clone --recursive https://github.com/phutrong-2208/CS202-Plants_Vs_Zombies
```

### 2. Build the project

Open the project folder in VS Code, then run:

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

### 3. Run the program

If you are already in the `build` directory:
```bash
.\PvZMain.exe
```
otherwise,
```bash
cd build
.\PvZMain.exe
```
