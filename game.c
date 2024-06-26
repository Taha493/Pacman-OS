#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

float x = 2.0f, y = 46.0f;
float i;
float PI = 3.1416;
pthread_mutex_t moveInput = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dotCollisionMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t playerMovementMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t scoreMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t playerGhostCollisionMutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t powerPelletMutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t KeyAndPermit = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ghostPellet = PTHREAD_MUTEX_INITIALIZER; 


bool playerGhostCollision = false;
time_t powerPelletEatenTime;
bool GetFast = false;
time_t ghostPelletEatenTime;

float initialPlayerX = 2.0f;
float initialPlayerY = 46.0f;


bool ready = false;
int score = 0;
int lives = 3;
bool dotCollision = false; 

struct Ghosts {
  float posX;
  float posY;
  bool frozen;
  float initx;
  float inity;
  bool key;
  bool permit;
};



struct Ghosts ghosts[4];
 
void initGhosts()
{
   ghosts[0].posX = 20.0f;
  ghosts[0].posY = 91.f;
      ghosts[0].frozen = false;
        ghosts[0].initx = 20.0f;
  ghosts[0].inity = 91.f;
  ghosts[0].key = false;
  ghosts[0].permit = false;
      

  
  
   ghosts[1].posX = 2.0f;
  ghosts[1].posY = 91.f;
      ghosts[1].frozen = false;
             ghosts[1].initx = 2.0f;
  ghosts[1].inity = 91.f;
    ghosts[1].key = false;
  ghosts[1].permit = false;



  
   ghosts[2].posX = 2.0f;
  ghosts[2].posY = -79.f;
      ghosts[2].frozen = false;
         ghosts[2].initx = 2.0f;
  ghosts[2].inity = -79.f;
    ghosts[2].key = false;
  ghosts[2].permit = false;



  
  
   ghosts[3].posX = -20.0f;
  ghosts[3].posY = -79.f;
    ghosts[3].frozen = false;
       ghosts[3].initx = -20.0f;
  ghosts[3].inity = -79.f;
    ghosts[3].key = false;
  ghosts[3].permit = false;

}



// Structure to pass argc and argv
struct ThreadArgs {
    int argc;
    char** argv;
};

