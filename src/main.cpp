#include <httplib.h>
#include <nlohmann/json.hpp>
#include <map>
#include <mutex>
#include <cstdlib>
#include <iostream>
#include <string>

using json = nlohmann::json;
using namespace httplib;

// Global variables
std::map<int, json> users;
int next_id = 1;
std::mutex users_mutex;

// Swagger HTML template
const std::string swagger_html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>User Management API</title>
    <link rel="stylesheet" type="text/css" href="https://unpkg.com/swagger-ui-dist/swagger-ui.css">
    <script src="https://unpkg.com/swagger-ui-dist/swagger-ui-bundle.js"></script>
    <script src="https://unpkg.com/swagger-ui-dist/swagger-ui-standalone-preset.js"></script>
</head>
<body>
    <div id="swagger-ui"></div>
    <script>
    const spec = {
        "openapi": "3.0.0",
        "info": {
            "title": "User Management API",
            "version": "1.0.0",
            "description": "A C++ REST API for managing users with CRUD operations"
        },
        "servers": [{"url": window.location.origin}],
        "paths": {
            "/users": {
                "get": {
                    "summary": "Get all users",
                    "responses": {
                        "200": {
                            "description": "List of users",
                            "content": {
                                "application/json": {
                                    "schema": {
                                        "type": "array",
                                        "items": {
                                            "$ref": "#/components/schemas/User"
                                        }
                                    }
                                }
                            }
                        }
                    }
                },
                "post": {
                    "summary": "Create a new user",
                    "requestBody": {
                        "required": true,
                        "content": {
                            "application/json": {
                                "schema": {
                                    "type": "object",
                                    "required": ["name", "email"],
                                    "properties": {
                                        "name": {"type": "string"},
                                        "email": {"type": "string"}
                                    }
                                }
                            }
                        }
                    },
                    "responses": {
                        "201": {"description": "User created"},
                        "400": {"description": "Invalid input"}
                    }
                }
            },
            "/users/{id}": {
                "get": {
                    "summary": "Get a user by ID",
                    "parameters": [{
                        "name": "id",
                        "in": "path",
                        "required": true,
                        "schema": {"type": "integer"}
                    }],
                    "responses": {
                        "200": {"description": "User data"},
                        "404": {"description": "User not found"}
                    }
                },
                "put": {
                    "summary": "Update a user",
                    "parameters": [{
                        "name": "id",
                        "in": "path",
                        "required": true,
                        "schema": {"type": "integer"}
                    }],
                    "requestBody": {
                        "content": {
                            "application/json": {
                                "schema": {
                                    "type": "object",
                                    "properties": {
                                        "name": {"type": "string"},
                                        "email": {"type": "string"}
                                    }
                                }
                            }
                        }
                    },
                    "responses": {
                        "200": {"description": "User updated"},
                        "404": {"description": "User not found"}
                    }
                },
                "delete": {
                    "summary": "Delete a user",
                    "parameters": [{
                        "name": "id",
                        "in": "path",
                        "required": true,
                        "schema": {"type": "integer"}
                    }],
                    "responses": {
                        "200": {"description": "User deleted"},
                        "404": {"description": "User not found"}
                    }
                }
            }
        },
        "components": {
            "schemas": {
                "User": {
                    "type": "object",
                    "properties": {
                        "id": {"type": "integer"},
                        "name": {"type": "string"},
                        "email": {"type": "string"}
                    }
                }
            }
        }
    };
    
    window.onload = function() {
        SwaggerUIBundle({
            spec: spec,
            dom_id: '#swagger-ui',
            presets: [
                SwaggerUIBundle.presets.apis,
                SwaggerUIStandalonePreset
            ],
            layout: "StandaloneLayout"
        });
    }
    </script>
</body>
</html>
)";

void seed_users() {
    std::lock_guard<std::mutex> lock(users_mutex);
    
    json alice = {{"id", next_id++}, {"name", "Alice"}, {"email", "alice@example.com"}};
    json bob = {{"id", next_id++}, {"name", "Bob"}, {"email", "bob@example.com"}};
    json charlie = {{"id", next_id++}, {"name", "Charlie"}, {"email", "charlie@example.com"}};
    
    users[alice["id"]] = alice;
    users[bob["id"]] = bob;
    users[charlie["id"]] = charlie;
}

