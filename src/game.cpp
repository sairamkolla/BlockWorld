#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define TILE_SIDE 4


#define FLOOR_LENGTH 10
#define FLOOR_WIDTH 20
#define TILE_SEPERATION_FACTOR 0.1
#define CAMERA_DIST 100
#define GRAVITY 0.01
#define CAMERA_MAX 5
#define CAMERA_MIN 1

using namespace std;

struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

typedef struct pointer{
  double prev_xpos=0;
  double pres_xpos=0;
  double prev_ypos=0;
  double pres_ypos=0;
  int dx =0;
  int dy=0;
  bool left_is_clicked=0;
  bool right_is_released=0;
  bool left_is_released=0;
  bool right_is_clicked=0;
}pointer;

typedef struct block{
  VAO *object;
  float point[3] = {0,0,0};
  float dim[3] = {TILE_SIDE,TILE_SIDE,TILE_SIDE};
  //float color[6][3] = {{1,0,0},{0,1,0},{0,0,1},{0.49,0.42,0.42},{0,0,0},{0.5,0,0}};
  float color[6][3] = {{0,0,1},{0.49,0.42,0.42},{0,0,1},{0.49,0.42,0.42},{0,0,0},{0.5,0,0}};
  int x;
  int y;
  int z;
  float scale[3] = {1,1,1,};
}block;

typedef struct coin{
  VAO *object;
  float point[3] = {0,0,0};
  float dim[3] = {TILE_SIDE,TILE_SIDE,TILE_SIDE};
  //float color[6][3] = {{1,0,0},{0,1,0},{0,0,1},{0.49,0.42,0.42},{0,0,0},{0.5,0,0}};
  float color[6][3] = {{1,1,0},{1,1,0},{1,1,0},{1,1,0},{0.8,0.8,0},{0.8,0.8,0}};
  int x;
  int y;
  int z;
  bool is_alive = 0;
  float scale[3] = {1,1,1,};
}coin;

typedef struct mario{
  VAO *object;
  int health = 10;
  int right = 0;
  int front = 0;
  int jump=0;
  int move=0;
  int score =0;
  float Vy = 0;
  float prev[3] = {0,0,0};
  float axis[3] = {0,1,0};
  float pres[3] = {0,0,0};
  float point[3] = {0,0,0};
  float dim[3] = {TILE_SIDE,TILE_SIDE,TILE_SIDE};
  //float color[6][3] = {{1,0,0},{0,1,0},{0,0,1},{0.49,0.42,0.42},{0,0,0},{0.5,0,0}};
  float color[6][3] = {{0.2,0.8,0.2},{1,0,0},{0.2,0.8,0.2},{0.2,0.8,0.2},{1,1,1},{0.2,0.8,0.2}};

  float scale[3] = {1,1,1,};
}mario;


pointer mouse;
vector<block*> blocks;
vector<coin*> coins;
float c_t[3] = {0,0,0};
float c_e[3] = {0,0,CAMERA_DIST};
float c_u[3] = {0,1,0};
bool panning_mode=0;

float thita = 0;
float phi = 0;
float dist = CAMERA_DIST;
float fov=90;
int vertical_angle_increase = 1;
float rotateangle =0;
mario player;
int coin_count=0;
int present_camera = 1;
GLuint programID;


/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
                          0,                  // attribute 0. Vertices
                          3,                  // size (x,y,z)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
                          1,                  // attribute 1. Color
                          3,                  // size (r,g,b)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/

VAO *test;


struct VAO* createCircle(float radius,float c[3]){
  // We plot all points in the space and render the shape.

  // if theta is angle made with x axis then coordinates of the point are rcos(o) and rsin(0)

  GLfloat vertex_buffer_data[1080];
  GLfloat color_buffer_data[1080];
  int i;
    for (i=0; i+2<1080; i+=3) {
        vertex_buffer_data[i] = radius*cos(i*M_PI/180);
        vertex_buffer_data[i+1] = radius*sin(i*M_PI/180);
        vertex_buffer_data[i+2] = 0;
        color_buffer_data[i] = c[0];
        color_buffer_data[i+1] = c[1];
        color_buffer_data[i+2] = c[2];
    }
    //cout << "hello" ;
    //cout << c[0] << " " << c[1] << " " << c[2] << endl;
    return  create3DObject(GL_TRIANGLE_FAN, 360, vertex_buffer_data, color_buffer_data, GL_FILL);


}