int maze[35][30] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1},
    {1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,0,0,1},
    {1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,1},
    {1,0,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,0,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1},
    {1,0,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,1},
    {1,0,1,1,1,0,1,0,1,1,1,1,5,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
    {1,1,1,0,1,1,1,0,1,0,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,1,1,1,0,1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1,1,0,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,1,0,1,1,0,1,1,1},
    {1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,1},
    {1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,1},
    {1,1,1,1,1,1,1,0,1,1,1,0,1,0,0,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,0,1,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,0,1,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,0,1,0,1},
    {1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
    {1,0,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,1,0,1},
    {1,0,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,0,1,0,1},
    {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1,1,0,1,0,1},
    {1,3,0,0,1,0,0,0,0,0,0,0,0,0,1,0,5,0,0,0,0,0,0,0,1,0,0,0,3,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


bool collisionWithDot(float playerX, float playerY) {
    int col = (int)(playerX + 97) / 6;
    int row = (int)(91 - playerY) / 5;
    return maze[row][col] == 0; // Check if the player is on a yellow dot
}



void* moveGhost(void* arg) {
    struct Ghosts* ghost = (struct Ghosts*)arg;
    int prevDirection = -1; // Initialize previous direction to an invalid value
    int direction = 0; // Start moving right
    srand(time(NULL)); // Seed the random number generator
    
    while (1) {
        pthread_mutex_lock(&KeyAndPermit);
        if (!ghost->permit && !ghost->key) {
            pthread_mutex_unlock(&KeyAndPermit);
            sleep(1); // Adjust sleep duration as needed
            pthread_mutex_lock(&KeyAndPermit);
            ghost->key = true;
            ghost->permit = true;
            pthread_mutex_unlock(&KeyAndPermit);
            sleep(4); // Adjust sleep duration as needed
        }
        pthread_mutex_unlock(&KeyAndPermit);
        
        float nextX = ghost->posX;
        float nextY = ghost->posY;

        if (!ghost->frozen && ghost->key && ghost->permit) {
            
            switch (direction) {
                case 0: nextX += 6; break; // Right
                case 1: nextY += 5; break; // Up
                case 2: nextX -= 6; break; // Left
                case 3: nextY -= 5; break; // Down
            }

            // Calculate indices for next position
            int nextRowIndex = (int)(91 - nextY) / 5;
            int nextColIndex = (int)(nextX + 97) / 6;

            // Check if the next position is within the maze boundaries and is empty
            if (nextRowIndex >= 0 && nextRowIndex < 35 && nextColIndex >= 0 && nextColIndex < 30) {
                if (maze[nextRowIndex][nextColIndex] == 0 || maze[nextRowIndex][nextColIndex] == 2 || maze[nextRowIndex][nextColIndex] == 5) {
                    ghost->posX = nextX;
                    ghost->posY = nextY;
                    
                    if(maze[nextRowIndex][nextColIndex] ==5)
                    {
                    pthread_mutex_lock(&ghostPellet);
                        maze[nextRowIndex][nextColIndex] = 0;
                        ghostPelletEatenTime = time(NULL);
                        GetFast = true;
                        pthread_mutex_unlock(&ghostPellet);
                        
                    }
                } else {
                    int newDirection;
                    do {
                        newDirection = rand() % 4;
                    } while (newDirection == prevDirection);
                    direction = newDirection;
                }
            } else {
                int newDirection;
                do {
                    newDirection = rand() % 4;
                } while (newDirection == prevDirection);
                direction = newDirection;
            }
        } else {
            float distance = sqrt(pow((x - ghost->posX), 2) + pow((y - ghost->posY), 2));
            if (distance < 4.0f) { 
                ghost->posX = ghost->initx;
                ghost->posY = ghost->inity;
            }
        }

        prevDirection = direction;

      if (GetFast) {
            usleep(10000); // 50 milliseconds
        } else {
            usleep(100000); 
        }

        if (!ghost->frozen) {
            for (int i = 0; i < 4; i++) {
                float distance = sqrt(pow((x - ghosts[i].posX), 2) + pow((y - ghosts[i].posY), 2));
                if (distance < 4.0f) { 
                    playerGhostCollision = true;
                    break; 
                }
            }
        }

       
        glutPostRedisplay();
    }
    return NULL;
}




void graphics(void) {
 int row, col;

    glClear(GL_COLOR_BUFFER_BIT);
    
    // Render maze
    for (row = 0; row < 35; row++) {
        for (col = 0; col < 30; col++) {
            if (maze[row][col] == 1) {
                glColor3ub(255, 165, 0); 
            } else {
                glColor3ub(0,0,0);
            }
            
           
            glBegin(GL_QUADS);
            glVertex2f(-97 + col * 6, 95 - row * 5); // Top-left corner
            glVertex2f(-92 + (col + 1) * 6, 95 - row * 5); // Top-right corner
            glVertex2f(-92 + (col + 1) * 6, 90 - (row + 1) * 5); // Bottom-right corner
            glVertex2f(-97 + col * 6, 90 - (row + 1) * 5); // Bottom-left corner
            glEnd();
            
            
            if (maze[row][col] == 0) {
                glColor3ub(255, 255, 0); // Yellow color for dots
                glBegin(GL_TRIANGLE_FAN);
                GLfloat radius = 0.5f;
                GLfloat centerX = -97 + col * 6 + 3; // Calculate center X position of the box
                GLfloat centerY = 95 - row * 5 - 2.5; // Calculate center Y position of the box
                glVertex2f(centerX, centerY); // Center of the box
                int i;
                int triangleAmount = 40;
                GLfloat twicePi = 2.0 * PI;
                for (i = 0; i <= triangleAmount; i++) {
                    glVertex2f(
                        centerX + (radius * cos(i * twicePi / triangleAmount)),
                        centerY + (radius * sin(i * twicePi / triangleAmount)));
                }
                glEnd();
            }
            else if(maze[row][col] == 3)
            {
                glColor3ub(255, 100, 100); 
                glBegin(GL_TRIANGLE_FAN);
                GLfloat radius = 2.0f;
                GLfloat centerX = -97 + col * 6 + 3; // Calculate center X position of the box
                GLfloat centerY = 95 - row * 5 - 2.5; // Calculate center Y position of the box
                glVertex2f(centerX, centerY); // Center of the box
                int i;
                int triangleAmount = 40;
                GLfloat twicePi = 2.0 * PI;
                for (i = 0; i <= triangleAmount; i++) {
                    glVertex2f(
                        centerX + (radius * cos(i * twicePi / triangleAmount)),
                        centerY + (radius * sin(i * twicePi / triangleAmount)));
                }
                glEnd();
            }
            else if(maze[row][col] == 5)
            {
               glColor3ub(160, 32, 140); 
                glBegin(GL_TRIANGLE_FAN);
                GLfloat radius = 2.0f;
                GLfloat centerX = -97 + col * 6 + 3; // Calculate center X position of the box
                GLfloat centerY = 95 - row * 5 - 2.5; // Calculate center Y position of the box
                glVertex2f(centerX, centerY); // Center of the box
                int i;
                int triangleAmount = 40;
                GLfloat twicePi = 2.0 * PI;
                for (i = 0; i <= triangleAmount; i++) {
                    glVertex2f(
                        centerX + (radius * cos(i * twicePi / triangleAmount)),
                        centerY + (radius * sin(i * twicePi / triangleAmount)));
                }
                glEnd();
            }
        }
    }
    
    
    int i;
    int triangleAmount = 40;
    GLfloat twicePi = 2.0 * PI;
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(255, 255, 0);
    GLfloat radius = 2.0f;
    glVertex2f(x, y);
    for (i = 0; i <= triangleAmount; i++) {
        glVertex2f(
            x + (radius * cos(i * twicePi / triangleAmount)),
            y + (radius * sin(i * twicePi / triangleAmount)));
    }
    glEnd();
    
    for (int i = 0; i < 4; i++) {
        
      if (difftime(time(NULL), powerPelletEatenTime) < 5) {
          
            glColor3ub(0, 0, 255);
            ghosts[i].frozen = true;
        } else {
           
            glColor3ub(255, 0, 0);
            ghosts[i].frozen = false;
        }
        glBegin(GL_TRIANGLE_FAN);
        GLfloat radius = 2.0f;
        GLfloat centerX = ghosts[i].posX;
        GLfloat centerY = ghosts[i].posY;
        glVertex2f(centerX, centerY);
        for (int j = 0; j <= triangleAmount; j++) {
            glVertex2f(
                centerX + (radius * cos(j * twicePi / triangleAmount)),
                centerY + (radius * sin(j * twicePi / triangleAmount)));
        }
        glEnd();
    }

    
 
    unsigned char text1[] = "Score: ";
    int w;
    w = glutBitmapLength(GLUT_BITMAP_9_BY_15, text1);
    glRasterPos2f(-97, 97);
    glColor3ub(255, 255, 0);
    int len = strlen(text1);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text1[i]);
    }
    
    char scoreStr[10];
    sprintf(scoreStr, "%d", score);
    len = strlen(scoreStr);
    glRasterPos2f(-80, 97); 
    glColor3ub(255, 255, 0);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, scoreStr[i]);
    }
    
    //displays lives
    unsigned char text2[] = "Lives: ";
    int w1;
    w1 = glutBitmapLength(GLUT_BITMAP_9_BY_15, text2);
    glColor3ub(255, 255, 0);
    len = strlen(text2);
    glRasterPos2f(80, 97);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text2[i]);
    }
    
     char liveStr[10];
    sprintf(liveStr, "%d", lives);
    len = strlen(liveStr);
    glRasterPos2f(97, 97); 
    glColor3ub(255, 255, 0);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, liveStr[i]);
    }
    
    
    //displays coins
    
    glutSwapBuffers();
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glLoadIdentity();
    glOrtho(-100.0, 100.0, -100.0, 100.0, -1.0, 1.0);
}

