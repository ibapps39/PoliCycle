#pragma once
#include <raylib.h>
#include <string>
#include <functional>
#include <cmath>

// Constants
#define FPS 60
#define GDP_CIRCLE_RADIUS 50
#define SECTOR_CIRCLE_RADIUS 40
#define SECTOR_DISTANCE 120
#define CAMERA_MOVE_SPEED 5.0f
#define CAMERA_ZOOM_SPEED 0.1f

// Economic Sector Types
enum SectorType {
    CONSUMER_SPENDING,  // C
    INVESTMENT,         // I
    GOVERNMENT,         // G
    NET_EXPORTS         // NX
};

// Economic Data Structure
struct EconomyData {
    float gdp;                  // Total GDP
    float consumerSpending;     // C
    float investment;           // I
    float governmentSpending;   // G
    float netExports;           // NX
    float population;           // Population size
    float esgRating;            // Environment, Social, Governance rating (0-100)
};

// Circle Entity Structure
struct CircleEntity {
    Vector2 position;
    float radius;
    Color color;
    std::string label;
    float value;
};

// Camera Control Structure
struct CameraControl {
    Camera2D camera;
    Vector2 target;
    float zoom;
    bool isDragging;
    Vector2 dragStart;
};

// Function prototypes - using functional C++ style with pure functions where possible

// Camera control functions
CameraControl initCamera();
CameraControl updateCamera(const CameraControl& cam, bool mouseControl);

// Economy calculation functions
float calculateGDP(float c, float i, float g, float nx);
float updateConsumerSpending(float population, float prevValue, float deltaTime);
float updateInvestment(float esgRating, float prevValue, float deltaTime);
float updateGovernmentSpending(float population, float prevValue, float deltaTime);
float updateNetExports(float esgRating, float population, float prevValue, float deltaTime);
float updateESGRating(float currentESG, float deltaTime);
float updatePopulation(float currentPopulation, float deltaTime);

// Rendering functions
void drawGDPCircle(const CircleEntity& entity);
void drawSectorCircle(const CircleEntity& entity);
void drawEconomyStats(const EconomyData& economy, const Vector2& position);

// Utility functions
Color getColorBasedOnGrowth(float value, float prevValue);
std::string formatNumber(float number);
Vector2 getSectorPosition(Vector2 center, SectorType sector);
CircleEntity createCircleEntity(Vector2 position, float radius, Color color, const std::string& label, float value);