VAO *xaxis,*yaxis,*zaxis;

VAO *test_cube;

VAO *createCube1(float length,float width,float height,float color[6][3]){
  float x = length/2;
  float y = height;
  float z = width/2;

  GLfloat vertex_buffer_data [] = {
    -x,0,z, //front face
    x,0,z,  //front face
    x,y,z,  //front face

    x,y,z,  //front face
    -x,y,z, //front face
    -x,0,z, //front face

    x,0,z,   //right face
    x,0,-z,  //right face
    x,y,-z, //right face

    x,y,-z, //right face
    x,y,z,  //right face
    x,0,z,  //right face

    x,0,-z, //back face
    -x,0,-z, //back face
    -x,y,-z,  //back face

    -x,y,-z,  //back face
    x,y,-z,   //back face
    x,0,-z,   //back face

    -x,0,-z,  //left face
    -x,0,z, //left face
    -x,y,z, //left face

    -x,y,z, //left face
    -x,y,-z,  //left face
    -x,0,-z,  //left face

    -x,y,z, //top face
    x,y,z,  //top face
    x,y,-z, //top face

    x,y,-z, //top face
    -x,y,-z,  //top face
    -x,y,z,  //top face

    -x,0,z, //bottom face
    x,0,z,  //bottom face
    x,0,-z, //bottom face

    x,0,-z, //bottom face
    -x,0,-z,  //bottom face
    -x,0,z  //bottom face
  };

  /*GLfloat color_buffer_data [] = {
    1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,
    0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,
    0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,
    0.49,0.42,0.42,0.49,0.42,0.42,0.49,0.42,0.42,0.49,0.42,0.42,0.49,0.42,0.42,0.49,0.42,0.42,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0.5,0,0,0.5,0,0,0.5,0,0,0.5,0,0,0.5,0,0,0.5,0,0
  };*/

  GLfloat color_buffer_data [] = {
    color[0][0], color[0][1], color[0][2], 
  color[0][0], color[0][1], color[0][2], 
  color[0][0], color[0][1], color[0][2], 
  color[0][0], color[0][1], color[0][2], 
  color[0][0], color[0][1], color[0][2], 
  color[0][0], color[0][1], color[0][2], 
  color[1][0], color[1][1], color[1][2], 
  color[1][0], color[1][1], color[1][2], 
  color[1][0], color[1][1], color[1][2], 
  color[1][0], color[1][1], color[1][2], 
  color[1][0], color[1][1], color[1][2], 
  color[1][0], color[1][1], color[1][2], 
  color[2][0], color[2][1], color[2][2], 
  color[2][0], color[2][1], color[2][2], 
  color[2][0], color[2][1], color[2][2], 
  color[2][0], color[2][1], color[2][2], 
  color[2][0], color[2][1], color[2][2], 
  color[2][0], color[2][1], color[2][2], 
  color[3][0], color[3][1], color[3][2], 
  color[3][0], color[3][1], color[3][2], 
  color[3][0], color[3][1], color[3][2], 
  color[3][0], color[3][1], color[3][2], 
  color[3][0], color[3][1], color[3][2], 
  color[3][0], color[3][1], color[3][2], 
  color[4][0], color[4][1], color[4][2], 
  color[4][0], color[4][1], color[4][2], 
  color[4][0], color[4][1], color[4][2], 
  color[4][0], color[4][1], color[4][2], 
  color[4][0], color[4][1], color[4][2], 
  color[4][0], color[4][1], color[4][2], 
  color[5][0], color[5][1], color[5][2], 
  color[5][0], color[5][1], color[5][2], 
  color[5][0], color[5][1], color[5][2], 
  color[5][0], color[5][1], color[5][2], 
  color[5][0], color[5][1], color[5][2], 
  color[5][0], color[5][1], color[5][2] 
  };
  return create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}

 void create_axis(){
   GLfloat vertex_buffer_data [] = {
    1000,0,0,
    -1000,0,0,
    0,0,0,
  };
  GLfloat color_buffer_data [] = {
    1,0,0,
    1,0,0,
    1,0,0,

  };
   GLfloat vertex_buffer_data1 [] = {
    0,1000,0,
    0,-1000,0,
    0,0,0,
  };
   GLfloat color_buffer_data1 [] = {
    0,1,0,
    0,1,0,
    0,1,0,

  };

   GLfloat vertex_buffer_data2 [] = {
    0,0,-1000,
    0,0,1000,
    0,0,0,
  };
   GLfloat color_buffer_data2 [] = {
    0,0,1,
    0,0,1,
    0,0,1,

  };
  xaxis = create3DObject(GL_TRIANGLE_FAN, 3, vertex_buffer_data, color_buffer_data, GL_LINE);
  yaxis = create3DObject(GL_TRIANGLE_FAN, 3, vertex_buffer_data1, color_buffer_data1, GL_LINE);
  zaxis = create3DObject(GL_TRIANGLE_FAN, 3, vertex_buffer_data2, color_buffer_data2, GL_LINE);
}

