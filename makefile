CC = gcc

OBJS = tetris.c fundamentals.c SDL2/SDL2_gfxPrimitives.c SDL2/SDL2_framerate.c SDL2/SDL2_rotozoom.c Chipmunk/chipmunk.c Chipmunk/cpArbiter.c Chipmunk/cpArray.c Chipmunk/cpBBTree.c Chipmunk/cpBody.c Chipmunk/cpCollision.c Chipmunk/cpConstraint.c Chipmunk/cpDampedRotarySpring.c Chipmunk/cpDampedSpring.c Chipmunk/cpGearJoint.c Chipmunk/cpGrooveJoint.c Chipmunk/cpHashSet.c Chipmunk/cpHastySpace.c Chipmunk/cpMarch.c Chipmunk/cpPinJoint.c Chipmunk/cpPivotJoint.c Chipmunk/cpPolyline.c Chipmunk/cpPolyShape.c Chipmunk/cpRatchetJoint.c Chipmunk/cpRobust.c Chipmunk/cpRotaryLimitJoint.c Chipmunk/cpShape.c Chipmunk/cpSimpleMotor.c Chipmunk/cpSlideJoint.c Chipmunk/cpSpace.c Chipmunk/cpSpaceComponent.c Chipmunk/cpSpaceDebug.c Chipmunk/cpSpaceHash.c Chipmunk/cpSpaceQuery.c Chipmunk/cpSpaceStep.c Chipmunk/cpSpatialIndex.c Chipmunk/cpSweep1D.c

INCLUDE_PATHS = -IC:/SDL/SDL2-2.0.10/i686-w64-mingw32/include/SDL2
INCLUDE_PATHS += -IC:/SDL/SDL2_image-2.0.5/i686-w64-mingw32/include/SDL2
INCLUDE_PATHS += -IC:/SDL/SDL2_ttf-2.0.15/i686-w64-mingw32/include/SDL2
LIBRARY_PATHS = -LC:/SDL/SDL2-2.0.10/i686-w64-mingw32/lib
LIBRARY_PATHS += -LC:/SDL/SDL2_image-2.0.5/i686-w64-mingw32/lib
LIBRARY_PATHS += -LC:/SDL/SDL2_ttf-2.0.15/i686-w64-mingw32/lib

# -w (suppresses all warnings)
# -Wl,-subsystem (windows gets rid of the console window)
# COMPILER_FLAGS = -w -Wl,-subsystem,windows
# COMPILER_FLAGS = -Wall -Wextra -Werror -O2 -std=c99 -pedantic
COMPILER_FLAGS = -w
#COMPILER_FLAGS = -Wall -g

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

OBJ_NAME = Tetris

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)