#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

typedef enum GameScreen
{
    TUTORIAL,
    GAMEPLAY,
    GAME_OVER
} GameScreen;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Grow Ghost");
    InitAudioDevice();

    Music backgroundMusic = LoadMusicStream("assets/Spooktober.wav");
    PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, 0.3);
    GameScreen currentScreen = TUTORIAL;
    Sound collectSounds[3];
    for (int i = 0; i < 3; ++i)
    {
        collectSounds[i] = LoadSound(TextFormat("assets/collect%i.wav", i + 1));
    }
    Sound deathSound = LoadSound("assets/death.wav");
    // Main menu assets

    Texture2D mouseTexture = LoadTexture("assets/mouse.png");
    Texture2D qButtonTexture = LoadTexture("assets/qbutton.png");
    Texture2D eButtonTexture = LoadTexture("assets/ebutton.png");

    // Player variables
    Texture2D playerTexture = LoadTexture("assets/ghost.png");
    float const minSpeed = 50;
    float const maxSpeed = 400;
    float const minSize = 0.1;
    float const maxSize = 1.5;
    float const growSpeed = 0.8;
    float const speedChange = 7;

    float speed = maxSpeed;
    float playerSize = 1.0;

    const float colliderGrowSpeedFactor = 30;
    float collisionRadius = 30;
    float playerRotation = 0;
    Vector2 playerVelocity = {};
    Vector2 playerPosition = {screenWidth/2, screenHeight/2};
    Vector2 mousePosition = {};
    // Enemies variables
    const float skullRotationSpeed = 100;
    Texture2D skullTexture = LoadTexture("assets/skull.png");
    Vector2 skullCenters[10];

    for (int i = 0; i < 10; ++i)
    {
        skullCenters[i].x = (i * 80) + 40;
        skullCenters[i].y = GetRandomValue(-400, -20);
    }

    float centralSkullRotation = 0;
    float skullSize = 40;
    float skullSpeed = 40;
    
    // Candy variables
    Texture2D candyTexture = LoadTexture("assets/candy.png");
    Vector2 candyPosition = (Vector2){GetRandomValue(100,700), GetRandomValue(100, 350)};
    float candyRadius = 30;
    Color candyColor = ColorFromHSV(GetRandomValue(0,300), 100,100);
    int candyScore = 0;

    //Particle variables
    const int minParticles = 3;
    const int maxParticles = 10;
    int particleNum = 10;
    Vector2 particlePositions[maxParticles];

    for (int i = 0; i < particleNum; ++i)
        particlePositions[i] = candyPosition;

    Vector2 particleVelocities[particleNum];

    for (int i = 0; i < maxParticles; ++i)
        particleVelocities[i] = Vector2Zero();
    
    Color particleColor = candyColor;
    float particleSpeed = 70;
    float particleDecaySpeed = 40;
    float particleRadius = 20;

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC keys
    {
        UpdateMusicStream(backgroundMusic);
        // Update
        //----------------------------------------------------------------------------------
        switch (currentScreen)
        {
        case TUTORIAL:
            //Update
            //----------------------------------------------------------------------------------
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
            {
                    for (int i = 0; i < 10; ++i)
                    {
                        skullCenters[i].x = (i * 80) + 40;
                        skullCenters[i].y = GetRandomValue(-400, -20);
                    }
                    candyPosition = (Vector2){GetRandomValue(100,700), GetRandomValue(100, 350)};
                    playerPosition = (Vector2){screenWidth/2, screenHeight/2};
                
                    for (int i = 0; i < particleNum; ++i)
                        particlePositions[i] = candyPosition;
                    
                    for (int i = 0; i < maxParticles; ++i)
                        particleVelocities[i] = Vector2Zero();
                    candyScore = 0;

                currentScreen = GAMEPLAY;
                skullSpeed = 40;
            }
            //----------------------------------------------------------------------------------

            //Draw
            //----------------------------------------------------------------------------------
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTexture(mouseTexture, 74, 130, LIGHTGRAY);
                DrawTexture(qButtonTexture, 600, 250, LIGHTGRAY);
                DrawTexture(eButtonTexture, 700, 250, LIGHTGRAY);

                DrawText("lonelyfloat and mycodeisonfire present", screenWidth/2 - 200, 0, 20, PURPLE);
                DrawText(TextFormat("LEFT CLICK AND DRAG \nthe ghost around to \ncollect candies!"), 10, 245, 20, LIGHTGRAY);
                DrawText(TextFormat("Use Q and E to \nenlarge/shrink \nyour spirit!"), 600, 145, 20, LIGHTGRAY);
                DrawText("Be careful, \neverything has a \ncost.", 600, 350, 20, RED);
                DrawText("Grow Ghost", screenWidth/2 - 125, 50, 50, WHITE);
                DrawText(" Left Click to start!", screenWidth/2 - 140, 250, 30, WHITE);
            EndDrawing();
            //----------------------------------------------------------------------------------
            break;
        case GAMEPLAY: 
            // Particles
            //----------------------------------------------------------------------------------
            if(CheckCollisionCircles(playerPosition, collisionRadius, candyPosition, candyRadius))
            {
                PlaySound(collectSounds[GetRandomValue(0,2)]);
                //Particles
                particleNum = GetRandomValue(minParticles, maxParticles);
                particleColor = candyColor;
                particleRadius = 20;

                for (int i = 0; i < particleNum; ++i)
                {
                    particlePositions[i] = candyPosition;
                    particleVelocities[i].x = sin(DEG2RAD * (i * 360/particleNum)) * (particleSpeed * GetFrameTime());
                    particleVelocities[i].y = -cos(DEG2RAD * (i * 360/particleNum)) * (particleSpeed * GetFrameTime());
                }

                candyPosition = (Vector2){GetRandomValue(100,700), GetRandomValue(100, 350)};
                candyColor = ColorFromHSV(GetRandomValue(0,300), 100,100);
                ++candyScore;
                if(candyScore % 10 == 0 && candyScore > 0) skullSpeed += 5;

            }

            for (int i = 0; i < particleNum; ++i)
            {
                particlePositions[i].x += particleVelocities[i].x;
                particlePositions[i].y += particleVelocities[i].y;
            }
            particleRadius -= particleDecaySpeed * GetFrameTime();

            //Skull movement
            //----------------------------------------------------------------------------------
            centralSkullRotation += skullRotationSpeed * GetFrameTime();
            if(centralSkullRotation == 360) centralSkullRotation = 0;
            
            for (int i = 0; i < 10; ++i)
            {
                skullCenters[i].y += skullSpeed * GetFrameTime();
                if(skullCenters[i].y >= 470) skullCenters[i].y = GetRandomValue(-100, -20);
            }
            
            //----------------------------------------------------------------------------------

            // Player movement
            //----------------------------------------------------------------------------------
            mousePosition = GetMousePosition();

            if(!CheckCollisionPointCircle(mousePosition, playerPosition, collisionRadius)) 
                playerVelocity = Vector2Zero();
            if(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointCircle(mousePosition, playerPosition, collisionRadius))
                playerVelocity = (Vector2){mousePosition.x - playerPosition.x, mousePosition.y - playerPosition.y};
            playerPosition.x += speed * (playerVelocity.x * GetFrameTime());
            playerPosition.y += speed * (playerVelocity.y * GetFrameTime());
            
            for (int i = 0; i < 10; ++i)
            {
                if(CheckCollisionCircles(playerPosition, collisionRadius, skullCenters[i], skullSize - 10)) 
                {
                    PlaySound(deathSound);
                    currentScreen = GAME_OVER;
                }
            }
            
            if(IsKeyDown(KEY_Q)) 
            {
                playerSize += growSpeed * GetFrameTime();
                collisionRadius += growSpeed * colliderGrowSpeedFactor * GetFrameTime();
                speed -= speedChange;
            }
            if(IsKeyDown(KEY_E)) 
            {
                playerSize -= growSpeed * GetFrameTime();
                collisionRadius -= growSpeed * colliderGrowSpeedFactor * GetFrameTime();
                speed += speedChange;
            }
            
            playerRotation = Vector2Angle(playerPosition, candyPosition) + 90;
            collisionRadius = Clamp(collisionRadius, 2, 45);
            playerSize = Clamp(playerSize, minSize, maxSize);
            speed = Clamp(speed, minSpeed, maxSpeed);
            // Candy collection
            //----------------------------------------------------------------------------------
            //----------------------------------------------------------------------------------

            //----------------------------------------------------------------------------------
            
            // Draw
            //----------------------------------------------------------------------------------
            BeginDrawing();
                ClearBackground(BLACK);
                    DrawText(TextFormat("Your candy count: %i", candyScore), screenWidth/2 - 150, screenHeight - 30, 30, PINK);
                    DrawTexturePro(playerTexture, (Rectangle){0,0,playerTexture.width, playerTexture.height}, 
                    (Rectangle){playerPosition.x,playerPosition.y,playerTexture.width * playerSize, playerTexture.height * playerSize},
                    (Vector2){playerTexture.width/2 * playerSize, playerTexture.height/2 * playerSize}, playerRotation, WHITE);
                    //DrawCircleV(playerPosition, collisionRadius, GREEN); // Player hitbox
                    DrawTextureV(candyTexture, (Vector2){candyPosition.x - candyTexture.width/2,candyPosition.y - candyTexture.height/2}, candyColor);
                    for (int i = 0; i < 10; ++i)
                    {
                        DrawTexturePro(skullTexture, (Rectangle){0,0, skullTexture.width, skullTexture.height},
                        (Rectangle){skullCenters[i].x,skullCenters[i].y, skullTexture.width, skullTexture.height},
                        (Vector2){skullTexture.width/2, skullTexture.height/2}, centralSkullRotation + i, RED);
                    }
                    for (int i = 0; i < particleNum; ++i)
                    {
                        if(particleRadius > 0) 
                            DrawCircleV(particlePositions[i], particleRadius, particleColor);
                    }
                    
            EndDrawing();
            //----------------------------------------------------------------------------------

            break;
        case GAME_OVER:
            //Update
            //----------------------------------------------------------------------------------
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
                currentScreen = TUTORIAL;
            //----------------------------------------------------------------------------------

            //Draw
            //----------------------------------------------------------------------------------
            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("GAME OVER!", screenWidth/2 - 90, screenHeight/2 - 15, 30, LIGHTGRAY);
                DrawText("LEFT CLICK TO RETURN TO MENU", screenWidth/2 - 250, screenHeight/2 + 20, 30, LIGHTGRAY);
                DrawText(TextFormat("Your score was: %i", candyScore), screenWidth/2 - 150, screenHeight/2 + 50, 30, PINK);
            EndDrawing();
            //----------------------------------------------------------------------------------
            break;
        default:
            break;
        }

    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(mouseTexture);
    UnloadTexture(qButtonTexture);
    UnloadTexture(eButtonTexture);
    UnloadTexture(playerTexture);
    UnloadTexture(skullTexture);
    UnloadTexture(candyTexture);

    for (int i = 0; i < 3; ++i)
    {
        UnloadSound(collectSounds[i]);
    }
    UnloadSound(deathSound);
    UnloadMusicStream(backgroundMusic);
    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