/*typedef struct block{
  VAO *object;
  float point[3] = {0,0,0};
  float dim[3] = {TILE_SIDE,TILE_SIDE,TILE_SIDE};
  float color[6][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
  float scale[3] = {1,1,1,};
}block;*/


block* create_block(int x,int z,int level){
  block* temp = new block;
  temp->point[0] = (float(TILE_SIDE)/2 + TILE_SEPERATION_FACTOR)* (2*x-1); 
  temp->point[2] = (float(TILE_SIDE)/2 +TILE_SEPERATION_FACTOR)* (2*z-1);
  temp->point[1] = float(TILE_SIDE)*level;
  //test_cube = createCube(4,4,4,cl);
  temp->x = x;
  temp->y = level;
  temp->z = z;
  temp->object = createCube1(TILE_SIDE,TILE_SIDE,TILE_SIDE,temp->color);
  
  return temp;
}
coin* create_coin(int x,int z,int level){
  cout << "coin created" << endl;
  coin* temp = new coin;
  temp->point[0] = (float(TILE_SIDE)/2 + TILE_SEPERATION_FACTOR)* (2*x-1); 
  temp->point[2] = (float(TILE_SIDE)/2 +TILE_SEPERATION_FACTOR)* (2*z-1);
  temp->point[1] = float(TILE_SIDE)*level;
  //test_cube = createCube(4,4,4,cl);
  temp->x = x;
  temp->y = level;
  temp->z = z;
  temp->is_alive = 1;
  temp->object = createCube1(TILE_SIDE,TILE_SIDE,TILE_SIDE,temp->color);
  
  return temp;
}

void make_floor(){
  for(int j=1;j<=FLOOR_WIDTH/2;j++){
    for(int i=1;i<=FLOOR_LENGTH/2;i++){
      blocks.push_back(create_block(i, j,0));
      blocks.push_back(create_block(-i, j,0));
      blocks.push_back(create_block(i, -j,0));
      blocks.push_back(create_block(-i, -j,0)); 
    }
    //cout << blocks.size() << endl;
  }
}


void read(){
  ifstream input( "content/ref.txt" );
  int row=1;
  string line;
  int m;
  if (input.is_open()){
      while(getline(input,line)){
          //cout << line << '\n';
          int m=line.length();
          int col=0;
          //cout << "line length is " << m << endl;
          for(int i=0;i<m;i++){
            int height = int(line[i])-int('0');
            if (line[i] == 'C'){              
              blocks.push_back(create_block(row,i,0));
              coins.push_back(create_coin(row,i,1));
              coin_count++;
            }
            else if(height){
              for(int j=0;j<height;j++){
                blocks.push_back(create_block(row,i,j));
              }
            }
            
          }
          //cout << row << "  completed" << endl;
          row++;
      }
      input.close();
    }
    cout << blocks.size() << endl;
}


















