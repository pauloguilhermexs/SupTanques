# SupTanques - Tank System Supervisory 🚰

This project consists of a simplified supervisory control system (SCADA), named **SupTanques**, developed in **C++**. The main goal is to remotely monitor and manage a level control plant composed of two interconnected tanks, a storage reservoir, a variable-flow water pump, liquid level sensors, flow sensors, and solenoid valves. 

The application implements a robust **Client-Server Architecture** using TCP/IP sockets to allow multiple operators to supervise and interact with the industrial plant concurrently.

## 🚀 Features

- **Distributed Client-Server Architecture**: The server application interfaces directly with the hardware simulation (sensors and actuators), while multiple remote clients can connect concurrently to visualize data and send commands via TCP/IP sockets over the internet or local network.
- **Role-Based Access Control (RBAC)**: Connection to the supervisory system requires authentication. Users are categorized by permission levels:
  - **Viewers**: Can observe the real-time state of the plant but cannot modify it.
  - **Administrators**: Possess full control to trigger valves and adjust the pump flow, in addition to viewing the data.
- **Real-Time Actuation**: Administrators can remotely open/close the solenoid valves (Tank 1 and Tank 2) and specify the precise input signal to adjust the pump's flow rate.
- **Hardware Simulation**: The server integrates a built-in simulation engine that mimics real-world physics, tracking liquid volumes, overflow alarms, and gravity-fed flow between the orifice connecting the two tanks.
- **Dual Visual Interface Modes (Qt Client)**: The graphical client provides two real-time observation modes:
  - **Synoptic View**: A 2D animation showing the current water levels of both tanks, valve states, pump percentages, and visual overflow alerts.
  - **Historical Trend Chart**: A line graph plotting the liquid level variations over time.
- **Console Fallback Client**: Includes a lightweight text-based terminal client for fast monitoring, debugging, and user management (add/remove users).

## 🛠️ Structure and Main Classes

- **`SupServidor`**: The core server engine. It inherits from the plant simulator (`Tanks`), manages incoming TCP connections using the `MySocket` library, authenticates logins, broadcasts the `SupState` packet, and executes actuation commands.
- **`SupCliente`**: An abstract base class defining the standard protocol for requesting and parsing data from the server.
- **`SupCliente_Term`**: A concrete implementation of the client running directly in the command-line interface.
- **`SupClienteQt`**: A graphical implementation of the client utilizing the Qt Framework for UI rendering (`QMainWindow`, custom canvas elements, and pop-up login dialogs).
- **Communication Protocol**: The system uses a strict 16-bit unsigned integer (`uint16_t`) protocol for sending actions (e.g., `CMD_LOGIN`, `CMD_GET_DATA`, `CMD_SET_PUMP`), ensuring lightweight and reliable data transmission.

## 💻 Technologies

- **Programming Language**: C++ (System Programming, Object-Oriented Design)
- **Networking**: Windows Sockets API (`Ws2_32`) via custom `MySocket` wrapper.
- **UI Framework**: Qt (for the graphical client interface).
- **Compilation**: Static linking via GCC/MinGW (for standalone executable generation).
