#include "main.hpp"
#include <vector>

int main() {
    // Initialize window
    const int screenWidth = 1000;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Economic Circles Simulation");
    SetTargetFPS(FPS);
    
    // Initialize camera
    CameraControl cameraControl = initCamera();
    
    // Initialize economy data
    EconomyData economy = {
        .gdp = 0.0f,                    // Will be calculated
        .consumerSpending = 1200.0f,    // $1,200B
        .investment = 800.0f,           // $800B
        .governmentSpending = 950.0f,   // $950B
        .netExports = -250.0f,          // -$250B (trade deficit)
        .population = 330.0f,           // 330 million
        .esgRating = 65.0f              // Rating out of 100
    };
    
    // Calculate initial GDP
    economy.gdp = calculateGDP(
        economy.consumerSpending,
        economy.investment,
        economy.governmentSpending,
        economy.netExports
    );
    
    // Previous values for detecting changes
    EconomyData prevEconomy = economy;
    
    // Time tracking
    float elapsedTime = 0.0f;
    const float UPDATE_INTERVAL = 1.0f; // Update economy every second
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Update camera based on input
        cameraControl = updateCamera(cameraControl, true);
        
        // Update time tracking
        float deltaTime = GetFrameTime();
        elapsedTime += deltaTime;
        
        // Periodically update economy
        if (elapsedTime >= UPDATE_INTERVAL) {
            // Store previous values
            prevEconomy = economy;
            
            // Update components with functional approach
            economy.consumerSpending = updateConsumerSpending(economy.population, economy.consumerSpending, elapsedTime);
            economy.investment = updateInvestment(economy.esgRating, economy.investment, elapsedTime);
            economy.governmentSpending = updateGovernmentSpending(economy.population, economy.governmentSpending, elapsedTime);
            economy.netExports = updateNetExports(economy.esgRating, economy.population, economy.netExports, elapsedTime);
            
            // Update GDP (derived from components)
            economy.gdp = calculateGDP(
                economy.consumerSpending,
                economy.investment,
                economy.governmentSpending,
                economy.netExports
            );
            
            // Update other metrics
            economy.esgRating = updateESGRating(economy.esgRating, elapsedTime);
            economy.population = updatePopulation(economy.population, elapsedTime);
            
            // Reset timer
            elapsedTime = 0.0f;
        }
        
        // Create circle entities
        Vector2 center = {0, 0}; // Center of the economic view
        
        // Create GDP central circle
        Color gdpColor = getColorBasedOnGrowth(economy.gdp, prevEconomy.gdp);
        CircleEntity gdpCircle = createCircleEntity(
            center, 
            GDP_CIRCLE_RADIUS+GDP_CIRCLE_RADIUS*((economy.gdp-prevEconomy.gdp)/economy.gdp), 
            gdpColor, 
            "GDP", 
            economy.gdp
        );
        
        // Create sector circles
        std::vector<CircleEntity> sectorCircles;
        
        // Consumer spending circle
        Color cColor = getColorBasedOnGrowth(economy.consumerSpending, prevEconomy.consumerSpending);
        sectorCircles.push_back(createCircleEntity(
            getSectorPosition(center, CONSUMER_SPENDING),
            SECTOR_CIRCLE_RADIUS,
            cColor,
            "Consumer",
            economy.consumerSpending
        ));
        
        // Investment circle
        Color iColor = getColorBasedOnGrowth(economy.investment, prevEconomy.investment);
        sectorCircles.push_back(createCircleEntity(
            getSectorPosition(center, INVESTMENT),
            SECTOR_CIRCLE_RADIUS,
            iColor,
            "Investment",
            economy.investment
        ));
        
        // Government spending circle
        Color gColor = getColorBasedOnGrowth(economy.governmentSpending, prevEconomy.governmentSpending);
        sectorCircles.push_back(createCircleEntity(
            getSectorPosition(center, GOVERNMENT),
            SECTOR_CIRCLE_RADIUS,
            gColor,
            "Government",
            economy.governmentSpending
        ));
        
        // Begin drawing
        BeginDrawing();
        ClearBackground(DARKBLUE);
        
        // Begin 2D camera mode
        BeginMode2D(cameraControl.camera);
        
        // Draw gridlines for reference
        const int gridSize = 100;
        const int gridExtent = 1000; // How far the grid extends
        
        for (int i = -gridExtent; i <= gridExtent; i += gridSize) {
            DrawLine(i, -gridExtent, i, gridExtent, ColorAlpha(GRAY, 0.2f));
            DrawLine(-gridExtent, i, gridExtent, i, ColorAlpha(GRAY, 0.2f));
        }
        
        // Draw GDP center circle
        drawGDPCircle(gdpCircle);
        
        // Draw connecting lines from GDP to sectors
        for (const auto& sector : sectorCircles) {
            DrawLineEx(center, sector.position, 3.0f, ColorAlpha(WHITE, 0.3f));
            drawSectorCircle(sector);
        }
        
        EndMode2D();
        
        // Draw UI elements in screen space
        drawEconomyStats(economy, {10, 10});
        
        // Draw instructions
        DrawText("Controls: WASD to move, Mouse wheel to zoom, Right-click drag to pan", 10, screenHeight - 30, 16, WHITE);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}