void draw(VAO *object,float location[3],float angle,float axis[3]){ //assuming angle in radians
  glm::mat4 VP = Matrices.projection * Matrices.view;
  glm::mat4 MVP;
  Matrices.model = glm::mat4(1.0f);  
  glm::mat4 translate = glm::translate (glm::vec3(location[0],location[1],location[2]));     // glTranslatef
  if(angle!=0){
    glm::mat4 rotate = glm::rotate((float)(angle), glm::vec3(axis[0],axis[1],axis[2])); // rotate about vector (-1,1,1)  
  //glm::mat4 scale = glm::scale(glm::vec3(scale_matrix[0],scale_matrix[1],scale_matrix[2]));
    Matrices.model *=  translate*rotate;//*scale;
  
  }
  else
    Matrices.model *=  translate;//*scale;

  //Matrices.model *= (translate * rotate);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(object);
  //cout << "cannon rotated" << endl;
  ///////////////Have to reset after finishing 
  Matrices.model = glm::mat4(1.0f);  
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  ////////////////////

}


/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void restart(){
          player.pres[0] = 10.0f;
          player.pres[1] = 4.0f;
          player.pres[2] = 4.0f;
          player.jump = 0;
          player.Vy = 0;
          player.health--;
return ;
}
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
     // Function is called first on GLFW_PRESS.

    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_C:
                break;
            case GLFW_KEY_P:
                break;
            case GLFW_KEY_X:
                // do something ..
                break;
            case GLFW_KEY_LEFT:
            case GLFW_KEY_RIGHT:
                player.right = 0;
                break;
            case GLFW_KEY_UP:
            case GLFW_KEY_DOWN:
              player.front = 0;
              player.move=0;
              break;
            default:
                break;
        }
    }
    else if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                quit(window);
                break;
            case GLFW_KEY_LEFT:
                player.right = -1;
                break;
            case GLFW_KEY_RIGHT:
                player.right = 1;
                break;
            case GLFW_KEY_UP:
                player.front = 1;
                player.move=1;               
                break;
            case GLFW_KEY_DOWN:
                player.front = -1;
                player.move=1;                
                break;
            case GLFW_KEY_SPACE:
                if(!player.jump){
                  player.jump=1;
                  player.Vy = 0.4;
                }
            default:
                break;
        }
    }
}
void horizontal_rotation(){
  thita += 5 * M_PI/180.0f ;
  if(thita > 2*M_PI)
    thita -= 2*M_PI;}
void vertical_rotation(){
  phi += 5 * float(M_PI)/180.0f * vertical_angle_increase;
  if(phi > float(M_PI)/2){
    vertical_angle_increase*=-1;
    phi = float(M_PI)/2 - 3*M_PI/180.0f;
  }
  else if(phi < -float(M_PI)/2){
    phi = -float(M_PI)/2 + 3*M_PI/180.0f;
    vertical_angle_increase*=-1;
  }

}
/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
            quit(window);
            break;
    case 'P':
    case 'p':
          panning_mode = !panning_mode;
          cout << "panning_mode is " << panning_mode << endl;
          break;  
    case 'm':
          horizontal_rotation();
          break;
    case 'n':
          vertical_rotation();
          break;
    case 'r':
          restart();
          break;
    case 'c':
        present_camera++;
        if(present_camera > CAMERA_MAX)
          present_camera=1;
        if(present_camera==1){
          c_t[0] =0;
          c_t[1] =0;
          c_t[2] = 0;
          phi = 45*M_PI/180.0f;
          thita = 60*M_PI/180.0f;
        }
        break;
		default:
			break;
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_RELEASE){
              mouse.left_is_released = 1;
              mouse.left_is_clicked = 0;
            }
            else if(action == GLFW_PRESS){
              mouse.left_is_released = 0;
              mouse.left_is_clicked = 1;  
            }
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_RELEASE) {
              mouse.right_is_released = 1;
              mouse.right_is_clicked = 0;
            }
            else if(action == GLFW_PRESS){
              mouse.right_is_released = 0;
              mouse.right_is_clicked = 1;  
            }
            break;
        default:
            break;
    }
}


