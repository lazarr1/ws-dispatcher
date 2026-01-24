# ws-dispatcher

An asynchronous C++20 WebSocket server framework built on Boost.Asio and Boost.Beast, designed for high-performance message handling with thread pool support. The idea behind this project is to be bring up a C++ application without concerns over Client/Server lifetime, sockets/ websocket protocols, threads and asynchronous I/O.

## Overview

**ws-dispatcher** is a lightweight, production-ready WebSocket server library that provides:

- **Asynchronous I/O**: Built on Boost.Asio for non-blocking network operations
- **WebSocket Support**: Full RFC 6455 WebSocket protocol support via Boost.Beast
- **Thread Pool Processing**: Offloads message handling to a configurable worker thread pool
- **Message Queuing**: Thread-safe message queuing with automatic delivery
- **Extensible Architecture**: Custom service handlers for implementing business logic
- **Modern C++**: Written in C++20 with clean, maintainable code

## Purpose

ws-dispatcher simplifies building scalable WebSocket servers in C++. It handles all the low-level socket management, WebSocket handshakes, and asynchronous I/O, allowing you to focus on implementing your application logic through service handlers.

Key use cases:
- Real-time communication servers
- Push notification systems
- Live data streaming
- Interactive applications requiring bidirectional communication

## Requirements

- **C++20** or later
- **Boost**: 1.80 or later (requires `system` component)
- **CMake**: 3.20 or later
- **Compiler**: GCC, Clang, or MSVC with C++20 support

## Building as a Library

### Quick Start

1. **Clone and navigate to the project:**
   ```bash
   cd ws-dispatcher
   ```

2. **Build the library:**
   ```bash
   # Debug build
   ./build_debug.sh

   # Or release build
   ./build_release.sh
   ```

   Alternatively, use CMake directly:
   ```bash
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```

3. **Include in your project:**
   
   Add to your CMakeLists.txt:
   ```cmake
   find_package(Boost 1.80 REQUIRED COMPONENTS system)
   add_subdirectory(ws-dispatcher)

   # Link to your target
   target_link_libraries(your_target PRIVATE WsDispatcher::WsDispatcher)
   ```

### Building Without Examples

To build only the library without example executables:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DWSDISPATCHER_BUILD_EXAMPLES=OFF
cmake --build build
```

## Architecture

### Core Components

**Server** (`server.hpp`/`server.cpp`)
- Manages the main TCP acceptor
- Binds to a specified endpoint
- Creates new sessions for incoming connections

**Dispatcher** (`dispatcher.hpp`/`dispatcher.cpp`)
- Accepts incoming TCP connections
- Manages WebSocket handshakes
- Coordinates session lifecycle
- Runs the main I/O context event loop

**Session** (`session.hpp`/`session.cpp`)
- Handles individual WebSocket connections
- Manages message reading and writing
- Routes messages to service handlers
- Thread-safe message queuing

**Service Handler** (`service_handler.hpp`)
- Abstract interface for custom business logic
- Receives incoming messages
- Returns responses and session control actions
- Can maintain per-session state

### Message Flow

```
Client → TCP Connection → Dispatcher → Session → Service Handler
                                          ↓
                                    Worker Thread Pool
                                          ↓
                          Message Processing (Custom Logic)
                                          ↓
Client ← WebSocket Frame ← Message Queue ← Session
```

## Usage Example

### Implement a Custom Service Handler

```cpp
#include "service_handler.hpp"

class MyServiceHandler : public IServiceHandler {
public:
    ServiceResult onMessage(std::string msg) override {
        ServiceResult result;
        
        // Your business logic here
        std::cout << "Received: " << msg << std::endl;
        
        // Send response
        result.outgoing_msgs.push_back("Echo: " + msg);
        
        // Keep connection open
        result.action = SessionAction::Continue;
        
        return result;
    }
};
```

### Set Up the Server

```cpp
#include "dispatcher.hpp"
#include "my_service_handler.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/thread_pool.hpp>

int main() {
    // Create I/O context and thread pool
    net::io_context ioc;
    net::thread_pool workers(4);  // 4 worker threads
    
    // Create endpoint
    auto endpoint = tcp::endpoint(tcp::v4(), 8080);
    
    // Create and start dispatcher
    auto dispatcher = std::make_shared<Dispatcher<MyServiceHandler>>(workers, ioc, endpoint);
    dispatcher->do_accept();
    
    return 0;
}
```

## API Reference

### Dispatcher

```cpp
Dispatcher<T>(net::thread_pool& tp, net::io_context& ioc, tcp::endpoint& endpoint);
void do_accept();
```

Creates a WebSocket server listening on the specified endpoint with the given thread pool for message processing. Template type T must implement the IServiceHandler interface.

### Session

```cpp
Session(tcp::socket& socket, net::thread_pool& tp, net::io_context& ioc, 
        std::unique_ptr<IServiceHandler> sh);
void start();
```

Handles individual client connections. Created automatically by the dispatcher.

### IServiceHandler

```cpp
class IServiceHandler {
    virtual ServiceResult onMessage(std::string msg) = 0;
};
```

Override `onMessage()` to handle incoming WebSocket messages.

### ServiceResult

```cpp
struct ServiceResult {
    std::vector<std::string> outgoing_msgs;  // Messages to send back to client
    SessionAction action;                     // Control action (Continue/Close/Error)
};
```

### SessionAction

```cpp
enum class SessionAction {
    Continue,  // Keep connection open and await next message
    Close,     // Close the connection gracefully
    Error      // Close due to error
};
```

## Performance Characteristics

- **Concurrency**: Handles multiple concurrent WebSocket connections
- **Message Processing**: Offloaded to thread pool to prevent blocking I/O operations
- **Memory Safety**: Uses smart pointers and RAII for automatic resource management
- **Thread Safety**: Message queues protected with mutexes for concurrent access

## Building with Custom Options

```bash
# Debug build with examples
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DWSDISPATCHER_BUILD_EXAMPLES=ON
cmake --build build

# Release build without examples
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DWSDISPATCHER_BUILD_EXAMPLES=OFF
cmake --build build
```

## Compilation Flags

The library is built with strict compiler warnings:
- `-Wall -Wextra -Wpedantic` on GCC/Clang
- C++20 standard enforced

## License

See LICENSE file for details.

## Contributing

Contributions are welcome! Please ensure code follows the existing style and includes appropriate tests.
