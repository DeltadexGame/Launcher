# Launcher v0.1.0
This repo comprises the source code and assets for Team 10's Launcher for the Team Project Module, 3rd Year Computer Science at UCC. 

![Live demo of the launcher in action](https://i.imgur.com/qSXiuFh.gifv)

## Development Phases 

#### 1. Initial Mockups
InVision Studio was used to create the initial mockups of the launcher's numerous screens, including the home, login, signup, 
and latest cards screens. Animations were also used, which I planned to convert to CSS transitions later on. 

#### 2. Choosing A Library 
Initially, I decided to use Rust's `webview` crate, which would allow the application to utilize the user system's webview, 
but high DPI was not supported. 

Electron tends to lead to bloated file size and has performance issues. I wanted a lightweight solution which would promise both speed and high-performance, as well as small size. 

I moved to Ultralight, which allows for the above. It allowed me to create the HTML/CSS/JS frontend, and then manage JS callbacks 
with C++ for native tasks, such as executing the game's JAR file, etc., 

#### 3. Development 
Development began mid-Week 2 once the mockups were finalised and is currently ongoing. 

#### To-Do
The following items will be completed in the coming week(s):
- Start communication with the API for sign-up and login of the users.
- Download images from CDN for the client to make use of them.

## Running/Building

1. To build this application, you'll need to [install the prerequisites required for Ultralight](https://docs.ultralig.ht/docs/installing-prerequisites).

2. Then, execute these commands:

```bash 
git clone https://github.com/silvia-odwyer/game-launcher
cd game-launcher
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

> **Important Note**: If you would like CMake to generate 64-bit applications on Windows, then execute the following: `cmake .. -DCMAKE_GENERATOR_PLATFORM=x64` instead of `cmake ..`

3. Run the Launcher:

**Windows**: Navigate to `game-launcher/build/Release` and locate the `Launcher.exe` executable and run.

**Linux or macOS**: Navigate to `game-launcher/build` and locate the Launcher executable to run. 