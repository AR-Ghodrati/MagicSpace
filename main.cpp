#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <vector>


#define PI  3.14159265358979323846
#define  T_PI   PI*2
#define GRAVITY 1.5f
#define GLEW_STATIC

using namespace std;

enum LvlEnum{
    MAIN_MENU,GAME_LVL
};
enum ShapeType{
    CUBE,SPHERE
};


class Color{
public:
    Color() {

    }

    Color(float r, float g, float b){
        this->r=r;
        this->g=g;
        this->b=b;
    }
    float r,g,b;
};

class Shape{
public:
    Shape(float x, float y, float z,float r, float g, float b,bool Target,ShapeType type, float Size){
        this->x=x;
        this->y=y;
        this->z=z;
        this->color=Color( r,  g,  b);
        this->Target=Target;
        this->type=type;
        this->Size=Size;
    }
    ShapeType type;
    Color color = Color(0, 0, 0);
    float x,y,z;
    float Size;
    bool Target;
};

class Player{
public:
    float x;
    float y=0.5f;
    float z=-1.0f;
    float MoveStep=0.15f;
    bool IsGameOver;
    int Score;
};

LvlEnum Lvl;
Player player;
Player LastStatus;
vector<Shape>cubeVector;


float CubeRaito=0.03f;
int CubeCount=300;

bool IsPressed,First;
int windowWidth,windowHeight;

// variables to compute frames per second
int frame;
long time, timebase;
char s[50];

GLuint LoadTexture( const char * filename )
{

    GLuint texture;

    int width, height;

    unsigned char * data;

    FILE * file;

    file = fopen( filename, "rb" );

    if ( file == nullptr) return 0;
    width = 1024;
    height = 512;
    data = (unsigned char *)malloc(static_cast<size_t>(width * height * 3));
    //int size = fseek(file,);
    fread(data, static_cast<size_t>(width * height * 3), 1, file );
    fclose( file );

    for(int i = 0; i < width * height ; ++i)
    {
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];

        data[index] = R;
        data[index+2] = B;

    }


    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );


    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
    free( data );

    return texture;
}

float getXPixel(int Pix){
    return static_cast<float>((float)Pix / ((float)windowWidth / 2) - 1.0);
}
float getYPixel(int Pix){
    return ((float)-Pix/((float)windowHeight/2) + 1);

}

void DrawShape(Shape shape){
    glColor3f(shape.color.r, shape.color.g, shape.color.b);
    if(!shape.Target)
        glutWireCube(shape.Size);
    else{
        if(shape.type == CUBE)
            glutSolidCube(shape.Size);
        else if(shape.type == SPHERE) glutSolidSphere(shape.Size,20,20);
    }
    glFlush();

}

void setOrthographicProjection() {

    // switch to projection mode
    glMatrixMode(GL_PROJECTION);

    // save previous matrix which contains the
    //settings for the perspective projection
    glPushMatrix();

    // reset matrix
    glLoadIdentity();

    // set a 2D orthographic projection
    //gluOrtho2D(0, windowWidth, windowHeight, 0);

    // switch back to modelview mode
    glMatrixMode(GL_MODELVIEW);
}

void restorePerspectiveProjection() {

    glMatrixMode(GL_PROJECTION);
    // restore previous projection matrix
    glPopMatrix();

    // get back to modelview mode
    glMatrixMode(GL_MODELVIEW);
}

void renderSpacedBitmapString(

        float x,
        float y,
        float spacing,
        void *font,
        const string s) {

    char c;
    float x1= x;



    for (char i:s) {
        glRasterPos2f(x1,y);
        glutBitmapCharacter(font,i);
        x1 +=spacing;
    }
}