// Helper function to set JSON response headers
void set_json_response(Response& res, const json& data, int status = 200) {
    res.set_content(data.dump(), "application/json");
    res.status = status;
}

int main() {
    Server server;
    
    // Seed initial users
    seed_users();
    
    // Swagger UI at root
    server.Get("/", [](const Request&, Response& res) {
        res.set_content(swagger_html, "text/html");
    });
    
    // GET all users
    server.Get("/users", [](const Request&, Response& res) {
        std::lock_guard<std::mutex> lock(users_mutex);
        json users_array = json::array();
        
        for (const auto& [id, user] : users) {
            users_array.push_back(user);
        }
        
        set_json_response(res, users_array);
    });
    
    // POST create user
    server.Post("/users", [](const Request& req, Response& res) {
        try {
            auto body = json::parse(req.body);
            
            if (!body.contains("name") || !body.contains("email")) {
                set_json_response(res, {{"error", "Name and email are required"}}, 400);
                return;
            }
            
            std::lock_guard<std::mutex> lock(users_mutex);
            json new_user = {
                {"id", next_id++},
                {"name", body["name"]},
                {"email", body["email"]}
            };
            
            users[new_user["id"]] = new_user;
            set_json_response(res, new_user, 201);
            
        } catch (const json::exception& e) {
            set_json_response(res, {{"error", "Invalid JSON"}}, 400);
        }
    });
    
    // GET single user
    server.Get(R"(/users/(\d+))", [](const Request& req, Response& res) {
        int id = std::stoi(req.matches[1]);
        std::lock_guard<std::mutex> lock(users_mutex);
        
        if (users.find(id) == users.end()) {
            set_json_response(res, {{"error", "User not found"}}, 404);
            return;
        }
        
        set_json_response(res, users[id]);
    });
    
    // PUT update user
    server.Put(R"(/users/(\d+))", [](const Request& req, Response& res) {
        int id = std::stoi(req.matches[1]);
        std::lock_guard<std::mutex> lock(users_mutex);
        
        if (users.find(id) == users.end()) {
            set_json_response(res, {{"error", "User not found"}}, 404);
            return;
        }
        
        try {
            auto body = json::parse(req.body);
            
            if (body.contains("name")) {
                users[id]["name"] = body["name"];
            }
            if (body.contains("email")) {
                users[id]["email"] = body["email"];
            }
            
            set_json_response(res, users[id]);
            
        } catch (const json::exception& e) {
            set_json_response(res, {{"error", "Invalid JSON"}}, 400);
        }
    });
    
    // DELETE user
    server.Delete(R"(/users/(\d+))", [](const Request& req, Response& res) {
        int id = std::stoi(req.matches[1]);
        std::lock_guard<std::mutex> lock(users_mutex);
        
        if (users.find(id) == users.end()) {
            set_json_response(res, {{"error", "User not found"}}, 404);
            return;
        }
        
        users.erase(id);
        set_json_response(res, {{"message", "User deleted"}});
    });
    
    // CORS headers for all requests (useful for frontend development)
    server.set_post_routing_handler([](const Request&, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    });
    
    // Handle OPTIONS requests for CORS preflight
    server.Options(R"(/users.*)", [](const Request&, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 200;
    });
    
    // Get port from environment variable or use default
    int port = 5000;
    const char* env_port = std::getenv("PORT");
    if (env_port) {
        port = std::atoi(env_port);
    }
    
    // Start server
    std::cout << "🚀 User Management API starting on http://localhost:" << port << std::endl;
    std::cout << "📖 Swagger UI available at http://localhost:" << port << "/" << std::endl;
    std::cout << "Press Ctrl+C to stop the server..." << std::endl;
    
    if (!server.listen("0.0.0.0", port)) {
        std::cerr << "Failed to start server on port " << port << std::endl;
        return 1;
    }
    
    return 0;
}