void keyboard(unsigned char key, int, int) {
    pthread_mutex_lock(&moveInput);
    
    float nextX = x;
    float nextY = y;
    

    if (key == 'w' || key == 'W') {
        nextY += 5.0f;
    } else if (key == 'a' || key == 'A') {
        nextX -= 6.0f;
    } else if (key == 's' || key == 'S') {
        nextY -= 5.0f;
    } else if (key == 'd' || key == 'D') {
        nextX += 6.0f;
    }
    
    
    
    int nextRowIndex = (int)(91 - nextY) / 5;
    int nextColIndex = (int)(nextX + 97) / 6;
    

    if (nextRowIndex >= 0 && nextRowIndex < 35 && nextColIndex >= 0 && nextColIndex < 30) {
    
if (maze[nextRowIndex][nextColIndex] == 0 || maze[nextRowIndex][nextColIndex] == 2 || maze[nextRowIndex][nextColIndex] == 3 ||maze[nextRowIndex][nextColIndex] ==5 ) {
            x = nextX;
            y = nextY;
        }
    }
    
  
     if (maze[nextRowIndex][nextColIndex] == 3 ) {
         pthread_mutex_lock(&powerPelletMutex);
        maze[nextRowIndex][nextColIndex] = 0; 
        powerPelletEatenTime = time(NULL);
        pthread_mutex_unlock(&powerPelletMutex);
    }
    
     if (collisionWithDot(nextX, nextY)) {
        pthread_mutex_lock(&dotCollisionMutex);
        int row = (int)(91 - nextY) / 5;
        int col = (int)(nextX + 97) / 6;
        maze[row][col] = 2; 
        dotCollision = true; 
        pthread_mutex_unlock(&dotCollisionMutex);
    }
    for(int i = 0; i < 4; i++)
    {
    if(ghosts[i].frozen){
     float distance = sqrt(pow((x - ghosts[i].posX), 2) + pow((y - ghosts[i].posY), 2));
            if (distance < 4.0f) { // Adjust threshold as needed
                         pthread_mutex_lock(&powerPelletMutex);
                         ghosts[i].posX = ghosts[i].initx;
                          ghosts[i].posY = ghosts[i].inity;
                          ghosts[i].key = false;
                          ghosts[i].permit = false;
                             pthread_mutex_unlock(&powerPelletMutex);
            }
    }
    }
    pthread_mutex_unlock(&moveInput);
    glutPostRedisplay();
}