void Menu(){

    if(player.z<-300)
        player.z=-1;

    player.z-=0.1f;
    gluLookAt(player.x, player.y, player.z,
              player.x, player.y, INT_MIN,
              0.0f, 1.0f, 0.0f);



    int cubeCounter = 0;
    Color temp;
    for (auto &Wall_L_cube : cubeVector) {
        if (!Wall_L_cube.Target) {
            if (cubeCounter % 3 == 0)
                temp = Color(1, 0.5f, 0.2f);
            else if (cubeCounter % 2 == 0)
                temp = Color(0.5f, 1, 0.2f);
            else if (cubeCounter % 5 == 0)
                temp = Color(0.5f, 0, 1);
            else if (cubeCounter % 7 == 0)
                temp = Color(0.5f, 0.5f, 1);
            else
                temp = Color(0.5f, 0.5f, 0.5f);


            Wall_L_cube.color = temp;
            cubeCounter++;
        }
    }

    int Counter = 0;

    for (Shape cube:cubeVector) {
        if (cube.z < player.z && !cube.Target) { // Avoid To Draw Before
            glPushMatrix();
            glTranslatef(cube.x, cube.y, cube.z);
            DrawShape(cube);
            glPopMatrix();
            Counter++;
        }
    }

    setOrthographicProjection();

    glPushMatrix();
    glLoadIdentity();


    glColor3f(1,0,0);

    renderSpacedBitmapString(-0.4f, 0.5f,0.08f , GLUT_BITMAP_TIMES_ROMAN_24,
                             const_cast<char *>("Magic Space"));
    renderSpacedBitmapString(0.2f, 0.4f,0.02f , GLUT_BITMAP_HELVETICA_12,
                             const_cast<char *>("By Alireza Ghodrati"));


    // Draw Start BTN
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glVertex2f(-0.3f,-0.1f);
    glVertex2f(0.3f,-0.1f);
    glVertex2f(0.3f,-0.3f);
    glVertex2f(-0.3f,-0.3f);

    glEnd();

    glColor3f(0,0,0);
    renderSpacedBitmapString(-0.23f, -0.22f , 0.04f, GLUT_BITMAP_HELVETICA_18,
                             const_cast<char *>("Start Game!!"));

    // End

    // Draw Quit BTN
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glVertex2f(-0.3f,-0.4f);
    glVertex2f(0.3f,-0.4f);
    glVertex2f(0.3f,-0.6f);
    glVertex2f(-0.3f,-0.6f);

    glEnd();


    glColor3f(0,0,0);
    renderSpacedBitmapString(-0.08f, -0.53f , 0.04f, GLUT_BITMAP_HELVETICA_18,
                             const_cast<char *>("Quit"));

    // End

    glPopMatrix();

    restorePerspectiveProjection();


}

void DrawScore(){



    glPushMatrix();
    glLoadIdentity();
    setOrthographicProjection();

    glColor3f(1,1,1);
    renderSpacedBitmapString(-0.95f, 0.9f,0.03f , GLUT_BITMAP_HELVETICA_18,
                             "Score Is:"+to_string(player.Score));


    glPopMatrix();
    restorePerspectiveProjection();




}

void DrawFPS(){
    // Code to compute frames per second
    frame++;

    time=glutGet(GLUT_ELAPSED_TIME);

    if (time - timebase > 1000) {
        sprintf(s,"FPS:%4.2f",
                frame*1000.0/(time-timebase));
        timebase = time;
        frame = 0;

        if(!player.IsGameOver)
            player.Score+=100;
    }


    // Code to display a string (fps) with bitmap fonts
    setOrthographicProjection();

    glPushMatrix();
    glLoadIdentity();
    renderSpacedBitmapString(0.7f, 0.9f,0.03f , GLUT_BITMAP_HELVETICA_18,
                             s);

    renderSpacedBitmapString(0,0,0,GLUT_BITMAP_HELVETICA_18,"X");
    glPopMatrix();

    restorePerspectiveProjection();
}

void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;
    auto ratio = static_cast<float>(1.0 * w / h);

    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);

    // Reset Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set the correct perspective.
    gluPerspective(45,ratio,1,1000);

    // Get Back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}

bool inLvl(){
    for (Shape c : cubeVector)
        if(c.z < player.z) // Avoid To Check Before
            return player.y < 1.5f && player.y > -1.5f
                   && player.x > -1.1f && player.x < 1.1f;
    return false;
}

bool IsHited(){

    for (Shape c : cubeVector)
        if(c.z < player.z && c.Target) // Avoid To Check Before && Target
            return player.y <= (c.y+c.Size+0.1f) && player.y >= (c.y-c.Size-0.1f)
                   && player.x >= (c.x-c.Size-0.1f) && player.x <= (c.x+c.Size+0.1f)
                   && (player.z- (c.z+c.Size)) <= 0.05f;
    return false;
}

void DrawGameOver(){
    glPushMatrix();
    glLoadIdentity();
    setOrthographicProjection();

    glColor3f(1,1,1);
    renderSpacedBitmapString(-0.5f,0,0.1f,GLUT_BITMAP_TIMES_ROMAN_24,
                             "Game Over!!");

    glPopMatrix();
    restorePerspectiveProjection();

}

