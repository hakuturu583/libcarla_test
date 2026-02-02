#include <carla/client/Client.h>
#include <carla/client/World.h>
#include <carla/client/Map.h>
#include <carla/client/BlueprintLibrary.h>
#include <carla/client/ActorBlueprint.h>
#include <carla/client/Vehicle.h>
#include <carla/geom/Transform.h>
#include <carla/geom/Location.h>
#include <carla/geom/Rotation.h>
#include <carla/rpc/VehicleControl.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

namespace cc = carla::client;
namespace cg = carla::geom;

using namespace std::chrono_literals;

int main(int argc, char* argv[]) {
    try {
        // CARLA server connection settings
        std::string host = "localhost";
        uint16_t port = 2000;

        if (argc > 1) {
            host = argv[1];
        }
        if (argc > 2) {
            port = std::stoi(argv[2]);
        }

        std::cout << "Connecting to CARLA server: " << host << ":" << port << std::endl;

        // Create client and connect
        auto client = cc::Client(host, port);
        client.SetTimeout(10s);

        // Get world
        std::cout << "Getting world information..." << std::endl;
        auto world = client.GetWorld();

        std::cout << "Getting map name..." << std::endl;
        auto map = world.GetMap();
        std::cout << "Connected to world: " << map->GetName() << std::endl;

        // Get blueprint library
        std::cout << "Getting blueprint library..." << std::endl;
        auto blueprint_library = world.GetBlueprintLibrary();

        // Find vehicle blueprint (use Tesla Model 3)
        std::cout << "Finding vehicle blueprint..." << std::endl;
        auto vehicle_bp = blueprint_library->Find("vehicle.tesla.model3");
        if (vehicle_bp == nullptr) {
            // If Tesla Model 3 is not found, use the first available vehicle
            auto vehicles = blueprint_library->Filter("vehicle.*");
            if (vehicles->empty()) {
                std::cerr << "Error: No vehicle blueprints found" << std::endl;
                return 1;
            }
            vehicle_bp = &(*vehicles)[0];
            std::cout << "Using vehicle: " << vehicle_bp->GetId() << std::endl;
        } else {
            std::cout << "Spawning Tesla Model 3" << std::endl;
        }

        // Get spawn points
        std::cout << "Getting spawn points..." << std::endl;
        auto spawn_points = map->GetRecommendedSpawnPoints();
        if (spawn_points.empty()) {
            std::cerr << "Error: No spawn points found" << std::endl;
            return 1;
        }

        // Use the first spawn point
        auto spawn_point = spawn_points[0];
        std::cout << "Spawn point: ("
                  << spawn_point.location.x << ", "
                  << spawn_point.location.y << ", "
                  << spawn_point.location.z << ")" << std::endl;

        // Spawn vehicle
        std::cout << "Spawning vehicle..." << std::endl;
        auto actor = world.SpawnActor(*vehicle_bp, spawn_point);
        std::cout << "Casting vehicle..." << std::endl;
        auto vehicle = std::static_pointer_cast<cc::Vehicle>(actor);
        std::cout << "Vehicle spawned (ID: " << vehicle->GetId() << ")" << std::endl;

        // Wait a bit
        std::this_thread::sleep_for(1s);

        // Setup spectator camera position (behind the vehicle)
        std::cout << "\nSetting up spectator camera..." << std::endl;
        auto spectator = world.GetSpectator();
        auto vehicle_transform = vehicle->GetTransform();
        auto forward_vector = vehicle_transform.GetForwardVector();

        // Position spectator behind and above the vehicle
        cg::Location spectator_location = vehicle_transform.location;
        spectator_location.x -= forward_vector.x * 7.0f;  // 7 meters behind
        spectator_location.y -= forward_vector.y * 7.0f;
        spectator_location.z += 3.0f;  // 3 meters above

        // Look at the vehicle
        cg::Rotation spectator_rotation = vehicle_transform.rotation;
        spectator_rotation.pitch = -10.0f;  // Angle down slightly

        spectator->SetTransform(cg::Transform(spectator_location, spectator_rotation));
        std::cout << "Spectator camera positioned behind vehicle" << std::endl;

        // Execute scenario: move vehicle forward
        std::cout << "\n=== Scenario start: Moving vehicle forward for 5 seconds ===" << std::endl;

        cc::Vehicle::Control control;
        control.throttle = 0.5f;  // Throttle 50%
        control.steer = 0.0f;     // Steering center
        control.brake = 0.0f;     // No brake

        // Move forward for 5 seconds
        for (int i = 0; i < 50; ++i) {
            vehicle->ApplyControl(control);

            // Get and display vehicle position
            auto location = vehicle->GetLocation();
            auto velocity = vehicle->GetVelocity();
            float speed = std::sqrt(velocity.x * velocity.x +
                                   velocity.y * velocity.y +
                                   velocity.z * velocity.z) * 3.6f; // m/s to km/h

            std::cout << "Time " << (i * 0.1f) << "s - "
                      << "Position: (" << location.x << ", " << location.y << ", " << location.z << ") "
                      << "Speed: " << speed << " km/h" << std::endl;

            std::this_thread::sleep_for(100ms);
        }

        // Apply brake and stop
        std::cout << "\nApplying brake to stop" << std::endl;
        control.throttle = 0.0f;
        control.brake = 1.0f;
        vehicle->ApplyControl(control);

        std::this_thread::sleep_for(2s);

        // Clean up
        // Note: Explicitly calling Destroy() can cause segfault in some CARLA versions.
        // The server will automatically clean up actors when the client disconnects.
        std::cout << "\nCleaning up..." << std::endl;

        // Reset all actors (server will handle cleanup)
        vehicle.reset();
        actor.reset();

        // Option 2: If you need to explicitly destroy, wrap in try-catch
        // try {
        //     vehicle->Destroy();
        // } catch (const std::exception& e) {
        //     std::cerr << "Warning during destroy: " << e.what() << std::endl;
        // }

        std::cout << "Scenario completed!" << std::endl;

        // Use std::exit() to avoid segfault during automatic cleanup
        // This is a workaround for a known issue with CARLA C++ client cleanup
        std::exit(0);

    } catch (const std::exception& e) {
        std::cerr << "Error occurred: " << e.what() << std::endl;
        std::exit(1);
    }

    return 0;
}
