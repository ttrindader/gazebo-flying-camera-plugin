# gazebo-flying-camera-plugin

Control the **Gazebo Classic GUI camera** like an FPV drone — fly through the world, **record** camera poses, and **replay** smooth flights.  
This repo ships **System Plugins** (loaded with `-g`) that operate directly on the GUI **UserCamera**.

> **Status**: MVP (no dedicated GUI yet). A simple GUI for record/replay is on the roadmap.

---

## 📦 Components

- **`libFlyingCameraPlugin.so`** — *demo*: moves the camera along a parametric path (autonomous flight) to validate the pipeline.
- **`libFlyingCameraFromPoses.so`** — *replay*: reads a list of poses (XYZ + yaw) and flies the camera along that route with interpolation.
- **`libCameraPoseCollector.so`** — *logger*: captures the current UserCamera poses over time and writes them to a file (to build routes).

> Corresponding sources: `FlyingCameraPlugin.cc`, `FlyingCameraFromPoses.cc`, `CameraPoseCollector.cc`.

---

## 🧩 Requirements

- **Gazebo Classic 11 (GUI)** — tested on Ubuntu 20.04/22.04.  
  > Gazebo Classic has reached EOL, but it’s still widely used. A port to new Gazebo (Garden+) is planned.
- `libgazebo-dev`, `cmake`, `g++` (or `clang`)
- (Optional) ROS Noetic present on the system. *Not required to build these plugins.*

Typical install on Ubuntu:

```bash
sudo apt update
sudo apt install gazebo libgazebo-dev cmake g++
```

---

## ⚙️ Build (plain CMake)

```bash
git clone https://github.com/<YOUR_USER>/gazebo-flying-camera-plugin.git
cd gazebo-flying-camera-plugin
mkdir build && cd build
cmake ..
make -j$(nproc)
```

After build, `build/` will contain `libFlyingCameraPlugin.so`, `libFlyingCameraFromPoses.so`, and `libCameraPoseCollector.so`.

Tell Gazebo where to find the plugins (or copy the `.so` files into `~/.gazebo/plugins`):

```bash
# from gazebo-flying-camera-plugin/build
export GAZEBO_PLUGIN_PATH="$GAZEBO_PLUGIN_PATH:$(pwd)"
```

> Tip: add the line above to your `~/.bashrc`.

### Build via Catkin (optional)
If you prefer to keep it inside a ROS workspace, create a small package and reuse the CMake logic below pointing to these `.cc` files. The code **does not** depend on ROS; catkin is just a wrapper here.

---

## ▶️ Run

All three are **System Plugins** ⇒ you must load them with `-g` when launching Gazebo:

```bash
# 1) Autonomous demo (parametric trajectory)
gazebo --verbose -g libFlyingCameraPlugin.so <your_world.world>

# 2) Replay a route from poses
gazebo --verbose -g libFlyingCameraFromPoses.so <your_world.world>

# 3) Record UserCamera poses to a file
gazebo --verbose -g libCameraPoseCollector.so <your_world.world>
```

> If you see **“incorrect plugin type”**, you probably used `-s` (model/world plugin). These are **System Plugins** → use **`-g`**.

---

## 📁 Route data format

A route is a time‑ordered sequence of **position (x,y,z)** and **yaw**. The recommended minimal format is CSV:

```csv
# t(s), x, y, z, yaw(rad)
0.00,  0.0,  0.0,  2.0,  0.0
0.10,  0.5,  0.0,  2.2,  0.0
0.20,  1.0,  0.2,  2.3,  0.1
...
```

- **`libCameraPoseCollector.so`**: logs the *UserCamera* poses while you move it (mouse/keyboard in Gazebo GUI).  
  *Output*: CSV with columns `t,x,y,z,yaw`.  
- **`libFlyingCameraFromPoses.so`**: reads that CSV and **interpolates** between samples for smooth motion.

> **Where are files saved/loaded?** In this MVP, file paths are defined in code. Open `CameraPoseCollector.cc` and `FlyingCameraFromPoses.cc` to adjust (e.g., `poses.csv`). Soon this will be configurable via env vars and/or a GUI dialog.

---

## 🧪 Suggested flow (record → replay)

1. **Record**  
   ```bash
   gazebo -g libCameraPoseCollector.so <world>
   ```
   Fly manually with the *UserCamera* (right mouse, `Shift` + drag, etc.). Close Gazebo to flush the CSV.

2. **Replay**  
   ```bash
   gazebo -g libFlyingCameraFromPoses.so <world>
   ```
   The plugin loads the CSV and “flies” the same path, smoothly interpolating position and yaw.

---

## 🧰 Quick example

Create a small `poses.csv` next to your world and run the replay:

```csv
0.0, 0, 0, 3, 0
1.0, 5, 0, 4, 0.2
2.0, 5, 5, 4, 1.57
3.0, 0, 5, 3, 3.14
```

```bash
gazebo --verbose -g libFlyingCameraFromPoses.so empty.world
```

You should see the camera “flying” a square.

---

## 🛠️ Reference CMake

You can adapt this minimal CMake to your environment:

```cmake
cmake_minimum_required(VERSION 3.10)
project(gazebo_flying_camera_plugin)

find_package(gazebo REQUIRED)

add_library(FlyingCameraPlugin SHARED FlyingCameraPlugin.cc)
target_link_libraries(FlyingCameraPlugin ${GAZEBO_LIBRARIES})
target_include_directories(FlyingCameraPlugin PRIVATE ${GAZEBO_INCLUDE_DIRS})

add_library(FlyingCameraFromPoses SHARED FlyingCameraFromPoses.cc)
target_link_libraries(FlyingCameraFromPoses ${GAZEBO_LIBRARIES})
target_include_directories(FlyingCameraFromPoses PRIVATE ${GAZEBO_INCLUDE_DIRS})

add_library(CameraPoseCollector SHARED CameraPoseCollector.cc)
target_link_libraries(CameraPoseCollector ${GAZEBO_LIBRARIES})
target_include_directories(CameraPoseCollector PRIVATE ${GAZEBO_INCLUDE_DIRS})
```

> **Important**: each `.cc` uses `GZ_REGISTER_SYSTEM_PLUGIN(...)` — i.e., they are **System Plugins**.

---

## ❓ FAQ / Troubleshooting

- **Plugin loads but nothing happens.**  
  Make sure the Gazebo **GUI** is running. The plugins operate on the GUI’s *UserCamera* (not available in headless mode).

- **“incorrect plugin type” error.**  
  Use `-g libYourPlugin.so` (System Plugin), not `-s` (Model/World Plugin).

- **Gazebo can’t find the `.so`.**  
  Export `GAZEBO_PLUGIN_PATH` to the build folder or move the libraries to `~/.gazebo/plugins`.

---

## 🗺️ Roadmap

- [ ] **GUI** for record & replay (buttons: *Record*, *Stop*, *Save CSV*, *Load & Play*).  
- [ ] Configuration via **env vars** and command‑line args (e.g., `FLY_CAM_INPUT`, `FLY_CAM_OUTPUT`, logging rate).  
- [ ] Ready‑made “orbit”/“dolly” motion presets.  
- [ ] Port to **new Gazebo (Garden+)**.

Contributions are welcome — please open an issue with your use case or ideas.

---

## 📄 License

Maybe: **MIT** (to adjust).

---

## ✍️ Authors

- Tiago Trindade Ribeiro (@ttrindader) — idea, initial implementation.  
- PRs are welcome!
