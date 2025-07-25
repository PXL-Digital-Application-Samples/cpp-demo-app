# 🧩 C++ User Management API

A lightweight C++ REST API for managing users (Create, Read, Update, Delete) with in-memory data storage. Built with cpp-httplib (a modern, header-only HTTP library) and includes auto-seeded user data and interactive Swagger (OpenAPI) documentation served at the root (`/`).

---

## 📦 Features

- ✅ In-memory storage with thread-safe operations
- 🔄 Full CRUD endpoints: `POST`, `GET`, `PUT`, `DELETE`  
- 🧪 Swagger UI for testing & docs at `/` 
- 🚀 High performance C++ implementation
- 🧰 Unit tests with Catch2 framework
- 📦 Modern CMake build system with automatic dependency management

---

## 🛠️ Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+ (updated for better dependency compatibility)

---

## 🏗️ Building the Project

### Create build directory

```bash
mkdir build
cd build
```

### Configure with CMake

```bash
cmake ..
```

**Note:** The first build will take longer as CMake automatically downloads dependencies (cpp-httplib, nlohmann/json, Catch2).

### Build

```bash
# Linux/macOS
make -j$(nproc)

# Windows (with Visual Studio)
cmake --build . --config Release
```

---

## 🚀 Running the Application

### Run the API server

```bash
# From the build directory
./user_api                    # Linux/macOS
# OR
.\Release\user_api.exe        # Windows (Visual Studio build)

# The API will start on http://localhost:5000
```

**Default Configuration:**
- Server runs on port 5000
- Pre-seeded with 3 sample users (Alice, Bob, Charlie)
- Thread-safe in-memory storage

### Access Swagger UI

Open your browser and navigate to `http://localhost:5000/`

---

## 🧪 Running Tests

```bash
# From the build directory
./test_api                    # Linux/macOS
# OR
.\Release\test_api.exe        # Windows (Visual Studio build)

# Alternative: Use CTest (cross-platform)
ctest --verbose
```

---

## 📚 API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET    | `/users` | Get all users |
| GET    | `/users/{id}` | Get a specific user |
| POST   | `/users` | Create a new user |
| PUT    | `/users/{id}` | Update a user |
| DELETE | `/users/{id}` | Delete a user |

### Request/Response Examples

#### Create User

```bash
curl -X POST http://localhost:5000/users \
  -H "Content-Type: application/json" \
  -d '{"name": "John Doe", "email": "john@example.com"}'
```

#### Get All Users

```bash
curl http://localhost:5000/users
```

#### Update User

```bash
curl -X PUT http://localhost:5000/users/1 \
  -H "Content-Type: application/json" \
  -d '{"name": "Jane Doe"}'
```

---

## 📁 Project Structure

```text
.
├── CMakeLists.txt      # Build configuration with automatic dependency management
├── CMakePresets.json   # CMake presets for different build configurations
├── Makefile           # Alternative simple build system (manual dependencies)
├── vcpkg.json         # Package manifest for vcpkg (alternative dependency manager)
├── src/
│   └── main.cpp        # Main application with REST API and Swagger UI
├── tests/
│   └── test_main.cpp   # Unit tests with Catch2 framework
└── README.md           # This file
```

---

## 🔧 Dependencies

All dependencies are automatically fetched and built by CMake (no manual installation required):

- **[cpp-httplib](https://github.com/yhirose/cpp-httplib)** v0.14.3 - Modern, header-only C++ HTTP/HTTPS library (excellent Windows support)
- **[nlohmann/json](https://github.com/nlohmann/json)** v3.11.3 - JSON for Modern C++ (header-only)
- **[Catch2](https://github.com/catchorg/Catch2)** v3.4.0 - Modern C++ testing framework

### Alternative Build Methods

**Option 1: CMake (Recommended)**
- Automatic dependency management
- Cross-platform support
- Integrated testing with CTest

**Option 2: Traditional Makefile**
- Requires manual dependency installation
- See comments in `Makefile` for setup instructions

**Option 3: vcpkg**
- Use `vcpkg.json` manifest for package management
- Compatible with Visual Studio integration