void pan_screen(){
    //float xshift = (scalex_left+scalex_right)*(float((mouse.dx)/float(SCREEN_WIDTH)));
  float b[3] ; // b is positive z axis
  b[0] = (c_e[0]-c_t[0])/dist;
  b[1] = (c_e[1]-c_t[1])/dist;
  b[2] = (c_e[2]-c_t[2])/dist;
  float a[3] = {0,1,0};  // a is +ve y axis
  float final[3];
  final[0] = a[1]*b[2] - a[2]*b[1];
  final[1] = b[0]*a[2] - a[0]*b[2];
  final[2] = a[0]*b[1] - b[0]*a[1];
  //cout << final[0] << "  " << final[1] << "  " << final[2] << endl;
  float x = sqrt(pow(final[0],2)+pow(final[1],2)+pow(final[2],2));
  final[0] /= x;
  final[1] /= x;
  final[2] /= x;
  if(mouse.dx != 0){
  
    float floats_in_x = dist * tan(fov/2 * M_PI/180.0f) * float(SCREEN_WIDTH)/float(SCREEN_HEIGHT) * 2;
    float xshiftvalue = floats_in_x * (float((mouse.dx)/float(SCREEN_WIDTH)));
      c_t[0] -= final[0]*xshiftvalue;
      c_t[1] -= final[1]*xshiftvalue;
      c_t[2] -= final[2]*xshiftvalue;
    
  }
  if(mouse.dy != 0){
    float a[3] = {0,1,0};  // a is +ve y axis
    float c[3]= {0,1,0};
    //c[0] = a[1]*b[2] - a[2]*b[1];
    //c[1] = b[0]*a[2] - a[0]*b[2];
    //c[2] = a[0]*b[1] - b[0]*a[1];
    c[0] = b[1]*final[2] - b[2]*final[1];
    c[1] = final[0]*b[2] - b[0]*final[2];
    c[2] = b[0]*final[1] - final[0]*b[1];
  
    float floats_in_y = dist * tan(fov/2 * M_PI/180.0f) * float(SCREEN_HEIGHT)/float(SCREEN_WIDTH) * 2;
    float yshiftvalue = floats_in_y * (float((mouse.dy)/float(SCREEN_HEIGHT)));
      c_t[0] += c[0]*yshiftvalue;
      c_t[1] += c[1]*yshiftvalue;
      c_t[2] += c[2]*yshiftvalue;
    
  }

}

void update_mouse(GLFWwindow* window,double a,double b){
  mouse.prev_xpos = mouse.pres_xpos;
  mouse.prev_ypos = mouse.pres_ypos;
  glfwGetCursorPos(window, &mouse.pres_xpos, &mouse.pres_ypos);
  mouse.dx = mouse.pres_xpos - mouse.prev_xpos;
  mouse.dy = mouse.pres_ypos - mouse.prev_ypos; 
 
  if(mouse.left_is_clicked && panning_mode){
    pan_screen();
  }

  /*if(panning_mode && mouse.left_is_clicked){
    cout << "************" << endl;
    if(mouse.dx > 0)
      cout << "moving right" << endl;
    else if(mouse.dx < 0)
      cout << "moving left" << endl;
    else
      cout << "no movement in horizontal direction" << endl;

    if(mouse.dy < 0)
      cout << "moving top" << endl;
    else if(mouse.dy > 0)
      cout << "moving bottom" << endl;
    else
      cout << "no movement in vertical direction" << endl;
    cout << "************" << endl;
  }*/
}




void zoomin(GLFWwindow* window,double xoffset, double yoffset){

  if(yoffset > 0){
    //cout << "scrolling up" << endl;
    //fov-= 20;
    dist-=20;
  }
  else if(yoffset < 0){
    //fov+=20;
    dist+=20;
    //cout << "scrolling down" << endl;
  }
  //cout << "dist is " << dist <<", float angle is " << fov << ", xmax is "<<dist * tan(fov/2 * M_PI/180.0f) * float(SCREEN_WIDTH)/float(SCREEN_HEIGHT) << ", ymax is  " <<dist * tan(fov/2 * M_PI/180.0f) * float(SCREEN_HEIGHT)/float(SCREEN_WIDTH) <<  endl;


}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
    int fbwidth=width, fbheight=height;
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
     is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
     Matrices.projection = glm::perspective (fov*M_PI/180.0,  double(SCREEN_WIDTH)/double(SCREEN_HEIGHT), 0.1, 5000.0);
    // Ortho projection for 2D views
    //Matrices.projection = glm::ortho(-scalex_left, scalex_right, -scaley_bottom, scaley_top, -500.0f, 500.0f);
}





