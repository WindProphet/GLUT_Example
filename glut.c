/**
 * GLUT Example
 * macOS: $ gcc -framework GLUT -framework OpenGL glut.c -o glut_example
 */


#include<stdio.h>
#include<math.h>

// OpenGL Utility Toolkit
#ifdef _WIN32
#ifndef _MSC_VER
#include<windows.h>
#endif
#endif
#ifdef __APPLE__
#include<GLUT/GLUT.h>
#else
#include<GL/GLUT.h>
#endif

// Macros
#define COLOR(r,g,b) glColor3f((GLfloat)(r),(GLfloat)(g),(GLfloat)(b))
#define LINE(x1, y1, z1, x2, y2, z2) do{glBegin(GL_LINES);glVertex3f((x1),(y1),(z1));glVertex3f((x2),(y2),(z2));glEnd();}while(0)
#define PRINT_4x4_MATRIX(m) printf("\n\x1b[31m%9.5f\t%9.5f\t%9.5f\t%9.5f\n\x1b[34m%9.5f\t%9.5f\t%9.5f\t%9.5f\n\x1b[32m%9.5f\t%9.5f\t%9.5f\t%9.5f\n\x1b[35m%9.5f\t%9.5f\t%9.5f\t%9.5f\n\x1b[30;0m",m[0],m[4],m[8],m[12],m[1],m[5],m[9],m[13],m[2],m[6],m[10],m[14],m[3],m[7],m[11],m[15])
#define PRINT_MATRIX(type) do{double _TMP_MAT[16];glGetDoublev(type,_TMP_MAT);PRINT_4x4_MATRIX(_TMP_MAT);}while(0)
#define DRAW_TEXT(str) if(str)for(int i=0;str[i];i++)glutBitmapCharacter((_SET_FONT),str[i])
#define DRAW_PRINTF(format, ...) do{char _BUFFER[256];sprintf(_BUFFER,format,__VA_ARGS__);DRAW_TEXT(_BUFFER);}while(0)
#ifdef __cplusplus
#define MUTSTR(str) const_cast<char *>(str)
#else
#define MUTSTR(str) (str)
#endif
#ifdef _MSC_VER
#define sprintf sprintf_s
#endif

// Properties
#pragma clang diagnostic ignored "-Wdeprecated-declarations" // suppress GLUT deprecate functions
#define _WIDTH  400
#define _HEIGHT 400
#define _SET_FONT GLUT_BITMAP_HELVETICA_12

GLint all_width  = (_WIDTH),
      all_height = (_HEIGHT);

GLfloat dx  = 0,
        dy  = 0,
        wdx = 0,
        wdy = 0,
        s   = 0;

int matrixshow = 0,
    axisshow   = 1;
char* msg = NULL;

#define freeRotateMode 0
#define axisRotateMode 1
int rotateMode = 0;

#define plannerProjection     0
#define perspectiveProjection 1
int projectionMode =          0;

#pragma mark PredefinedObjects

void drawAxis() {
  COLOR(0.4, 0.4, 0.4);
  for (int i=-10; i<=10; i++)
    LINE(-100, 0, i*10, 100, 0, i*10);
  for (int i=-10; i<=10; i++)
    LINE(i*10, 0, -100, i*10, 0, 100);

  glLineWidth(2);
  COLOR(1.0, 0.0, 0.0);
  LINE(-100, 0, 0, 100, 0, 0);
  COLOR(0.0, 0.0, 1.0);
  LINE(0, -100, 0, 0, 100, 0);
  COLOR(0.0, 1.0, 0.0);
  LINE(0, 0, -100, 0, 0, 100);
  glLineWidth(1);
}

double currentMatrix[16];
void printMatrix(int type) {
  double raster[4];
  glGetDoublev(GL_CURRENT_RASTER_POSITION, raster);
  for (int i = 0; i < 4; i++) {
    COLOR(0.5 + 0.5 * (i % 3 == 0), 0.5 + 0.5 * (i == 2), 0.5 + 0.5 * (i % 2));
    for (int j = 0; j < 4; j++) {
      glRasterPos2f(raster[0] + (j)*60, raster[1] + (3-i)*30);
      DRAW_PRINTF("%9.5f", currentMatrix[j*4+i]);
    }
  }
  glRasterPos2f(raster[0], raster[1]);
  COLOR(1, 1, 1);
}

#pragma mark InterfaceEvent

void UserInterface(void) {
  COLOR(1, 1, 1);
  glRasterPos2f(20, 20);
  if (matrixshow) printMatrix(GL_MODELVIEW_MATRIX);
  if (msg) {
    glRasterPos2f(10, all_height - 20);
    COLOR(1, 1, 1);
    DRAW_TEXT(msg);
  }
}

void RenderObjects(void) {
  if (axisshow) {
    // glPushAttrib: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glPushAttrib.xml
    glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    drawAxis();
    glPopAttrib();
  }
  // Material: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
  glPushAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glShadeModel(GL_SMOOTH);
  glTranslatef(0,0,0);
  glutSolidTeapot(50);
  glPopAttrib();
}

void LightMain(void) {
  GLfloat ambientLight[]  = { 0.2f   , 0.2f   , 0.2f   , 1.0f };
  GLfloat diffuseLight[]  = { 0.8f   , 0.8f   , 0.8f   , 1.0f };
  GLfloat specularLight[] = { 1.0f   , 1.0f   , 1.0f   , 1.0f };
  GLfloat lightPos[] =      { 200.0f , 300.0f , 100.0f , 0.0f };

  glEnable(GL_LIGHTING);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  
  glEnable(GL_LIGHT0);
}

