#include <GLUT/glut.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define X_RESOLUTION 1280
#define Y_RESOLUTION 800

void draw_terrain(void);
void yCoord_generator(void);
float get_random_float(float, float);
void renderInit(void);
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int );
void arrow_keys(int, int, int);

// Grid
const float TRIANGLE_WIDTH  = 1.0f;
const float TRIANGLE_HEIGHT = 1.0f;

const int GRID_ROWS    = 100;
const int GRID_COLUMNS = 100;
const int GRID_WIDTH = GRID_ROWS * TRIANGLE_WIDTH;
const int GRID_HEIGHT = GRID_COLUMNS * TRIANGLE_HEIGHT;

const float Y_MIN   = 0.0f;
const float Y_MAX   = 0.5f;
const float Y_GRADE = 0.9f;

// Camera
const float CAMERA_MOVE_SPEED = 0.3f;
const float CAMERA_ROTATE_X   = 0.3f;
const float CAMERA_ROTATE_Y   = 0.2f;

float camera_eye_x     =  0.0f;
float camera_eye_y     =  1.0f;
float camera_eye_z     =  -5.0f;

float camera_lookAt_x  =  0.0f;
float camera_lookAt_y  =  1.0f;
float camera_lookAt_z  = -6.0f;

float camera_vector_x  =  0.0f;
float camera_vector_y  =  1.0f;
float camera_vector_z  =  0.0f;

float camera_angle_x = 0.0f;
float camera_angle_y = 0.0f;

float x_offset = (GRID_ROWS / 2);
float y[GRID_ROWS][GRID_COLUMNS];

void draw_terrain() {
    glColor3f(1.0f, 1.0f, 1.0f);
    int x, z, i;
    for (x = (0.0f-x_offset); x < (GRID_ROWS-x_offset)-1; x += TRIANGLE_WIDTH) {
        for (z = 0.0f; z < GRID_COLUMNS-1; z += TRIANGLE_HEIGHT) {
            // Left half
            glVertex3f(x,      y[(int)(x+x_offset)][z],    -z);
            glVertex3f(x+TRIANGLE_WIDTH, y[(int)(x+x_offset+1)][z],  -z);
            glVertex3f(x,      y[(int)(x+x_offset)][z+1],  -z-TRIANGLE_HEIGHT);

            // Right half
            glVertex3f(x+TRIANGLE_WIDTH, y[(int)(x+x_offset+1)][z],   -z);
            glVertex3f(x,      y[(int)(x+x_offset)][z+1],   -z-TRIANGLE_HEIGHT);
            glVertex3f(x+TRIANGLE_WIDTH, y[(int)(x+x_offset+1)][z+1], -z-TRIANGLE_HEIGHT);
        }
    }
}

void yCoord_generator() {
    int xy, zy; 
    float xy_min, xy_max;
    float zy_min, zy_max;

    // Generate random y-coordinates
    srand(time(NULL));
    for (int xy = 0; xy < GRID_ROWS; xy++) {
        for (int zy = 0; zy < GRID_COLUMNS; zy++) {
            if (xy > 0) {
                if (zy > 0) {
                    // (x, z) = (>1, >1)
                    float avg = (
                        y[xy-1][zy-1] + y[xy-1][zy] +
                        y[xy][zy-1] + y[xy][zy] + 
                        y[xy+1][zy-1]) / 5;
                    y[xy][zy] = get_random_float(avg - Y_GRADE, avg + Y_GRADE);
                }
                else {
                    // (x, z) = (>1, 0)
                    float avg = (y[xy-1][zy] + y[xy][zy]) / 2;
                    y[xy][zy] = get_random_float(avg - Y_GRADE, avg + Y_GRADE);
                }
            }
            else if (zy > 0) {
                // (x, z) = (0, >0)
                float avg = (y[xy][zy] + y[xy][zy]) / 2;
                y[xy][zy] = get_random_float(avg-Y_GRADE, avg+Y_GRADE);
            }
            else {
                // (x, z) = (0, 0)
                y[xy][zy] = get_random_float(Y_MIN, Y_MAX); 
            }
        }
    }
}

float get_random_float(float min, float max) {
    float random = ((float) rand()) / (float) (RAND_MAX);
    float diff = fabs(max - min);
    random = random * diff + min;
    return (random >= 0) ? random : 0.0f;
}

void init() {
    yCoord_generator();

    // Set the bg-color
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_SMOOTH);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(camera_eye_x,    camera_eye_y,    camera_eye_z,
              camera_lookAt_x, camera_lookAt_y, camera_lookAt_z,
              camera_vector_x, camera_vector_y, camera_vector_z);

    GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
        draw_terrain();
    glEnd();
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 1.0f, 200.0f);
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27:  // Ascii 'esc'
            exit(0);
            break;
        case 113: // Ascii 'q'
            camera_eye_y -= CAMERA_MOVE_SPEED;
            camera_lookAt_y -= CAMERA_MOVE_SPEED;
            break;
        case 119: // Ascii 'w'
            if (camera_eye_z > -GRID_HEIGHT+TRIANGLE_HEIGHT*2) {
                camera_eye_z -= CAMERA_MOVE_SPEED;
                camera_lookAt_z -= CAMERA_MOVE_SPEED;
            }
            break;
        case 101: // Ascii 'e'
            camera_eye_y += CAMERA_MOVE_SPEED;
            camera_lookAt_y += CAMERA_MOVE_SPEED;
            break;
        case 97:  // Ascii 'a'
            if (camera_eye_x > -(GRID_WIDTH/2)) {
                camera_eye_x -= CAMERA_MOVE_SPEED;
                camera_lookAt_x -= CAMERA_MOVE_SPEED;
            }
            break;
        case 115: // Ascii 's'
            if (camera_eye_z < 0.0f) {
                camera_eye_z += CAMERA_MOVE_SPEED;
                camera_lookAt_z += CAMERA_MOVE_SPEED;
            }
            break;
        case 100: // Ascii 'd'
            if (camera_eye_x < (GRID_WIDTH/2)-TRIANGLE_WIDTH) {
                camera_eye_x += CAMERA_MOVE_SPEED;
                camera_lookAt_x += CAMERA_MOVE_SPEED;
            }
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void arrow_keys(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:
            if (camera_angle_y < 2.0f) {
                camera_angle_y += CAMERA_ROTATE_Y;
                camera_lookAt_y += CAMERA_ROTATE_Y;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (camera_angle_x < 2.0f) {
                camera_angle_x += CAMERA_ROTATE_X;
                camera_lookAt_x += CAMERA_ROTATE_X;
            }
            break;
        case GLUT_KEY_DOWN:
            if (camera_angle_y > -2.0f) {
                camera_angle_y -= CAMERA_ROTATE_Y;
                camera_lookAt_y -= CAMERA_ROTATE_Y;
            }
            break;
        case GLUT_KEY_LEFT:
            if (camera_angle_x > -2.0f) {
                camera_angle_x -= CAMERA_ROTATE_X;
                camera_lookAt_x -= CAMERA_ROTATE_X;
            }
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    // GLUT Initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

    // Create the window
    glutInitWindowSize(X_RESOLUTION, Y_RESOLUTION);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-X_RESOLUTION)/2, 
                           (glutGet(GLUT_SCREEN_HEIGHT)-Y_RESOLUTION)/2);
    glutCreateWindow("Project 2");

    // Initialization
    init();

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(arrow_keys);

    // Start the main loop
    glutMainLoop(); 
    return 1;
}
