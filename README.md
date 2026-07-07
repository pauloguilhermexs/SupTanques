# 🚰 Tank Supervisory System (SupCliente)

[cite_start]This project is a SCADA (Supervisory Control and Data Acquisition) client application developed in **C++**[cite: 13, 70]. [cite_start]It allows the remote monitoring and control of a tank plant via a TCP/IP connection with a server[cite: 71].

[cite_start]The project features a robust architecture, separating business and network communication logic from its user interfaces, offering two versions: a **Terminal (Console)** interface [cite: 70, 71] [cite_start]and a **Graphical User Interface (GUI)** built with the **Qt** framework[cite: 13].


## 🚀 Features

* **Two Access Levels:**
  * [cite_start]*Viewer:* Can only observe real-time data such as tank levels, pump flow, and valve states[cite: 17].
  * [cite_start]*Administrator:* Has full permission to act on the plant by opening/closing valves and adjusting pump power[cite: 17].
* [cite_start]**Real-Time Monitoring:** Periodically reads server sensors including H1 and H2 levels in cm and %, pump flow, and overflow alerts[cite: 17].
* [cite_start]**Visual and Graphical Representation (Qt Version):** Features panels with control buttons, virtual LCD displays, and the option to view data on a temporal graph[cite: 17].
* [cite_start]**Cross-Platform Communication:** Uses a customized `MySocket` library built over TCP sockets, which can be easily compiled on both Windows and Linux[cite: 71, 72].

## 📁 Project Structure

The source code is divided into logical modules:

* **Core Logic:**
  * [cite_start]`supcliente.h` / `.cpp`: Manages the main connection thread, communication (GET, SET, LOGIN, LOGOUT commands), and stores the last state of the plant[cite: 71].
  * [cite_start]`supdados.h` / `.cpp`: Data structures (such as `SupState`) that model the physical state of the plant[cite: 71].
* **Graphical User Interface (Qt):**
  * [cite_start]`supcliente_qt.h` / `.cpp` / `.ui`: Rich graphical interface with checkable buttons, sliders, LCDs (`QLCDNumber`), and graphs[cite: 13, 17].
  * [cite_start]`supcliente_main_qt.cpp`: Entry point for the graphical application[cite: 13].
  * [cite_start]`SupCliente.pro`: Qt Creator project file[cite: 13].
* **Terminal Interface (Console):**
  * [cite_start]`supcliente_term.h` / `.cpp`: Text-based command-line interface[cite: 70, 71].
  * [cite_start]`supcliente_main_term.cpp`: Entry point for the console application[cite: 70, 71].
  * [cite_start]`SupCliente.cbp`: Code::Blocks project file[cite: 70].
* **Networking (MySocket):**
  * [cite_start]`mysocket.h` / `.cpp`: An object-oriented library to manage connections via Sockets supporting blocking stream sockets and queues/selects with timeout[cite: 71, 72].

## 🛠️ Technologies Used

* [cite_start]**C++17:** Base language standard[cite: 13, 70].
* [cite_start]**Threads and Mutexes:** Used for periodic asynchronous data requests without freezing the interface[cite: 71].
* [cite_start]**Qt Framework (version 5/6):** Powering the rich and reactive frontend with Signals and Slots[cite: 13].
* [cite_start]**Winsock2 (`Ws2_32`):** Native network API when compiled under Windows environments[cite: 13, 70, 72].

## ⚙️ How to Compile and Run

The repository contains ready-to-use configurations for different IDEs depending on the desired interface.

### Option 1: Graphical User Interface Version (Qt Creator)
Recommended for a full user experience.
1. [cite_start]Open the `SupCliente.pro` file in **Qt Creator**[cite: 13].
2. Configure your Kit (MinGW or MSVC on Windows, GCC on Linux).
3. [cite_start]The `.pro` file is already configured to automatically add network dependencies (`LIBS += -lWs2_32` for Windows)[cite: 13].
4. Press `Ctrl + R` (or click the Play button) to compile and run.

### Option 2: Terminal Version (Code::Blocks)
Recommended for testing, environments without a graphical server, or educational purposes.
1. [cite_start]Open the `SupCliente.cbp` file in **Code::Blocks**[cite: 70].
2. [cite_start]Go to `Settings > Compiler` and ensure the `-std=c++17` option is enabled[cite: 70].
3. [cite_start]In the `Linker settings`, the project is already configured to include the static library `Ws2_32`[cite: 70].
4. Press `F9` (Build & Run) to start.

## 🔧 Configuring MySocket (Cross-Platform)
[cite_start]The `MySocket` library (`mysocket.cpp` and `mysocket.h`) supports both Windows and Linux[cite: 71].
[cite_start]By default, the code uses the Windows implementation (`#include <winsock2.h>`)[cite: 71].
[cite_start]If you are compiling on Linux, open `mysocket.h` and `mysocket.cpp` and follow the instructions at the beginning of each file: simply comment out the Windows block and uncomment the Linux block which switches to `<sys/socket.h>`[cite: 71].

## 💻 Basic Usage

1. Upon launching the application, enter the server's target `IP`.
2. Enter your credentials (`Login` and `Senha`).
   * *Note: The privilege level (admin/viewer) depends on the credentials stored on the server.*
3. After connecting, observe the parameters changing in real-time.
4. If you are an *Admin*, try dragging the pump flow slider (`Pump`) and enabling/disabling the valves (`Valve 1` and `Valve 2`) to see the plant react instantly[cite: 17].
