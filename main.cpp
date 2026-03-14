#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include "crow_all.h"

using namespace std;
using namespace crow;

// --- Data Models (exactly required fields) ---

struct Customer {
    int id;
    string name;
    string email;
    string username;
    string password;
    string creditCardNumber;
};

struct Admin {
    string username;
    string password;
};

// --- In-memory predefined data (loaded at startup) ---

static vector<Customer> customers = {
    {100, "Alice Smith", "alice@example.com", "alice", "password123", "4111111111111111"},
    {101, "Bob Jones",   "bob@example.com",   "bobby", "hunter2",     "4242424242424242"}
};

static vector<Admin> admins = {
    {"admin",   "adminpass"},
    {"manager", "letmein"}
};

static int nextCustomerId = []{
    int maxId = 0;
    for (const auto& c : customers) maxId = max(maxId, c.id);
    return (maxId > 0 ? maxId + 1 : 1);
}();

int main() {
    SimpleApp app;

    // --- POST /login ---
    // Input JSON: { "username": "<string>", "password": "<string>" }
    // Success: { "success": true, "role": "admin" | "customer" }
    // Failure: { "success": false, "error": "Invalid credentials" | "Missing username or password" | "Invalid payload" }
    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::Post)([] (const request& req) {
        auto body = json::load(req.body);
        json::wvalue resp;

        if (!body) {
            resp["success"] = false;
            resp["error"] = "Invalid payload";
            return response{200, resp};
        }
        if (!body.has("username") || !body.has("password")) {
            resp["success"] = false;
            resp["error"] = "Missing username or password";
            return response{200, resp};
        }
        if (!(body["username"].t() == json::type::String) || !(body["password"].t() == json::type::String)) {
            resp["success"] = false;
            resp["error"] = "Invalid payload";
            return response{200, resp};
        }

        string username = string(body["username"].s());
        string password = string(body["password"].s());

        // Check Admins
        for (const auto& a : admins) {
            if (a.username == username && a.password == password) {
                resp["success"] = true;
                resp["role"] = "admin";
                return response{200, resp};
            }
        }

        // Check Customers
        for (const auto& c : customers) {
            if (c.username == username && c.password == password) {
                resp["success"] = true;
                resp["role"] = "customer";
                return response{200, resp};
            }
        }

        // Invalid
        resp["success"] = false;
        resp["error"] = "Invalid credentials";
        return response{200, resp};
    });

    // --- POST /signup ---
    // Input JSON: {
    //   "name": "<string>", "email": "<string>", "username": "<string>",
    //   "password": "<string>", "creditCardNumber": "<string>"
    // }
    // Success: { "success": true, "customerID": <int> }
    // Failure: { "success": false, "error": "..." }
    CROW_ROUTE(app, "/signup").methods(crow::HTTPMethod::Post)([] (const request& req) {
        auto body = json::load(req.body);
        json::wvalue resp;

        if (!body) {
            resp["success"] = false;
            resp["error"] = "Invalid payload";
            return response{200, resp};
        }

        // Require all fields
        const char* requiredFields[] = {"name", "email", "username", "password", "creditCardNumber"};
        for (const auto* f : requiredFields) {
            if (!body.has(f) || !(body[f].t() == json::type::String)) {
                resp["success"] = false;
                resp["error"] = "Missing or invalid field: " + string(f);
                return response{200, resp};
            }
        }

        string name  = string(body["name"].s());
        string email = string(body["email"].s());
        string uname = string(body["username"].s());
        string pass  = string(body["password"].s());
        string cc    = string(body["creditCardNumber"].s());

        // Basic non-empty validation
        if (name.empty() || email.empty() || uname.empty() || pass.empty() || cc.empty()) {
            resp["success"] = false;
            resp["error"] = "All fields must be non-empty";
            return response{200, resp};
        }

        // Uniqueness: username (across admins + customers), email (across customers)
        // Username vs Admins
        for (const auto& a : admins) {
            if (a.username == uname) {
                resp["success"] = false;
                resp["error"] = "Username already exists";
                return response{200, resp};
            }
        }
        // Username vs Customers
        for (const auto& c : customers) {
            if (c.username == uname) {
                resp["success"] = false;
                resp["error"] = "Username already exists";
                return response{200, resp};
            }
        }
        // Email vs Customers
        for (const auto& c : customers) {
            if (c.email == email) {
                resp["success"] = false;
                resp["error"] = "Email already exists";
                return response{200, resp};
            }
        }

        // Insert new customer
        Customer newCust;
        newCust.id = nextCustomerId++;
        newCust.name = name;
        newCust.email = email;
        newCust.username = uname;
        newCust.password = pass;
        newCust.creditCardNumber = cc;

        customers.push_back(std::move(newCust));

        resp["success"] = true;
        resp["customerID"] = customers.back().id;
        return response{200, resp};
    });

    // Start server (no extra routes)
    app.port(18080).multithreaded().run();
    return 0;
}