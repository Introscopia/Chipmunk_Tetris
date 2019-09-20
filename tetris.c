/*

*/
#include "fundamentals.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "SDL2/SDL2_gfxPrimitives.h"
#include "SDL2/SDL2_framerate.h"
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include "Chipmunk/chipmunk/chipmunk.h"


cpVect polar_to_cpVect( double A, double D ){
	return cpv( D * cos( A ), D * sin( A ) );
}


bool cpVect_in_SDL_Rect( cpVect v, SDL_Rect r ){
    if( v.x >= r.x && v.x <= r.x+r.w && v.y >= r.y && v.y <= r.y+r.h ) return 1;
    else return 0;
}

typedef struct{
	bool *contact;
	int I;
	int *count;
} blockdata;

enum CollisionTypes { BLOCK, FLOOR };

int contact_presolve(cpArbiter *arb, cpSpace *space, void *data){
	CP_ARBITER_GET_SHAPES(arb, a, b);
    blockdata *dat = (blockdata*)cpShapeGetUserData(a);
    if( dat->I == *(dat->count)-1 ) *(dat->contact) = 1;
    return 1;
}

void new_block( cpSpace *space, cpBody ***block_bodies, cpPolyShape ***block_shapes, int **block_ids, int *N, cpVect BCs [7][4], double L, double M, double MoIs [7], bool *contact ){
	int I = random( 0, 7 );
	//printf( "%d\n", I );
	*block_ids = (int*) realloc( *block_ids, (*N+1)*sizeof(int) );
	(*block_ids)[*N] = I;
	*block_bodies = (cpBody**) realloc( *block_bodies, (*N+1)*sizeof(cpBody*) );
	(*block_bodies)[*N] = cpSpaceAddBody( space, cpBodyNew( M, MoIs[I] ) );
    cpBodySetPosition( (*block_bodies)[*N], cpv( 683, -100 ) );
    cpBodySetVelocity( (*block_bodies)[*N], cpvzero );

    *block_shapes = (cpPolyShape**) realloc( *block_shapes, 4 * (*N+1) * sizeof(cpPolyShape*) );
    int sn = 4 * (*N);
    int en = 4 * (*N+1);
    for (int i = sn; i < en; ++i){
    	(*block_shapes)[i] = cpPolyShapeAlloc();
    	//cpVect v [4] = { BCs[I][i-sn], cpv(BCs[I][i-sn].x, BCs[I][i-sn].y + L), cpv(BCs[I][i-sn].x + L, BCs[I][i-sn].y + L), cpv(BCs[I][i-sn].x + L, BCs[I][i-sn].y) };
    	cpVect v [4] = { BCs[I][i-sn], cpv(BCs[I][i-sn].x+L, BCs[I][i-sn].y), cpv(BCs[I][i-sn].x+L, BCs[I][i-sn].y+L), cpv(BCs[I][i-sn].x, BCs[I][i-sn].y+L) };
	    cpPolyShapeInitRaw( (*block_shapes)[i], (*block_bodies)[*N], 4, v, 0.0 );
	    cpSpaceAddShape( space, (*block_shapes)[i] );
	    cpShapeSetFriction( (*block_shapes)[i], 0.7 );
	    cpShapeSetElasticity( (*block_shapes)[i], 0.025 );
	    cpShapeSetCollisionType( (*block_shapes)[i], BLOCK );
	    blockdata *BD = malloc( sizeof(blockdata) );
	    BD->contact = contact;
	    BD->I = *N;
	    BD->count = N;
	    cpShapeSetUserData( (*block_shapes)[i], (void*)BD );
    }
    *N += 1;
}


int lines_in_a_file( FILE* f ){
    //rewind(f);
    char c = getc(f);
    int lines = 0;
    while( c != EOF ){
        if (c == '\n') lines++;
        c = getc(f);
    }
    return lines;
}

Uint32 time_passed = 0;

TTF_Font *font = 0;


// - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o - o
int main(int argc, char *argv[]){
    srand (time(NULL));

    HWND hwnd_win = GetConsoleWindow();
    //ShowWindow(hwnd_win,SW_HIDE);
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width = 820;
    int height = 480;
    int cx = width / 2;
    int cy = height / 2;
    bool loop = 1;
    cpVect mouse = cpv(0, 0);
    bool up = 0, down = 0, left = 0, right = 0, clock = 0, anticlock = 0;

    Uint32 then, now, frames;
    FPSmanager fpsm;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return 3;
    }
    SDL_MaximizeWindow( window );
    SDL_GetWindowSize( window, &width, &height );

    //SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ); didn't notice any difference!
    
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif

    SDL_initFramerate(&fpsm);
    SDL_setFramerate(&fpsm, 60);

    IMG_Init(IMG_INIT_PNG);

    if(TTF_Init()==-1){
        printf("TTF_Init: %s\n", TTF_GetError());
    }
    SDL_Texture *tetromino_textures [7];// = (SDL_Texture **) malloc( 7 * sizeof(SDL_Texture *) );
    char tetromino_letters [7] = { 'I', 'O', 'L', 'J', 'T', 'S', 'Z' };
    cpVect BTCs [7];// block texture corners
    cpVect BTDs [7];// block texture dimensions
    for (int i = 0; i < 7; ++i){
       char str [64];
       sprintf( str, "Assets/%c.png", tetromino_letters[i] );
       SDL_Surface *surf = IMG_Load( str );
       tetromino_textures[i] = SDL_CreateTextureFromSurface( renderer, surf );
       BTDs[i] = cpv( surf->w, surf->h );
       BTCs[i] = cpv( -surf->w * 0.5, -surf->h * 0.5 );
       SDL_FreeSurface( surf );
    }    
    
    cpSpace *space;
    space = cpSpaceNew();
    cpSpaceSetGravity( space, cpv(0, 100) );
    cpSpaceSetIterations( space, 12 );
    cpSpaceSetSleepTimeThreshold( space, 5 );

    bool contact = 0;
    int latch = 0;
    cpCollisionHandler *block_floor_handler = cpSpaceAddCollisionHandler( space, BLOCK, FLOOR );
    block_floor_handler->preSolveFunc = contact_presolve;
    cpCollisionHandler *block_block_handler = cpSpaceAddCollisionHandler( space, BLOCK, BLOCK );
    block_block_handler->preSolveFunc = contact_presolve;

    double L = 25;
    double L2 = 2*L;
    double L3 = 3*L;
    cpVect Vs [7][8] = { //Vertices
    			/* I */ { cpv(-L,-2*L2), cpv(-L, 2*L2), cpv(L, 2*L2), cpv(L, -2*L2), cpvzero, cpvzero, cpvzero, cpvzero },
    			/* O */	{ cpv(-L2, -L2), cpv(-L2, L2), cpv(L2, L2), cpv(L2, -L2), cpvzero, cpvzero, cpvzero, cpvzero },
    			/* L */	{ cpv(-L2, -L3), cpv(-L2, L3), cpv(L2, L3), cpv(L2, L), cpv(0, L), cpv(0, -L3), cpvzero, cpvzero },
    			/* J */	{ cpv(-L2, L3), cpv(L2, L3), cpv(L2, -L3), cpv(0, -L3), cpv(0, L), cpv(-L2, L), cpvzero, cpvzero },
    			/* T */	{ cpv(-L3, L2), cpv(L3, L2), cpv(L3, 0), cpv(L, 0), cpv(L, -L2), cpv(-L, -L2), cpv(-L, -0), cpv(-L3, 0) },
    			/* S */	{ cpv(-L3, L2), cpv(L, L2), cpv(L, 0), cpv(L3, 0), cpv(L3, -L2), cpv(-L, -L2), cpv(-L, 0), cpv(-L3, 0) },
    			/* Z */	{ cpv(-L3, -L2), cpv(-L3, 0), cpv(-L, 0), cpv(-L, L2), cpv(L3, L2), cpv(L3, 0), cpv(L, 0), cpv(L, -L2) }
    				   };
    int Vns [7] = { 4, 4, 6, 6, 8, 8, 8 };
   	cpVect BCs [7][4] = {//block coordinates (top-left)
   				/* I */ { cpv(-L,-2*L2), cpv(-L, -L2), cpv(-L, 0), cpv(-L, L2) },
    			/* O */	{ cpv(-L2, -L2), cpv(-L2, 0), cpv(0, 0), cpv(0, -L2) },
    			/* L */	{ cpv(-L2, -L3), cpv(-L2, -L), cpv(-L2, L), cpv(0, L) },
    			/* J */	{ cpv(0, -L3), cpv(0, -L), cpv(0, L), cpv(-L2, L) },
    			/* T */	{ cpv(-L, -L2), cpv(-L3, 0), cpv(-L, 0), cpv(L, 0) },
    			/* S */	{ cpv(-L3, 0), cpv(-L, 0), cpv(-L, -L2), cpv(L, -L2) },
    			/* Z */ { cpv(-L3, -L2), cpv(-L, -L2), cpv(-L, 0), cpv(L, 0) }
   						};
   	Uint32 tetromino_colors[7] = { 0xFFFFFF01, 0xFFFE0000, 0xFF01FFFE, 0xFF017DFF, 0xFFFE00FF, 0xFF00FF00, 0xFF0000FE };
    
    double M = 3;
    double MoIs [7];
    cpVect anLbox [4] = { cpv(-L, -L), cpv(L, -L), cpv(L, L), cpv(-L, L) };
    cpVect off = cpv( L, L );
    for( int i = 0; i < 7; ++i ){
    	MoIs[i] = 0;
    	for (int j = 0; j < 4; ++j){
    		MoIs[i] += cpMomentForPoly( M, 4, anLbox, cpvadd( BCs[i][j], off ), 0 );
    	}
    }
    M *= 4;
    cpBody **block_bodies = NULL;//malloc( 100 * sizeof(cpBody*) );
    cpPolyShape **block_shapes = NULL;//malloc( 400 * sizeof(cpPolyShape*) );
    int *block_ids = NULL;
    int count = 0;
    new_block( space, &block_bodies, &block_shapes, &block_ids, &count, BCs, L2, M, MoIs, &contact );

    float wall_friction = 0.8;
    float wall_elasticity = 0.1;

    cpVect vertices [4] = { cpv( 4, 0 ), cpv( 4, height-4 ), cpv( width-4, height-4 ), cpv(width-4, 0) };
    float x, y;
    cpShape *walls [1];
    walls[0] = cpSegmentShapeNew(cpSpaceGetStaticBody(space), vertices[1], vertices[2], 0);
    cpShapeSetFriction(walls[0], wall_friction);
    cpShapeSetElasticity(walls[0], wall_elasticity);
    cpShapeSetCollisionType( walls[0], FLOOR );
    cpSpaceAddShape(space, walls[0]);
    //walls = (cpPolyShape**) malloc( 3 * sizeof(cpPolyShape*) );
    /*
    for( int o = 0; o < 3; o++ ){
        walls[o] = cpSegmentShapeNew(cpSpaceGetStaticBody(space), vertices[o], vertices[o+1], 0);
        cpShapeSetFriction(walls[o], wall_friction);
        cpShapeSetElasticity(walls[o], wall_elasticity);
        cpShapeSetCollisionType( walls[o], FLOOR );
        cpSpaceAddShape(space, walls[o]);
    }*/

    
    cpFloat timeStep = 1.0/40.0;


    puts("<<<Entering Main Loop>>>");
    while ( loop ) {//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% /LOOP %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|||

    	SDL_Event event;
        while( SDL_PollEvent(&event) ){
            switch (event.type) {
                case SDL_QUIT:
                    loop = 0;
                    break;
                case SDL_MOUSEMOTION:
                    mouse = cpv(event.motion.x, event.motion.y);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    
                    break;
                case SDL_MOUSEBUTTONUP:
                    
                    break;
                case SDL_KEYDOWN:
                    //if( event.key.keysym.sym == 'w' || event.key.keysym.sym == SDLK_UP ) up = 1;
                    if( event.key.keysym.sym == 's' || event.key.keysym.sym == SDLK_DOWN ) down = 1;
                    else if( event.key.keysym.sym == 'a' || event.key.keysym.sym == SDLK_LEFT ) left = 1;
                    else if( event.key.keysym.sym == 'd' || event.key.keysym.sym == SDLK_RIGHT ) right = 1;
                    else if( event.key.keysym.sym == 'q' ) anticlock = 1;
                    else if( event.key.keysym.sym == 'e' ) clock = 1;
                    break;
                case SDL_KEYUP:
                    if( event.key.keysym.sym == 'w' || event.key.keysym.sym == SDLK_UP ) up = 1;
                    else if( event.key.keysym.sym == 's' || event.key.keysym.sym == SDLK_DOWN ) down = 0;
                    else if( event.key.keysym.sym == 'a' || event.key.keysym.sym == SDLK_LEFT ) left = 0;
                    else if( event.key.keysym.sym == 'd' || event.key.keysym.sym == SDLK_RIGHT ) right = 0;
                    else if( event.key.keysym.sym == 'q' ) anticlock = 0;
                    else if( event.key.keysym.sym == 'e' ) clock = 0;
                    break;
            }
        }
        //cpBodyApplyForceAtWorldPoint( block_bodies[count-1], cpv( 0, -90 ), cpBodyGetPosition(block_bodies[count-1]));
        float vx = 0;
        float vy = 40;
        if( down ) vy += 120;//cpBodyApplyForceAtWorldPoint( block_bodies[count-1], cpv( 0, 480 ), cpBodyGetPosition(block_bodies[count-1]));
        if( left ) vx -= 80; cpBodyApplyForceAtWorldPoint( block_bodies[count-1], cpv( -2000, 0 ), cpBodyGetPosition(block_bodies[count-1]) );
        if( right ) vx += 80; cpBodyApplyForceAtWorldPoint( block_bodies[count-1], cpv( 2000, 0 ), cpBodyGetPosition(block_bodies[count-1]) );
        cpBodySetVelocity( block_bodies[count-1], cpv(vx, vy)  );
        if( up ){
			cpBodySetAngle( block_bodies[count-1], cpBodyGetAngle(block_bodies[count-1])+HALF_PI );
			cpSpaceReindexShapesForBody( space, block_bodies[count-1] );
			up = 0;
        }
        if( clock ) cpBodySetTorque( block_bodies[count-1], 10000 );
        if( anticlock ) cpBodySetTorque( block_bodies[count-1], -10000 );

		SDL_RenderClear( renderer );
		SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
		SDL_RenderFillRect( renderer, NULL );

		SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );

        for (int i = 0; i < 1; ++i){
            cpVect A = cpSegmentShapeGetA(walls[i]);
            cpVect B = cpSegmentShapeGetB(walls[i]);
            SDL_RenderDrawLine(renderer, A.x, A.y, B.x, B.y);
            //thickLineColor( renderer, A.x, A.y, B.x, B.y, 4, 0xFF000000 );
        }

        for( int i = 0; i < count; ++i ){
			/* block outlines. this was looping up to 4*count
			cpVect v [4];
			for( int j = 0; j < 4; ++j ) v[j] = cpBodyLocalToWorld( block_bodies[(int)floor(i*0.25)], cpPolyShapeGetVert( block_shapes[i], j ) );
			for( int j = 0; j < 3; ++j ) SDL_RenderDrawLineF( renderer, v[j].x, v[j].y, v[j+1].x, v[j+1].y );
			SDL_RenderDrawLine(renderer, v[3].x, v[3].y, v[0].x, v[0].y );
			*/
			float theta = cpBodyGetAngle(block_bodies[i]);
			cpVect corner = cpvadd( cpBodyGetPosition(block_bodies[i]), BTCs[block_ids[i]] );
			SDL_RenderCopyExF( renderer, tetromino_textures[block_ids[i]], NULL, &(SDL_FRect){ corner.x, corner.y, BTDs[block_ids[i]].x, BTDs[block_ids[i]].y },
        				       degrees(theta), NULL, SDL_FLIP_NONE );
    	}

    	if( contact ){
    		new_block( space, &block_bodies, &block_shapes, &block_ids, &count, BCs, L2, M, MoIs, &contact );
    		contact = 0;
    	}

        cpSpaceStep(space, timeStep);

        SDL_RenderPresent(renderer);
        time_passed = SDL_framerateDelay(&fpsm);
    }//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% /LOOP %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%|||

    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}