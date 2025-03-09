#include "raylib.h"


struct AnimData{
	Rectangle rec;
	Vector2 pos;
	int frame;
	float updateTime;
	float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
	return data.pos.y + data.rec.height >= windowHeight;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
	data.runningTime += deltaTime;

	if (data.runningTime > data.updateTime)
	{
		// reset runningTime
		data.runningTime = 0.0;
		// update animation frame
		data.rec.x = data.frame * data.rec.width;
		data.frame = (data.frame + 1) % maxFrame;
	}
	return data;
}


int main(){
	int windowDimensions[2] = {512, 380};
	InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher");

	int velocity = 0;
	bool isInAir = false;
	const int jump_vel = -600; //pixels/s

	// acceleration due to gravity (pixels/s/s)
	const int gravity = 1'200;


	// scarfy
	Texture2D scarfy = LoadTexture("textures/scarfy.png");
	AnimData scarfyData;
	scarfyData.rec.width = scarfy.width / 6;
	scarfyData.rec.height = scarfy.height;
	scarfyData.rec.x = 0;
	scarfyData.rec.y = 0;
	scarfyData.pos.x = windowDimensions[0] / 2 - scarfyData.rec.width / 2;
	scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
	scarfyData.frame = 0;
	scarfyData.updateTime = 1.0 / 12;
	scarfyData.runningTime = 0.0;

	//nebulae
	const int sizeOfNebulae{6};

	Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
	AnimData nebulae[sizeOfNebulae];

	// nebula x velocity (pixels/sec)
	int nebulaVel = -200;

	for (int i = 0; i < sizeOfNebulae; i++)
	{
		nebulae[i].rec.x = 0.0;
		nebulae[i].rec.y = 0.0;
		nebulae[i].rec.width = nebula.width / 8;
		nebulae[i].rec.height = nebula.height / 8;
		nebulae[i].pos.y = windowDimensions[1] - nebula.height / 8;
		nebulae[i].frame = 0;
		nebulae[i].runningTime = 0.0;
		nebulae[i].updateTime = 1.0 / 16;
		
		nebulae[i].pos.x = windowDimensions[0] + 300 * i;
	}

	float finishLine{nebulae[sizeOfNebulae - 1].pos.x};

	//backrounds

	Texture2D background = LoadTexture("textures/far-buildings.png");
	float bgX{};
	Texture2D midground = LoadTexture("textures/back-buildings.png");
	float mgX{};
	Texture2D foreground = LoadTexture("textures/foreground.png");
	float fgX{};

	bool collision{};

	SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(WHITE);
		
		// delta time = time since last frame
		float dT = GetFrameTime();
		
		// draw the background
		bgX -= 20 * dT;
		if (bgX <= -background.width * 2)
			bgX = 0.0;

		mgX -= 40 * dT;
		if (mgX <= -midground.width * 2)
			mgX = 0.0;

		fgX -= 80 * dT;
		if (fgX <= -foreground.width * 2)
			fgX = 0.0;

		//background
		
		Vector2 bg1Pos{bgX, 0.0};
		DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
		Vector2 bg2Pos{bgX + background.width * 2, 0.0};
		DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);
		
		//midground
		
		Vector2 mg1Pos{mgX, 0.0};
		DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
		Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
		DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

		//foreground
		
		Vector2 fg1Pos{fgX, 0.0};
		DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
		Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
		DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

		//ground check
		if (isOnGround(scarfyData, windowDimensions[1]))
		{
			//on ground
			velocity = 0;
			isInAir = false;
		}
		else {
			//mid-air
			isInAir = true;
			// apply gravity
			velocity += gravity * dT;
		}

		if (IsKeyPressed(KEY_SPACE) && !isInAir)
			velocity += jump_vel;

		//update nebula position
		for (int i = 0; i < sizeOfNebulae; i++)
			nebulae[i].pos.x += nebulaVel * dT;

		// update scarfy position
		scarfyData.pos.y += velocity * dT;
		
		// update finish line position
		finishLine += nebulaVel * dT;

		if(!isInAir)
			scarfyData = updateAnimData(scarfyData, dT, 6);

		for (int i = 0; i < sizeOfNebulae; i++)
			nebulae[i] = updateAnimData(nebulae[i], dT, 8);

		for(AnimData nebula: nebulae)
		{
			float pad{50};
			Rectangle nebRec{nebula.pos.x + pad, nebula.pos.y + pad, nebula.rec.width - 2 * pad, nebula.rec.height - 2 * pad};
			Rectangle scarfyRec{scarfyData.pos.x, scarfyData.pos.y, scarfyData.rec.width, scarfyData.rec.height};
			if(CheckCollisionRecs(nebRec, scarfyRec))
				collision = true;
		}

		if(collision)
			DrawText("Game Over! :(", 0, 0, 50, RED);
			else
			{
				if (finishLine <= scarfyData.pos.x - windowDimensions[1]/2)
					DrawText("You Win! :)", 0, 0, 50, GREEN);
				else
				{
					// draw nebulae
					for (int i = 0; i < sizeOfNebulae; i++)
						DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);

					// draw scarfy
					DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
				}
		}

		EndDrawing();
	}
	UnloadTexture(scarfy);
	UnloadTexture(nebula);
	UnloadTexture(background);
	UnloadTexture(midground);
	UnloadTexture(foreground);
	CloseWindow();
}