void* UIFunc(void* arg) {
    while (1) {
    
        pthread_mutex_lock(&dotCollisionMutex);
        if (dotCollision) {
            pthread_mutex_lock(&scoreMutex);
           
            score++;
           
            dotCollision = false;
            pthread_mutex_unlock(&scoreMutex);
        }
        pthread_mutex_unlock(&dotCollisionMutex);
        
   
        pthread_mutex_lock(&playerGhostCollisionMutex);
        if (playerGhostCollision) {
            lives--;
           
            playerGhostCollision = false;
           
            x = initialPlayerX;
            y = initialPlayerY;
            if (score == 551) {
                printf("PACMAN WON\n");
                exit(0); 
            }
            
            if (lives <= 0) {
                printf("Game Over\n");
                exit(0);
            }
        }
        pthread_mutex_unlock(&playerGhostCollisionMutex);
        
        if (difftime(time(NULL), ghostPelletEatenTime) >= 5) {
            pthread_mutex_lock(&ghostPellet);
            GetFast = false;
            pthread_mutex_unlock(&ghostPellet);
        }
    
       
        glutPostRedisplay();
    }
}


void* GEFunc(void* arg) {
    struct ThreadArgs* args = (struct ThreadArgs*)arg;
    
    glutInit(&args->argc, args->argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(900, 900);
    glutCreateWindow("Pacman");
    init();
    initGhosts();
    glutDisplayFunc(graphics);
    glutKeyboardFunc(keyboard);
    
    pthread_t UIThread;
    pthread_create(&UIThread, NULL , UIFunc, NULL);
    
   
  pthread_t ghostThreads[4];
    for (int i = 0; i < 4; i++) {
        pthread_create(&ghostThreads[i], NULL, moveGhost, (void*)&ghosts[i]);
    }
    
    glutMainLoop();
}

int main(int argc, char** argv) {
    struct ThreadArgs args;
    args.argc = argc;
    args.argv = argv;
    
    pthread_t GEThread;
    pthread_create(&GEThread, NULL , GEFunc, (void*)&args);
    
    while(1) {
        // Provide a proper exit condition or remove this loop
    }
    
    return 0;
}
