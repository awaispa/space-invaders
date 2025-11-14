#include "/opt/homebrew/include/raylib.h"
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

  if (direction == LEFT) {
    ship->position.x -= ship->speed;
  } else if (direction == RIGHT) {
    ship->position.x += ship->speed;
  } else {
    fprintf(stderr, "Error: Direction must be either LEFT or RIGHT\n");
    return ERROR_INCORRECT_ENUM_TYPE;
  }

  return SUCCESS;
}

// Level 1 ship moves only left and right
typedef struct {
  Vector2 position;
  float width;
  float height;
  float speed;
  int health;
  Color color;
  Direction direction;
} EnemyShipLevel1;

EnemyShipLevel1 NewEnemyShipLevel1(Vector2 starting_pos) {
  return (EnemyShipLevel1){.position = starting_pos,
                           .width = 60,
                           .height = 20,
                           .health = 100,
                           .speed = 1,
                           .color = RED};
};

void DrawEnemyShipLevel1(EnemyShipLevel1 *ship) {
  DrawRectangleV(ship->position, (Vector2){ship->width, ship->height},
                 ship->color);
}

void MoveEnemyShipLevel1(EnemyShipLevel1 *ship) {
  ship->position.x += ship->speed * ship->direction;

  if (ship->position.x + ship->width > SCREEN_WIDTH) {
    ship->position.x = SCREEN_WIDTH - ship->width; // Snap to right border
    ship->direction = -1;                          // Change direction to left
  } else if (ship->position.x < 0) {
    ship->position.x = 0; // Snap to left border
    ship->direction = 1;  // Change direction to right
  }
}

Vector2 BottomLeftPosEnemyShipLevel1(EnemyShipLevel1 ship) {
  return (Vector2){ship.position.x, ship.position.y};
}

Vector2 TopLeftPosEnemyShipLevel1(EnemyShipLevel1 ship) {
  return (Vector2){ship.position.x, ship.position.y - ship.height};
}

Vector2 BottomRightPosEnemyShipLevel1(EnemyShipLevel1 ship) {
  return (Vector2){ship.position.x + ship.width, ship.position.y};
}

Vector2 TopRightPosEnemyShipLevel1(EnemyShipLevel1 ship) {
  return (Vector2){ship.position.x + ship.width, ship.position.y - ship.height};
}

#define BULLET_COUNT 10
typedef struct {
  Vector2 position;
  float width;
  float height;
  float speed;
  Color color;
} Bullet;

Bullet bullets[BULLET_COUNT];
int bullet_count = 0;

void DrawBullet(Bullet bullet) {
  DrawCircle(bullet.position.x, bullet.position.y, 3, bullet.color);
}

void MoveBullet(Bullet *bullet) { bullet->position.y -= bullet->speed; }

int main(void) {
  // Initialization

  MainShip main_ship = {
      .position = {(int)(SCREEN_WIDTH / 2), SCREEN_HEIGHT - 100},
      .height = 70,
      .speed = 5.0f,
      .width = 130,
      .color = PINK,
  };

  Bullet consumable_ammo_level_1 = {};

  EnemyShipLevel1 enemy_ship_level_1 = NewEnemyShipLevel1((Vector2){20, 100});
  enemy_ship_level_1.direction = RIGHT;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game Name");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // === DRAW ===
    BeginDrawing();
    ClearBackground(BLUE);
    DrawText(TextFormat("Bullet Count: %d", BULLET_COUNT - bullet_count), 10,
             10, 13, WHITE);
    DrawText(TextFormat("%d", (int)GetTime()), SCREEN_WIDTH - 30, 10, 13,
             WHITE);
    DrawText(TextFormat("Enemy health %d", enemy_ship_level_1.health),
             SCREEN_WIDTH / 2, 10, 13, GREEN);

    DrawMainShip(main_ship);

    for (int i = 0; i < bullet_count; i++) {
      DrawBullet(bullets[i]);
    }

    if (GetTime() > START_TIME_LEVEL_1) {
      DrawEnemyShipLevel1(&enemy_ship_level_1);
    }

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

    if (IsKeyPressed(KEY_SPACE) && bullet_count < BULLET_COUNT) {
      bullets[bullet_count++] =
          (Bullet){.position = {main_ship.position.x + main_ship.width / 2,
                                main_ship.position.y},
                   .height = 10,
                   .width = 4,
                   .speed = 10,
                   .color = GREEN};
    };

    // Collision
    int DAMAGE = 5;
    for (int i = 0; i < bullet_count; i++) {
        Bullet* bullet = &bullet[i];
      MoveBullet(&bullets[i]);

      if (CheckCollisionCircleRec(*&bullets[i].position, *&bullets[i].width,
                                  (Rectangle){
                                      enemy_ship_level_1.position.x,
                                      enemy_ship_level_1.position.y,
                                      enemy_ship_level_1.width,
                                      enemy_ship_level_1.height,
                                  })) {
        enemy_ship_level_1.health -= DAMAGE;

        // TODO: remove bullet from array
        // for (int j = i; j < bullet_count - 1; j++) {
        //   bullets[j] = bullets[j + 1]; // Shift bullet positions left
        // }
        // i--;
      }
    }

    if (GetTime() > START_TIME_LEVEL_1) {
      MoveEnemyShipLevel1(&enemy_ship_level_1);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