/* Render the scene with openGL */
/* Edit this function according to your assignment */

int flaa=1;
void CheckCollision(){
  //cout << player.pres[0] << "  " << player.pres[1] <<  "  " << player.pres[2] << endl;
  float R = TILE_SIDE;
  float K = TILE_SIDE/sqrt(2);
  player.jump=1;
  for(int i=0;i<blocks.size();i++){
    float ydiff = player.pres[1] - blocks[i]->point[1];
    cout << "ydiff is " << ydiff << endl;
    /*if(ydiff == TILE_SIDE && int(player.pres[0]) >= blocks[i]->point[0]-2 && int(player.pres[0]) <= blocks[i]->point[0]+2 && int(player.pres[2]) >= blocks[i]->point[2]-2 && int(player.pres[2]) <= blocks[i]->point[2]+2 ){
        //on_the_floor=1;
        //cout << "already on floor" << blocks[i]->x << "   " <<blocks[i]->z << endl;
        player.jump=0;
        player.Vy=0;
        break;
    }*/
    if(ydiff <= TILE_SIDE && ydiff > 0 && int(player.pres[0]) >= blocks[i]->point[0]-2 && int(player.pres[0]) <= blocks[i]->point[0]+2 && int(player.pres[2]) >= blocks[i]->point[2]-2 && int(player.pres[2]) <= blocks[i]->point[2]+2 ){
        cout << "on floor because of " << blocks[i]->x << "   " <<blocks[i]->z << endl;
        player.jump=0;
        player.Vy=0;
        player.pres[1] = blocks[i]->point[1] + TILE_SIDE;
        break;
    }
  }

  cout << "result is " << player.jump << endl;

  for(int i=0;i<blocks.size();i++){
    float ydiff = player.pres[1] - blocks[i]->point[1];
    float temp_dist = sqrt(pow(player.pres[2] - blocks[i]->point[2],2) + pow(player.pres[0] - blocks[i]->point[0],2));    
      if(player.jump && temp_dist < R){
        cout <<"checking in jumping scenario" << endl;
        if(player.pres[0] > blocks[i]->point[0] + R/2 && player.pres[0] <= blocks[i]->point[0]+R){
          player.pres[0] = blocks[i]->point[0]+R;
        }
        else if(player.pres[0] < blocks[i]->point[0] - R/2 && player.pres[0] >= blocks[i]->point[0]-R){
          player.pres[0] = blocks[i]->point[0]-R;
        }
        else if(player.pres[2] > blocks[i]->point[2] + R/2 && player.pres[2] <= blocks[i]->point[2]+R){
          player.pres[2] = blocks[i]->point[2]+R;
        }
        else if(player.pres[2] < blocks[i]->point[2] - R/2 && player.pres[2] >= blocks[i]->point[2]-R){
          player.pres[2] = blocks[i]->point[2]-R;
        }
      }

      if(!player.jump && temp_dist <= TILE_SIDE && ydiff==0){
        cout << "reverting back !!" << blocks[i]->x <<"  "<< blocks[i]->z << endl;
        player.pres[0] = player.prev[0];
        player.pres[1] = player.prev[1];
        player.pres[2] = player.prev[2];
      }
  }
  return ;
}
void CheckCollision1(){
  bool on_the_floor = 0;
  bool shouldbreak = 0;
  for(int i=0;i<blocks.size();i++){
    float temp_dist = sqrt(pow(player.pres[2] - blocks[i]->point[2],2) + pow(player.pres[0] - blocks[i]->point[0],2));
      float ydiff = player.pres[1] - blocks[i]->point[1];

    if(player.jump){
      if(ydiff < TILE_SIDE && int(player.pres[0]) >= blocks[i]->point[0]-2 && int(player.pres[0]) <= blocks[i]->point[0]+2 && int(player.pres[2]) >= blocks[i]->point[2]-2 && int(player.pres[2]) <= blocks[i]->point[2]+2 ){// temp_dist < float(TILE_SIDE)/sqrt(2)){
        player.pres[1] = blocks[i]->point[1] + TILE_SIDE;
        player.jump = 0;
        player.Vy = 0;
        shouldbreak =1;
        on_the_floor=1;
      }
     /* else if(sqrt(pow(blocks[i]->point[0]-player.pres[0],2)+pow(blocks[i]->point[2]-player.pres[2],2)+pow(blocks[i]->point[1]-player.pres[1],2)) < TILE_SIDE){
      
      }
      break;*/
    }

    if(shouldbreak)
      break;

    if(ydiff == TILE_SIDE && temp_dist < float(TILE_SIDE)/sqrt(2)){
      on_the_floor = 1;
    }
    /*if(ydiff< float(TILE_SIDE)/2  && temp_dist < TILE_SIDE ){
        cout << "on the floor " << endl;
        cout << ydiff << endl;
        player.pres[1] = blocks[i]->point[1] + TILE_SIDE;
        player.jump = 0;
        player.Vy = 0;
        break;
    }*/
    if(sqrt(pow(blocks[i]->point[0]-player.pres[0],2)+pow(blocks[i]->point[2]-player.pres[2],2)+pow(blocks[i]->point[1]-player.pres[1],2)) < TILE_SIDE){
      player.pres[0] = player.prev[0];
      player.pres[1] = player.prev[1];
      player.pres[2] = player.prev[2];
      restart();
      //on_the_floor=1;
      break;
    }
  }
  if(!on_the_floor)
    player.jump = 1;


  for(int i=0;i<coins.size();i++){
    if(coins[i]->is_alive){
      if(sqrt(pow(coins[i]->point[0]-player.pres[0],2)+pow(coins[i]->point[2]-player.pres[2],2)+pow(coins[i]->point[1]-player.pres[1],2)) < TILE_SIDE){
        player.score++;
        coins[i]->is_alive = 0;
        coin_count--;
      }
    }
  }



return ;
}


