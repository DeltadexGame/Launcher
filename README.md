# Launcher v0.1.0
This repo comprises the source code and assets for the Deltadex Game Launcher.

![Live demo of the launcher in action](https://i.imgur.com/aigy3JJ.gif)

## News Feed

![Live demo of the news feed in action](https://i.imgur.com/lX812lH.gif)

## Specs
Total Size: ~26.4MB 

## Development Phases 

#### 1. Initial Mockups
InVision Studio was used to create the initial mockups of the launcher's numerous screens, including the home, login, signup, 
and latest cards screens. Animations were also used, which I planned to convert to CSS transitions later on. 

#### 2. Choosing A Library 
Initially, I decided to use Rust's `webview` crate, which would allow the application to utilize the user system's webview, 
but high DPI was not supported, so I reconsidered other options that supported high DPI.

Electron tends to lead to a bloated binary file size and has performance issues. I wanted a lightweight solution which would promise both speed and high-performance, as well as small size. 

I moved to Ultralight, which allows for the above. It allowed me to create the HTML/CSS/JS frontend, and then manage JS callbacks 
with C++ for native tasks, such as executing the game's JAR file, etc., 

### Why Ultralight?
- Performant
- Small binary sizes 
- Allowed me to create the frontend with HTML/CSS/JS

#### 3. Development 
Development began mid-Week 2 once the mockups were finalised and is currently ongoing. 

#### To-Do
The following items will be completed in the coming week:
- Download images of cards from the project's CDN into an assets directory, for the game client to make use of them.

## Running/Building

1. To build this application, you'll need to [install the prerequisites required for Ultralight](https://docs.ultralig.ht/docs/installing-prerequisites).

2. Download the Conan package manager. This is required for getting the dependencies required.

3. Then, execute these commands:

```bash 
git clone https://github.com/DeltadexGame/Launcher
cd Launcher
mkdir build
cd build
conan install ..
cmake ..
cmake --build . --config Release
```

OR 

Execute the `build.sh` script after giving execute permissions:

```bash
chmod +x build.sh
./build.sh
```

> **Important Note**: If you would like CMake to generate 64-bit applications on Windows, then execute the following: `cmake .. -DCMAKE_GENERATOR_PLATFORM=x64` instead of `cmake ..`

4. Run the Launcher:

**Windows**: Navigate to `Launcher/build/bin` and locate the `Launcher.exe` executable and run.

**Linux or macOS**: Navigate to `Launcher/build` and locate the Launcher executable to run. 