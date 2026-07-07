# 🚰 Tank Supervisory System (SupCliente)

This project is a SCADA (Supervisory Control and Data Acquisition) client application developed in **C++**. It allows the remote monitoring and control of a tank plant via a TCP/IP connection with a server.

The project features a robust architecture, separating business and network communication logic from its user interfaces, offering two versions: a **Terminal (Console)** interface and a **Graphical User Interface (GUI)** built with the **Qt** framework.

## 🚀 Features

* **Two Access Levels:**
  * *Viewer:* Can only observe real-time data such as tank levels, pump flow, and valve states.
  * *Administrator:* Has full permission to act on the plant by opening/closing valves and adjusting pump power.
* **Real-Time Monitoring:** Periodically reads server sensors including H1 and H2 levels in cm and %, pump flow, and overflow alerts.
* **Visual and Graphical Representation (Qt Version):** Features panels with control buttons, virtual LCD displays, and the option to view data on a temporal graph.
* **Cross-Platform Communication:** Uses a customized `MySocket` library built over TCP sockets, which can be easily compiled on both Windows and Linux.

## 📁 Project Structure

The source code is divided into logical modules:

* **Core Logic:**
  * `supcliente.h` / `.cpp`: Manages the main connection thread, communication (GET, SET, LOGIN, LOGOUT commands), and stores the last state of the plant.
  * `supdados.h` / `.cpp`: Data structures (such as `SupState`) that model the physical state of the plant.
* **Graphical User Interface (Qt):**
  * `supcliente_qt.h` / `.cpp` / `.ui`: Rich graphical interface with checkable buttons, sliders, LCDs (`QLCDNumber`), and graphs.
  * `supcliente_main_qt.cpp`: Entry point for the graphical application.
  * `SupCliente.pro`: Qt Creator project file.
* **Terminal Interface (Console):**
  * `supcliente_term.h` / `.cpp`: Text-based command-line interface.
  * `supcliente_main_term.cpp`: Entry point for the console application.
  * `SupCliente.cbp`: Code::Blocks project file.
* **Networking (MySocket):**
  * `mysocket.h` / `.cpp`: An object-oriented library to manage connections via Sockets supporting blocking stream sockets and queues/selects with timeout.

## 🛠️ Technologies Used

* **C++17:** Base language standard.
* **Threads and Mutexes:** Used for periodic asynchronous data requests without freezing the interface.
* **Qt Framework (version 5/6):** Powering the rich and reactive frontend with Signals and Slots.
* **Winsock2 (`Ws2_32`):** Native network API when compiled under Windows environments.

## ⚙️ How to Compile and Run

The repository contains ready-to-use configurations for different IDEs depending on the desired interface.

### Option 1: Graphical User Interface Version (Qt Creator)
Recommended for a full user experience.
1. Open the `SupCliente.pro` file in **Qt Creator**.
2. Configure your Kit (MinGW or MSVC on Windows, GCC on Linux).
3. The `.pro` file is already configured to automatically add network dependencies (`LIBS += -lWs2_32` for Windows).
4. Press `Ctrl + R` (or click the Play button) to compile and run.

### Option 2: Terminal Version (Code::Blocks)
Recommended for testing, environments without a graphical server, or educational purposes.
1. Open the `SupCliente.cbp` file in **Code::Blocks**.
2. Go to `Settings > Compiler` and ensure the `-std=c++17` option is enabled.
3. In the `Linker settings`, the project is already configured to include the static library `Ws2_32`.
4. Press `F9` (Build & Run) to start.

## 🔧 Configuring MySocket (Cross-Platform)
The `MySocket` library (`mysocket.cpp` and `mysocket.h`) supports both Windows and Linux.
By default, the code uses the Windows implementation (`#include <winsock2.h>`).
If you are compiling on Linux, open `mysocket.h` and `mysocket.cpp` and follow the instructions at the beginning of each file: simply comment out the Windows block and uncomment the Linux block which switches to `<sys/socket.h>`.

## 💻 Basic Usage

1. Upon launching the application, enter the server's target `IP`.
2. Enter your credentials (`Login` and `Senha`).
   * *Note: The privilege level (admin/viewer) depends on the credentials stored on the server.*
3. After connecting, observe the parameters changing in real-time.
4. If you are an *Admin*, try dragging the pump flow slider (`Pump`) and enabling/disabling the valves (`Valve 1` and `Valve 2`) to see the plant react instantly.