void setup()
{
  // clear the color and depth in the frame buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use the loaded shader program
  // Don't change unless you know what you are doing
  glUseProgram (programID);
  if(present_camera==1){

    c_e[1] = c_t[1] + dist * sin(phi);
    c_e[0] = c_t[0] + dist * cos(phi) * sin(thita);
    c_e[2] = c_t[2] + dist * cos(phi) * cos(thita);
  }
  else if(present_camera==2){
    c_t[0] = player.pres[0];
    c_t[1] = player.pres[1];
    c_t[2] = player.pres[2];
    dist = 10;
    thita = 270*M_PI/180.0f - rotateangle;
    phi = 45*M_PI/180.0f;
    c_e[1] = c_t[1] + dist * sin(phi);
    c_e[0] = c_t[0] + dist * cos(phi) * sin(thita);
    c_e[2] = c_t[2] + dist * cos(phi) * cos(thita);
  }
  else if(present_camera==3){
    int yolo=1;
    //if(rotateangle>M_PI/2 && rotateangle < 3*M_PI/2)
     // yolo*=-1;
    
    c_e[0] = player.pres[0]+TILE_SIDE/2*yolo*cos(rotateangle)+2;
    c_e[1] = player.pres[1]+TILE_SIDE/2;
    c_e[2] = player.pres[2]+TILE_SIDE/2*yolo*sin(rotateangle)+2;

    float temp_dist = 30;

    c_t[0] = player.pres[0]+temp_dist*cos(rotateangle)+2;
    c_t[1] = player.pres[1]+temp_dist*sin(phi);
    c_t[2] = player.pres[2]+temp_dist*sin(rotateangle)+2;
  
     
  }

  else if(present_camera==4){
    dist = 150;
    thita = 0;
    phi = 60*M_PI/180.0f;
    c_e[1] = c_t[1] + dist * sin(phi);
    c_e[0] = c_t[0] + dist * cos(phi) * sin(thita);
    c_e[2] = c_t[2] + dist * cos(phi) * cos(thita);
    
  }
  else if(present_camera==5){
    dist = 150;
    phi = M_PI/2;
    c_e[1] = c_t[1] + dist * sin(phi);
    c_e[0] = c_t[0] + dist * cos(phi) * sin(thita);
    c_e[2] = c_t[2] + dist * cos(phi) * cos(thita);
    
  }





  // Eye - Location of camera. Don't change unless you are sure!!
  //glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  glm::vec3 eye ( c_e[0], c_e[1], c_e[2]);
  
  // Target - Where is the camera looking at.  Don't change unless you are sure!!
  glm::vec3 target (c_t[0], c_t[1], c_t[2]);
  // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
  glm::vec3 up (c_u[0], c_u[1], c_u[2]);
  Matrices.view = glm::lookAt( eye, target, up );
  glm::mat4 VP = Matrices.projection * Matrices.view;
  glm::mat4 MVP;    // MVP = Projection * View * Model
  Matrices.model = glm::mat4(1.0f);
  MVP = VP * Matrices.model; // MVP = p * V * M
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  Matrices.model = glm::mat4(1.0f);

  /* Render your scene */
  float temploc[3] = {0,0,0};
  float tempaxis[3] = {0,0,0};
  float tempangle = 0;
  //draw(xaxis,temploc,0,tempscale);
  draw(xaxis,temploc,tempangle,tempaxis);
  //cout << "xaxis finished" << endl;
  draw(yaxis,temploc,0,tempaxis);
  //cout << "yaxis finished" << endl;
  draw(zaxis,temploc,0,tempaxis);
  //draw(test,temploc,0,tempscale);
  //cout << "zaxis finished" << endl;
  for(int i=0;i<blocks.size();i++){
    draw(blocks[i]->object,blocks[i]->point,0,tempaxis);
  }
  for(int i=0;i<coins.size();i++){
    if(coins[i]->is_alive)
      draw(coins[i]->object,coins[i]->point,0,tempaxis);
  }
  if(rotateangle > 2*M_PI)
    rotateangle-=2*M_PI;
  /*if(player.move==0)
    cout << "player is at rest" << endl;
  else if(player.move==1)
    cout << "player is moving!!" << endl;*/
  player.pres[2] += 0.2*sin(rotateangle)*player.move*player.front;//*player.right;
  player.pres[0] += 0.2*cos(rotateangle)*player.move*player.front;//*player.front;
  player.pres[1] += player.jump*player.Vy;
  rotateangle += player.right * 5*M_PI/180.0f;
  //cout << rotateangle << endl;

  if(player.jump){
    player.Vy -= GRAVITY;
  }

  CheckCollision1();

  
  draw(player.object,player.pres,2*M_PI-rotateangle,player.axis);
  player.prev[0] = player.pres[0];
  player.prev[1] = player.pres[1];
  player.prev[2] = player.pres[2];

}







