#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <string>

using json = nlohmann::json;

// Mock user storage for testing
class UserStore {
private:
    std::map<int, json> users;
    int next_id = 1;

public:
    void clear() {
        users.clear();
        next_id = 1;
    }
    
    void seed() {
        users[1] = {{"id", 1}, {"name", "Alice"}, {"email", "alice@example.com"}};
        users[2] = {{"id", 2}, {"name", "Bob"}, {"email", "bob@example.com"}};
        next_id = 3;
    }
    
    json getAll() {
        json result = json::array();
        for (const auto& [id, user] : users) {
            result.push_back(user);
        }
        return result;
    }
    
    json getUser(int id) {
        if (users.find(id) == users.end()) {
            return nullptr;
        }
        return users[id];
    }
    
    json createUser(const std::string& name, const std::string& email) {
        json new_user = {
            {"id", next_id++},
            {"name", name},
            {"email", email}
        };
        users[new_user["id"]] = new_user;
        return new_user;
    }
    
    json updateUser(int id, const json& updates) {
        if (users.find(id) == users.end()) {
            return nullptr;
        }
        
        if (updates.contains("name")) {
            users[id]["name"] = updates["name"];
        }
        if (updates.contains("email")) {
            users[id]["email"] = updates["email"];
        }
        
        return users[id];
    }
    
    bool deleteUser(int id) {
        if (users.find(id) == users.end()) {
            return false;
        }
        users.erase(id);
        return true;
    }
    
    size_t size() {
        return users.size();
    }
};

TEST_CASE("User Management API Tests", "[api]") {
    UserStore store;
    
    SECTION("Get all users") {
        store.seed();
        json users = store.getAll();
        
        REQUIRE(users.is_array());
        REQUIRE(users.size() == 2);
        REQUIRE(users[0]["name"] == "Alice");
        REQUIRE(users[1]["name"] == "Bob");
    }
    
    SECTION("Get single user") {
        store.seed();
        json user = store.getUser(1);
        
        REQUIRE(user != nullptr);
        REQUIRE(user["name"] == "Alice");
        REQUIRE(user["email"] == "alice@example.com");
    }
    
    SECTION("Get non-existent user") {
        store.seed();
        json user = store.getUser(99);
        
        REQUIRE(user == nullptr);
    }
    
    SECTION("Create user") {
        store.clear();
        json new_user = store.createUser("Charlie", "charlie@example.com");
        
        REQUIRE(new_user["name"] == "Charlie");
        REQUIRE(new_user["email"] == "charlie@example.com");
        REQUIRE(new_user["id"].is_number());
        REQUIRE(store.size() == 1);
    }
    
    SECTION("Update user") {
        store.seed();
        json updates = {{"name", "Alice Updated"}};
        json updated_user = store.updateUser(1, updates);
        
        REQUIRE(updated_user != nullptr);
        REQUIRE(updated_user["name"] == "Alice Updated");
        REQUIRE(updated_user["email"] == "alice@example.com");
    }
    
    SECTION("Update non-existent user") {
        store.seed();
        json updates = {{"name", "Nobody"}};
        json result = store.updateUser(999, updates);
        
        REQUIRE(result == nullptr);
    }
    
    SECTION("Delete user") {
        store.seed();
        bool deleted = store.deleteUser(1);
        
        REQUIRE(deleted == true);
        REQUIRE(store.size() == 1);
        REQUIRE(store.getUser(1) == nullptr);
    }
    
    SECTION("Delete non-existent user") {
        store.seed();
        bool deleted = store.deleteUser(999);
        
        REQUIRE(deleted == false);
        REQUIRE(store.size() == 2);
    }
    
    SECTION("JSON Validation") {
        // Test JSON parsing edge cases
        json valid_user = {{"name", "Test"}, {"email", "test@example.com"}};
        REQUIRE(valid_user.contains("name"));
        REQUIRE(valid_user.contains("email"));
        
        json invalid_user = {{"name", "Test"}};
        REQUIRE(invalid_user.contains("name"));
        REQUIRE_FALSE(invalid_user.contains("email"));
    }
}