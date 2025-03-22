#include "economy.hpp"

// Camera functions
CameraControl initCamera() {
    CameraControl cam;
    cam.camera.target = {0, 0};
    cam.camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    cam.camera.rotation = 0.0f;
    cam.camera.zoom = 1.0f;
    cam.target = {0, 0};
    cam.zoom = 1.0f;
    cam.isDragging = false;
    cam.dragStart = {0, 0};
    return cam;
}

CameraControl updateCamera(const CameraControl& cam, bool mouseControl) {
    CameraControl newCam = cam;
    
    // Keyboard movement with WASD
    if (IsKeyDown(KEY_W)) newCam.target.y -= CAMERA_MOVE_SPEED / newCam.zoom;
    if (IsKeyDown(KEY_S)) newCam.target.y += CAMERA_MOVE_SPEED / newCam.zoom;
    if (IsKeyDown(KEY_A)) newCam.target.x -= CAMERA_MOVE_SPEED / newCam.zoom;
    if (IsKeyDown(KEY_D)) newCam.target.x += CAMERA_MOVE_SPEED / newCam.zoom;
    
    // Mouse zoom
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        newCam.zoom += wheel * CAMERA_ZOOM_SPEED * newCam.zoom;
        if (newCam.zoom < 0.1f) newCam.zoom = 0.1f;
    }
    
    // Mouse panning
    if (mouseControl) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            newCam.isDragging = true;
            newCam.dragStart = GetMousePosition();
        }
        
        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            newCam.isDragging = false;
        }
        
        if (newCam.isDragging) {
            Vector2 mousePos = GetMousePosition();
            Vector2 delta = {
                (mousePos.x - newCam.dragStart.x) / newCam.zoom,
                (mousePos.y - newCam.dragStart.y) / newCam.zoom
            };
            newCam.target.x -= delta.x;
            newCam.target.y -= delta.y;
            newCam.dragStart = mousePos;
        }
    }
    
    // Update camera
    newCam.camera.target = newCam.target;
    newCam.camera.zoom = newCam.zoom;
    
    return newCam;
}

// Economy calculation functions
float calculateGDP(float c, float i, float g, float nx) {
    return c + i + g + nx;
}

float updateConsumerSpending(float population, float prevValue, float deltaTime) {
    // Simple model: consumer spending grows with population and has some natural fluctuation
    float baseGrowth = 0.05f * deltaTime;
    float populationFactor = 0.01f * population * deltaTime;
    float randomFactor = (float)GetRandomValue(-100, 100) / 10000.0f;
    return prevValue * (1.0f + baseGrowth + populationFactor + randomFactor);
}

float updateInvestment(float esgRating, float prevValue, float deltaTime) {
    // Investment is influenced by ESG rating
    float baseGrowth = 0.03f * deltaTime;
    float esgFactor = 0.002f * esgRating * deltaTime;
    float randomFactor = (float)GetRandomValue(-200, 200) / 10000.0f;
    return prevValue * (1.0f + baseGrowth + esgFactor + randomFactor);
}

float updateGovernmentSpending(float population, float prevValue, float deltaTime) {
    // Government spending grows with population
    float baseGrowth = 0.02f * deltaTime;
    float populationFactor = 0.005f * population * deltaTime;
    float randomFactor = (float)GetRandomValue(-50, 50) / 10000.0f;
    return prevValue * (1.0f + baseGrowth + populationFactor + randomFactor);
}

float updateNetExports(float esgRating, float population, float prevValue, float deltaTime) {
    // Net exports are influenced by ESG rating and population
    float baseChange = 0.01f * deltaTime;
    float esgFactor = 0.003f * esgRating * deltaTime;
    float populationFactor = -0.002f * population * deltaTime; // Larger countries tend to import more
    float randomFactor = (float)GetRandomValue(-300, 300) / 10000.0f;
    return prevValue * (1.0f + baseChange + esgFactor + populationFactor + randomFactor);
}

float updateESGRating(float currentESG, float deltaTime) {
    // ESG varies slightly over time with some randomness
    float change = (float)GetRandomValue(-100, 100) / 1000.0f * deltaTime;
    float newValue = currentESG + change;
    return fmax(0.0f, fmin(100.0f, newValue)); // Clamp between 0 and 100
}

float updatePopulation(float currentPopulation, float deltaTime) {
    // Population grows at a steady rate with small variations
    float growthRate = 0.001f * deltaTime;
    float randomFactor = (float)GetRandomValue(-10, 10) / 10000.0f;
    return currentPopulation * (1.0f + growthRate + randomFactor);
}