#pragma mark LoopEvent

void prepare() {
  rotateMode = axisRotateMode;
}

void display(void) {
  // Clear frame buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  LightMain();
  glViewport(1, 0, all_width, all_height);
  glPopMatrix();
  
  double MVMatrix[16];
  // glGet: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glGet.xml
  glGetDoublev(GL_MODELVIEW_MATRIX,MVMatrix);
  
  // Matrix transformations: https://open.gl/transformations
  // glRotate: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml
  switch (rotateMode) {
    case freeRotateMode:
      // Rotate in any position
      glRotatef(dx, MVMatrix[0], MVMatrix[4], MVMatrix[8]);
      glRotatef(dy, -MVMatrix[1], -MVMatrix[5], -MVMatrix[9]);
      break;
    case axisRotateMode:
      // Horizon to rotate x,z axis
      // Vertical to rotate y axis
      glRotatef(dx, MVMatrix[0], 0, MVMatrix[8]);
      glRotatef(dy, 0, 1, 0);
      break;
    default:
      break;
  }
  
  // Translate only on interface
  glTranslatef(wdx*MVMatrix[0], wdx*MVMatrix[4], wdx*MVMatrix[8]);
  glTranslatef(wdy*MVMatrix[1], wdy*MVMatrix[5], wdy*MVMatrix[9]);

  glScalef(exp(s), exp(s), exp(s));

  dx  = 0;
  dy  = 0;
  wdx = 0;
  wdy = 0;
  s   = 0;

  glGetDoublev(GL_MODELVIEW_MATRIX, currentMatrix);

  glPushMatrix();
  
  RenderObjects();
  
  // Draw User Interface overlay
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();                            // Reset Projection Matrix
  glOrtho(0, all_width, 0, all_height, -1, 1); // Set viewpoint to plain screen
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();                            // Reset ModelView Matrix
  UserInterface();                             // User drawing
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  // Make sure changes appear onscreen
  glutSwapBuffers();
}

void reshape(GLint width, GLint height) {
  all_width  = width;
  all_height = height;
  GLfloat aspect = (GLfloat)width / (GLfloat)height;
  GLfloat nRange = 100.0f;
  glMatrixMode(GL_MODELVIEW);

  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  switch (projectionMode) {
    case plannerProjection:
      // glOrtho: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml
      // Change the view point to fit the window
      if (width <= height) {
        glOrtho(-nRange, nRange, -nRange/aspect, nRange/aspect, -nRange*2, nRange*2);
      }
      else {
        glOrtho(-aspect*nRange, aspect*nRange, -nRange, nRange, -nRange*2, nRange*2);
      }
      break;
    case perspectiveProjection:
      // glFrustum: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml
      if (width <= height) {
        glFrustum(-nRange/1.4, nRange/1.4, -nRange/aspect/1.4, nRange/aspect/1.4, 500 - nRange*2/1.4, 500 + nRange*2/1.4);
      }
      else {
        glFrustum(-aspect*nRange/1.4, aspect*nRange/1.4, -nRange/1.4, nRange/1.4, 500 - nRange*2/1.4, 500 + nRange*2/1.4);
      }
      glTranslatef(0, 0, -500);
      break;
  }
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

#pragma mark Control

int mousestate = 1;
int mousebutton;
int last_x, last_y;

void mouse_button(int button, int state, int x, int y) {
  mousebutton = button;
  mousestate = state;
  last_x = x;
  last_y = y;
}

void mouse_motion(int x, int y) {
  msg = NULL;
  if (!mousestate) {
    switch (mousebutton) {
      case 0:
        dy = last_x - x;
        dx = last_y - y;
        break;
      case 1:
        s = (last_y - y) / 100.0;
        break;
      case 2:
        wdx = -(last_x - x);
        wdy = last_y - y;
        break;
    }
    last_x = x;
    last_y = y;
  }
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
  switch(key) {
    case 'a':
      axisshow = !axisshow;
      msg = axisshow ? MUTSTR("axis On") : MUTSTR("axis Off");
      break;
    case 'm':
      matrixshow = !matrixshow;
      break;
    case 'v':
      rotateMode = !rotateMode;
      msg = rotateMode ? MUTSTR("axisRotateMode") : MUTSTR("freeRotateMode");
      break;
    case 'i':
      msg = MUTSTR("load Identity");
      glPopMatrix();
      glLoadIdentity();
      glPushMatrix();
      break;
    case 'h':
      msg = MUTSTR("Help:"
          "    a: change axis on/off"
          "    m: matrix show /off"
          "    v: rotateMode axis or free"
          "    i: load identity matrix"
          "    p: change projection");
      break;
    case 'p':
      projectionMode = (projectionMode + 1) % 2;
      reshape(all_width, all_height);
      msg = projectionMode ? MUTSTR("perspective") : MUTSTR("planner");
      break;
  }
  glutPostRedisplay();
}

#pragma mark MainLoop

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(
      GLUT_DOUBLE |
      GLUT_RGB    |
      GLUT_DEPTH );
  glutInitWindowSize((_WIDTH), (_HEIGHT));
  glutCreateWindow("GLUT Example");

  // Register callbacks
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse_button);
  glutMotionFunc(mouse_motion);
  glutKeyboardFunc(keyboard);

  prepare();
  glutMainLoop();
  return 0;
}