/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
     is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

    /* Register function to handle mouse click */
    glfwSetMouseButtonCallback(window, mouseButton); 
    glfwSetCursorPosCallback(window, update_mouse);
    glfwSetScrollCallback(window,zoomin); // mouse button clicks

    return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
    /* Objects should be created before any other gl function and shaders */
	// Create the models
	create_axis(); // Generate the VAO, VBOs, vertices data & copy into the array buffer

  //make_floor();
	read();
  player.point[0] = player.pres[0] = player.prev[0] = 20.0f;
  player.point[1] = player.pres[1] = player.prev[1] = 4.0f;
  player.point[2] = player.pres[2] = player.prev[2] = 20.0f;
  player.object = createCube1(TILE_SIDE,TILE_SIDE,TILE_SIDE,player.color);
  // Create and compile our GLSL program from the shaders
	programID = LoadShaders( "content/shaders/Sample_GL.vert", "content/shaders/Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");

	
	reshapeWindow (window, width, height);

    // Background color of the scene
  glClearColor (1.0f, 1.0f, 1.0f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
	int width = SCREEN_WIDTH;
	int height = SCREEN_HEIGHT;

    GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);
  
    double last_update_time = glfwGetTime(), current_time;

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {

        // OpenGL Draw commands
        setup();

        // Swap Frame Buffer in double buffering
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();
        reshapeWindow (window, width, height);

        // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
        current_time = glfwGetTime(); // Time in seconds
        if ((current_time - last_update_time) >= 0.5) { // atleast 0.5s elapsed since last frame
            // do something every 0.5 seconds ..
            last_update_time = current_time;
        }
        if(!player.health){
          cout << "your health is zero :/ Try again.Your score is " << player.score  << endl;
          quit(window);
        }
        if(!coin_count){
          cout << "Congrats! you won.Your health is " << player.health << endl;
          quit(window);
        }
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