// Rendering functions
void drawGDPCircle(const CircleEntity& entity) {
    DrawCircleV(entity.position, entity.radius, entity.color);
    DrawCircleLines(entity.position.x, entity.position.y, entity.radius, ColorAlpha(BLACK, 0.7f));
    
    // Draw GDP label in the center
    const char* gdpText = entity.label.c_str();
    int textWidth = MeasureText(gdpText, 20);
    DrawText(gdpText, entity.position.x - textWidth/2, entity.position.y - 30, 20, BLACK);
    
    // Draw GDP value
    const char* valueText = TextFormat("$%.2fT", entity.value / 1000.0f); // Assume in billions, display as trillions
    int valueWidth = MeasureText(valueText, 24);
    DrawText(valueText, 
        entity.position.x - valueWidth/2, entity.position.y - 10, 24, BLACK);
}

void drawSectorCircle(const CircleEntity& entity) {
    DrawCircleV(entity.position, entity.radius, entity.color);
    DrawCircleLines(entity.position.x, entity.position.y, entity.radius, ColorAlpha(BLACK, 0.7f));
    
    // Draw sector label
    const char* labelText = entity.label.c_str();
    int textWidth = MeasureText(labelText, 18);
    DrawText(labelText, entity.position.x - textWidth/2, entity.position.y - 25, 18, BLACK);
    
    // Draw sector value
    const char* valueText = TextFormat("$%.2fB", entity.value);
    int valueWidth = MeasureText(valueText, 16);
    DrawText(valueText, entity.position.x - valueWidth/2, entity.position.y - 5, 16, BLACK);
}

void drawEconomyStats(const EconomyData& economy, const Vector2& position) {
    int yPos = position.y;
    int spacing = 25;
    
    DrawText(TextFormat("GDP: $%.2fT", economy.gdp / 1000.0f), position.x, yPos, 20, WHITE);
    yPos += spacing;
    
    DrawText(TextFormat("Population: %.2fM", economy.population), position.x, yPos, 20, WHITE);
    yPos += spacing;
    
    DrawText(TextFormat("ESG Rating: %.1f/100", economy.esgRating), position.x, yPos, 20, WHITE);
    yPos += spacing * 2;
    
    DrawText("GDP Components:", position.x, yPos, 18, WHITE);
    yPos += spacing;
    
    DrawText(TextFormat("C: $%.2fB (%.1f%%)", economy.consumerSpending, 
             economy.consumerSpending / economy.gdp * 100), position.x, yPos, 16, LIGHTGRAY);
    yPos += spacing;
    
    DrawText(TextFormat("I: $%.2fB (%.1f%%)", economy.investment,
             economy.investment / economy.gdp * 100), position.x, yPos, 16, LIGHTGRAY);
    yPos += spacing;
    
    DrawText(TextFormat("G: $%.2fB (%.1f%%)", economy.governmentSpending,
             economy.governmentSpending / economy.gdp * 100), position.x, yPos, 16, LIGHTGRAY);
    yPos += spacing;
    
    DrawText(TextFormat("NX: $%.2fB (%.1f%%)", economy.netExports,
             economy.netExports / economy.gdp * 100), position.x, yPos, 16, LIGHTGRAY);
}

// Utility functions
Color getColorBasedOnGrowth(float value, float prevValue) {
    if (value > prevValue * 1.05f) return GREEN;       // Strong growth
    if (value > prevValue * 1.01f) return LIME;        // Moderate growth
    if (value > prevValue * 0.99f) return YELLOW;      // Stagnation
    if (value > prevValue * 0.95f) return ORANGE;      // Moderate decline
    return RED;                                        // Strong decline
}

std::string formatNumber(float number) {
    if (number >= 1000) {
        return TextFormat("%.2fT", number / 1000.0f);
    } else {
        return TextFormat("%.2fB", number);
    }
}

Vector2 getSectorPosition(Vector2 center, SectorType sector) {
    float angle = 0.0f;
    
    switch (sector) {
        case CONSUMER_SPENDING:
            angle = 0.0f;           // Right (0 degrees)
            break;
        case INVESTMENT:
            angle = 2.0f * PI / 3;  // Top left (120 degrees)
            break;
        case GOVERNMENT:
            angle = 4.0f * PI / 3;  // Bottom left (240 degrees)
            break;
        case NET_EXPORTS:
            // Net exports is calculated, not a visible circle
            break;
    }
    
    return {
        center.x + cosf(angle) * SECTOR_DISTANCE,
        center.y + sinf(angle) * SECTOR_DISTANCE
    };
}

CircleEntity createCircleEntity(Vector2 position, float radius, Color color, const std::string& label, float value) {
    CircleEntity entity;
    entity.position = position;
    entity.radius = radius;
    entity.color = color;
    entity.label = label;
    entity.value = value;
    return entity;
}