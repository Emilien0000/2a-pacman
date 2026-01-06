#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define MAP_WIDTH 25
#define MAP_HEIGHT 13
#define TILE_SIZE 50
//te rex
#define MAX_TREX 3
#define MOVE_DELAY 500



const int SCREEN_WIDTH = 1250;
const int SCREEN_HEIGHT = 650;

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gSol = NULL;      // Mur
SDL_Surface* gmur = NULL;     // Sol
SDL_Surface* gPiece = NULL;   // Essence

int gTileMap[MAP_HEIGHT][MAP_WIDTH] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
    { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 2, 2, 2, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

typedef struct {
    float x, y;
    SDL_Surface* surface;
} GameObject; // On utilise la même structure pour voiture et fantôme

GameObject gCar;
GameObject gfantome;

typedef struct {
    int x, y; // Utilisons des entiers pour la grille
    SDL_Surface* surface;
    Uint32 lastMoveTime; // Pour contrôler la vitesse
} Rex;

Rex gTrexs[MAX_TREX];
int gTrexCount = 0;


//tee rex pr test

void initrex() {
    gTrexCount = 0;
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (gTileMap[i][j] == 2 && gTrexCount < MAX_TREX) {
                gTrexs[gTrexCount].x = j * TILE_SIZE;
                gTrexs[gTrexCount].y = i * TILE_SIZE;
                gTrexs[gTrexCount].surface = gfantome.surface;
                gTrexs[gTrexCount].lastMoveTime = SDL_GetTicks();
                gTrexCount++;

                gTileMap[i][j] = 3;
            }
        }
    }
}




// --- FONCTION DE COLLISION CASE PAR CASE ---
bool canMove(float nX, float nY) {
    int mapX = (int)nX / TILE_SIZE;
    int mapY = (int)nY / TILE_SIZE;

    // Hors limites
    if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) return false;

    // Mur (1)
    if (gTileMap[mapY][mapX] == 1) return false;

    return true;
}

bool init() {
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) return false;
    gWindow = SDL_CreateWindow( "CAR PACMAN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    gScreenSurface = SDL_GetWindowSurface( gWindow );
    return true;
}

void updaterex() {
    Uint32 currentTime = SDL_GetTicks(); // equilibrer PC gamer - Pc normal

    for (int i = 0; i < gTrexCount; i++) {
        if (currentTime > gTrexs[i].lastMoveTime + MOVE_DELAY) {
            int direction = rand() % 4; // 0: haut, 1: bas, 2: gauche, 3: droite
            int nextX = gTrexs[i].x;
            int nextY = gTrexs[i].y;

            switch (direction) {
                case 0: nextY -= TILE_SIZE; break;
                case 1: nextY += TILE_SIZE; break;
                case 2: nextX -= TILE_SIZE; break;
                case 3: nextX += TILE_SIZE; break;
            }

            if (canMove(nextX, nextY)) {
                gTrexs[i].x = nextX;
                gTrexs[i].y = nextY;
            }
            gTrexs[i].lastMoveTime = currentTime;
        }
    }
}


bool loadMedia() {
    gSol = SDL_LoadBMP( "buis_1.bmp" );
    gmur = SDL_LoadBMP( "sol.bmp" );
    gPiece = SDL_LoadBMP( "essence.bmp" );
    gCar.surface = SDL_LoadBMP("voit_1.bmp");
    gfantome.surface = SDL_LoadBMP("rex_1.bmp");

    if( !gSol || !gmur || !gPiece || !gCar.surface || !gfantome.surface ) return false;

    SDL_SetColorKey(gPiece, SDL_TRUE, SDL_MapRGB(gPiece->format, 0, 0, 0));
    SDL_SetColorKey(gCar.surface, SDL_TRUE, SDL_MapRGB(gCar.surface->format, 0, 0, 0));
    SDL_SetColorKey(gfantome.surface, SDL_TRUE, SDL_MapRGB(gfantome.surface->format, 0, 0, 0));

    gCar.x = TILE_SIZE;
    gCar.y = TILE_SIZE;

    return true;
}

void renderMap() {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            SDL_Rect dest = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };

            if (gTileMap[i][j] == 1) {
                SDL_BlitSurface(gSol, NULL, gScreenSurface, &dest);
            }
            else {
                // On dessine le sol (gmur) par défaut pour 0, 2 et 3
                SDL_BlitSurface(gmur, NULL, gScreenSurface, &dest);

                if (gTileMap[i][j] == 0) {
                    SDL_BlitSurface(gPiece, NULL, gScreenSurface, &dest);
                }
                else if (gTileMap[i][j] == 2) {
                    SDL_BlitSurface(gfantome.surface, NULL, gScreenSurface, &dest);
                }
            }
        }
    }
}

int main( int argc, char* args[] ) {
    if( !init() || !loadMedia() ) return 1;

    bool quit = false;
    SDL_Event e;

    while( !quit ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) quit = true;

            else if( e.type == SDL_KEYDOWN ) {
                switch( e.key.keysym.sym ) {
                    case SDLK_UP:
                        if (canMove(gCar.x, gCar.y - TILE_SIZE)) gCar.y -= TILE_SIZE;
                        break;
                    case SDLK_DOWN:
                        if (canMove(gCar.x, gCar.y + TILE_SIZE)) gCar.y += TILE_SIZE;
                        break;
                    case SDLK_LEFT:
                        if (canMove(gCar.x - TILE_SIZE, gCar.y)) gCar.x -= TILE_SIZE;
                        break;
                    case SDLK_RIGHT:
                        if (canMove(gCar.x + TILE_SIZE, gCar.y)) gCar.x += TILE_SIZE;
                        break;
                }
            }
        }

        // --- RAMASSER ESSENCE (On transforme 0 en 3 pour "sol vide") ---
        int gridX = (int)gCar.x / TILE_SIZE;
        int gridY = (int)gCar.y / TILE_SIZE;
        if (gTileMap[gridY][gridX] == 0) gTileMap[gridY][gridX] = 3;

        // --- RENDU ---
        SDL_FillRect(gScreenSurface, NULL, 0);
        renderMap();

        SDL_Rect carDest = { (int)gCar.x, (int)gCar.y, TILE_SIZE, TILE_SIZE };
        SDL_BlitSurface(gCar.surface, NULL, gScreenSurface, &carDest);

        SDL_UpdateWindowSurface(gWindow);
        SDL_Delay(16);
    }
    return 0;
}