void LVL1(){

    if(player.z<-300)
        player.z=-1;

    if(!IsHited() && !player.IsGameOver)
        player.z-=0.2f;
    else {
        player.IsGameOver= true;
        DrawGameOver();
    }


    // Set the camera
    gluLookAt(player.x, player.y, player.z,
              player.x, player.y, INT_MIN,
              0.0f, 1.0f, 0.0f);






    int cubeCounter = 0;
    Color temp;
    for (auto &Wall_L_cube : cubeVector) {
        if (!Wall_L_cube.Target) {
            if (cubeCounter % 3 == 0)
                temp = Color(1, 0.5f, 0.2f);
            else if (cubeCounter % 2 == 0)
                temp = Color(0.5f, 1, 0.2f);
            else if (cubeCounter % 5 == 0)
                temp = Color(0.5f, 0, 1);
            else if (cubeCounter % 7 == 0)
                temp = Color(0.5f, 0.5f, 1);
            else
                temp = Color(0.5f, 0.5f, 0.5f);


            Wall_L_cube.color = temp;
            cubeCounter++;
        }
    }

    int Counter = 0;

    for (Shape cube:cubeVector) {
        if (cube.z < player.z) { // Avoid To Draw Before
            glPushMatrix();
            glTranslatef(cube.x, cube.y, cube.z);
            DrawShape(cube);
            glPopMatrix();
            Counter++;
        }
    }


    DrawScore();
    DrawFPS();
}

void renderScene() {
// Clear Color and Depth Buffers

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Reset transformations
    glLoadIdentity();


    switch (Lvl){
        case MAIN_MENU:Menu();
            break;
        case GAME_LVL:LVL1();
            break;
        default:
            break;
    }


    glutSwapBuffers();
}


void pressKey(int key, int xx, int yy) {
    if(inLvl()) {
        LastStatus=player;

        switch (key) {

            case GLUT_KEY_UP :
                LastStatus = player;
                IsPressed = true;
                player.y += player.MoveStep;
                break;
            case GLUT_KEY_DOWN :
                IsPressed = true;
                player.y -= player.MoveStep;
                break;

            case GLUT_KEY_RIGHT:
                IsPressed = true;
                player.x += player.MoveStep;
                break;
            case GLUT_KEY_LEFT:
                IsPressed = true;
                player.x -= player.MoveStep;
                break;

            default:
                break;
        }
    } else {
        player.y = LastStatus.y;
        player.x = LastStatus.x;
    }
}

void releaseKey(int key, int x, int y) {

    switch (key) {
        case GLUT_KEY_UP :
        case GLUT_KEY_DOWN :
        case GLUT_KEY_RIGHT:
        case GLUT_KEY_LEFT:
            IsPressed = false ;break;
        default:break;
    }
}

void mouseButton(int button, int state, int x, int y) {

    switch (Lvl){
        case MAIN_MENU:
            if (button == GLUT_LEFT_BUTTON  && state == GLUT_DOWN){
                // Start Game
                //cout<<getXPixel(x)<<"  "<<getYPixel(y)<<endl;
                if(getXPixel(x)>-0.3f && getXPixel(x)<0.3f && getYPixel(y)>-0.3f && getYPixel(y)<-0.1f){
                    cout<<"Start Game";
                    glEnable(GL_DEPTH_TEST);
                    Lvl=GAME_LVL;
                } else if(getXPixel(x)>-0.3f && getXPixel(x)<0.3f && getYPixel(y)>-0.6f && getYPixel(y)<-0.4f){
                    exit(0);
                }

            }
            break;

        case GAME_LVL:
            break;
    }

}


void Init(){

    Lvl=MAIN_MENU;

    float counter=0;

    for (int i=0;i<CubeCount;i++) {


        if(i % 9 == 0)
            cubeVector.emplace_back(sin(i), sin(i), counter, 1, 0.2f, 0, true,CUBE,0.5f);

        if(i % 21 == 0)
            cubeVector.emplace_back(sin(i), cos(i), counter, 0, 1, 0, true,SPHERE,0.3f);

        if(i % 37 == 0)
            cubeVector.emplace_back(-cos(i) , sin(i), counter, 0, 0, 1, true,CUBE,0.4f);

        if(i % 13 == 0)
            cubeVector.emplace_back(sin(i), -cos(i), counter, 0.5f, 0, 1, true,SPHERE,0.15f);

        if(i % 47 == 0)
            cubeVector.emplace_back(cos(i), cos(i), counter, 0, 0.5f, 1, true,SPHERE,0.2f);


        cubeVector.emplace_back(0,0,counter,0,0,0, false,CUBE,5.0f);


        counter-=1.5f;
    }

}

int main(int argc, char **argv) {

    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);




    int w = glutGet(GLUT_SCREEN_WIDTH);
    int h = glutGet(GLUT_SCREEN_HEIGHT);
    windowWidth = w*3/4;
    windowHeight = h*2/3;


    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);
    glutCreateWindow("Magic Space By AR.GH");
    Init();
    // register callbacks
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);

    //glutIgnoreKeyRepeat(5);
    glutSpecialFunc(pressKey);
    glutMouseFunc(mouseButton);

    glutSpecialUpFunc(releaseKey);
    // here are the two new functions
    glutReshapeFunc(changeSize);

    glutMainLoop();

    return 1;
}