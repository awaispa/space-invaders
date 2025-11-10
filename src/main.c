#include "/opt/homebrew/include/raylib.h"
#include <_stdlib.h>
#include <stdio.h>

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
#define START_TIME_LEVEL_1 3.0
#define START_TIME_LEVEL_2 3.0
#define START_TIME_LEVEL_3 3.0
#define START_TIME_LEVEL_4 3.0
#define LEVEL_DELAY 5.0

typedef enum {
  SUCCESS = 0,
  ERROR_INVALID_ARGUMENT = -1,
  ERROR_FILE_NOT_FOUND = -2,
  ERROR_OUT_OF_MEMORY = -3,
  ERROR_INCORRECT_ENUM_TYPE = -4
} Status;

typedef enum { LEFT = -1, RIGHT = 1 } Direction;

typedef struct {
  Vector2 position;
  float width;
  float height;
  float speed;
  Color color;
} MainShip;

void DrawMainShip(MainShip ship) {
  DrawRectangleV(ship.position, (Vector2){ship.width, ship.height}, ship.color);
}

Status MoveShip(MainShip *ship, Direction direction) {
  if (direction != LEFT && direction != RIGHT) {
    fprintf(stderr, "Error: Direction must be either LEFT or RIGHT\n");
    return ERROR_INCORRECT_ENUM_TYPE;
  }

  if (direction == LEFT) {
    ship->position.x -= ship->speed;
  } else if (direction == RIGHT) {
    ship->position.x += ship->speed;
  }

  return SUCCESS;
}

#define ENEMY_SHIP_LEVEL_1_COUNT 3

// Level 1 ship moves only left and right
typedef struct {
  Vector2 starting_position;
  Vector2 ending_position;
  float width;
  float height;
  float speed;
  Color color;
  Direction direction;
} EnemyShipLevel1;

EnemyShipLevel1 enemy_ships_level_1[ENEMY_SHIP_LEVEL_1_COUNT];
// int enemy_ships_level_1_count = 0;

EnemyShipLevel1 NewEnemyShipLevel1(Vector2 starting_pos, Vector2 ending_pos) {
  return (EnemyShipLevel1){.starting_position = starting_pos,
                           .ending_position = ending_pos,
                           .width = 60,
                           .height = 20,
                           .speed = 8,
                           .color = RED};
};

void DrawEnemyShipLevel1(EnemyShipLevel1 *ship) {

  DrawRectangleV(ship->starting_position, (Vector2){ship->width, ship->height},
                 ship->color);
}

void MoveEnemyShipLevel1(EnemyShipLevel1 *ship) {
  ship->starting_position.x += ship->speed * ship->direction;

  // Boundary checks
  if (ship->starting_position.x + ship->width > SCREEN_WIDTH) {
    ship->starting_position.x =
        SCREEN_WIDTH - ship->width; // Snap to right border
    ship->direction = -1;           // Change direction to left
  } else if (ship->starting_position.x < 0) {
    ship->starting_position.x = 0; // Snap to left border
    ship->direction = 1;           // Change direction to right
  }
}

#define MAX_BULLETS 100
typedef struct {
  Vector2 position;
  float width;
  float height;
  float speed;
  Color color;
} Bullet;

Bullet bullets[MAX_BULLETS];
int bullet_count = 0;

void DrawBullet(Bullet bullet) {
  DrawRectangleV(bullet.position, (Vector2){bullet.width, bullet.height},
                 bullet.color);
}

void MoveBullet(Bullet *bullet) { bullet->position.y += bullet->speed; }

typedef struct {
  Vector2 position;
  float width;
  float height;
  float speed;
  Color color;
} Consumable_Ammo;

int main(void) {
  // Initialization

  MainShip main_ship = {
      .position = {(int)(SCREEN_WIDTH / 2),
                   SCREEN_HEIGHT - main_ship.height - 50},
      .height = 70,
      .speed = 5.0f,
      .width = 130,
      .color = PINK,
  };

  enemy_ships_level_1[0] =
      NewEnemyShipLevel1((Vector2){20, 100}, (Vector2){30, 150});
  enemy_ships_level_1[0].direction = RIGHT;
  enemy_ships_level_1[1] =
      NewEnemyShipLevel1((Vector2){25, 110}, (Vector2){35, 160});
  enemy_ships_level_1[1].direction = RIGHT;
  enemy_ships_level_1[2] =
      NewEnemyShipLevel1((Vector2){30, 120}, (Vector2){40, 170});
  enemy_ships_level_1[2].direction = RIGHT;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game Name");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // === UPDATE ===

    if (IsKeyDown(KEY_H) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
      Status status = MoveShip(&main_ship, LEFT);
      if (status != SUCCESS) {
        CloseWindow();
      }
    }
    if (IsKeyDown(KEY_L) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
      Status status = MoveShip(&main_ship, RIGHT);
      if (status != SUCCESS) {
        CloseWindow();
      }
    }

    if (IsKeyPressed(KEY_SPACE)) {
      if (bullet_count < MAX_BULLETS) {
        bullets[bullet_count++] =
            (Bullet){.position = {main_ship.position.x + main_ship.width / 2,
                                  main_ship.position.y},
                     .height = 10,
                     .width = 4,
                     .speed = -10,
                     .color = GREEN};
      };
    }

    // Update bullets
    for (int i = 0; i < bullet_count; i++) {
      MoveBullet(&bullets[i]);
    }

    if (GetTime() > START_TIME_LEVEL_1) {
      for (int i = 0; i < ENEMY_SHIP_LEVEL_1_COUNT; i++) {
        MoveEnemyShipLevel1(&enemy_ships_level_1[i]);
      }
    }
    // === DRAW ===
    BeginDrawing();
    ClearBackground(BLUE);
    DrawText(TextFormat("Bullet Count: %d", MAX_BULLETS - bullet_count), 10, 10,
             13, WHITE);
    DrawText(TextFormat("%d", (int)GetTime()), SCREEN_WIDTH - 30, 10, 13,
             WHITE);

    DrawMainShip(main_ship);

    for (int i = 0; i < bullet_count; i++) {
      DrawBullet(bullets[i]);
    }

    if (GetTime() > START_TIME_LEVEL_1) {
      for (int i = 0; i < ENEMY_SHIP_LEVEL_1_COUNT; i++) {
        DrawEnemyShipLevel1(&enemy_ships_level_1[i]);
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
