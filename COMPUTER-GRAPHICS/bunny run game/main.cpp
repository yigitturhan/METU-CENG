#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <ft2build.h>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

GLuint gProgram[5];
int gWidth, gHeight;

GLint modelingMatrixLoc[5];
GLint viewingMatrixLoc[5];
GLint projectionMatrixLoc[5];
GLint eyePosLoc[5];
glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::mat4 modelingMatrix;
glm::vec3 eyePos(0, 0, 0);
float x  =  0, y = 0, z = -3, k = 0, coefficient = 0, speed_helper = 1, speed = 3, coef2 = 0, random_val;
bool isDown = false, turn = false, d_press = false, a_press = false, is_checkpoint_passed = true, game_over = false, restart = false, collision_happened = false;
int activeProgramIndex = 0, score = 0, count = 0, c2 = 0;

struct Vertex
{
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Texture
{
	Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
	GLfloat u, v;
};

struct Normal
{
	Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Face
{
	Face(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	GLuint vIndex[3], tIndex[3], nIndex[3];
};
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};
vector<Vertex> gVertices, roadVertices, cubeVertices;
vector<Texture> gTextures, roadTextures, cubeTextures;
vector<Normal> gNormals, roadNormals, cubeNormals;
vector<Face> gFaces, roadFaces, cubeFaces;
GLuint vao1, vao2, vao3,gTextVBO;
GLuint gVertexAttribBuffer, gIndexBuffer, roadVertexAttribBuffer, roadIndexBuffer, cubeVertexAttribBuffer, cubeIndexBuffer;
GLint gInVertexLocRoad, gInNormalLocRoad;
GLint gInVertexLoc, gInNormalLoc;
int gVertexDataSizeInBytes, gNormalDataSizeInBytes;

bool ParseObj(const string& fileName, vector<Vertex> &vertex, vector<Texture> &texture, vector<Normal> &normal, vector<Face> &face)
{
	fstream myfile;
	myfile.open(fileName.c_str(), std::ios::in);
	if (myfile.is_open()){
		string curLine;
		while (getline(myfile, curLine)){
			stringstream str(curLine);
			GLfloat c1, c2, c3;
			GLuint index[9];
			string tmp;
			if (curLine.length() >= 2){
				if (curLine[0] == 'v'){
					if (curLine[1] == 't'){
						str >> tmp;
						str >> c1 >> c2;
						texture.push_back(Texture(c1, c2));
					}
					else if (curLine[1] == 'n'){
						str >> tmp;
						str >> c1 >> c2 >> c3;
						normal.push_back(Normal(c1, c2, c3));
					}
					else{
						str >> tmp;
						str >> c1 >> c2 >> c3;
						vertex.push_back(Vertex(c1, c2, c3));
					}
				}
				else if (curLine[0] == 'f'){
					str >> tmp;
					char c;
					int vIndex[3], nIndex[3], tIndex[3];
					str >> vIndex[0]; str >> c >> c;
					str >> nIndex[0];
					str >> vIndex[1]; str >> c >> c;
					str >> nIndex[1];
					str >> vIndex[2]; str >> c >> c;
					str >> nIndex[2];
					assert(vIndex[0] == nIndex[0] &&
						vIndex[1] == nIndex[1] &&
						vIndex[2] == nIndex[2]);
					for (int c = 0; c < 3; ++c){
						vIndex[c] -= 1;
						nIndex[c] -= 1;
						tIndex[c] -= 1;
					}
					face.push_back(Face(vIndex, tIndex, nIndex));
				}
				else cout << "Ignoring unidentified line in obj file: " << curLine << endl;
			}
		}
		myfile.close();
	}
	else return false;
	assert(vertex.size() == normal.size());
	return true;
}
bool ReadDataFromFile(const string& fileName, string& data)
{
	fstream myfile;
	myfile.open(fileName.c_str(), std::ios::in);
	if (myfile.is_open()){
		string curLine;
		while (getline(myfile, curLine)){
			data += curLine;
			if (!myfile.eof()) data += "\n";
		}
		myfile.close();
	}
	else return false;
	return true;
}

GLuint createVS(const char* shaderName)
{
	string shaderSource;
	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource)){
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}
	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &shader, &length);
	glCompileShader(vs);
	char output[1024] = { 0 };
	glGetShaderInfoLog(vs, 1024, &length, output);
	printf("VS compile log: %s\n", output);
	return vs;
}

GLuint createFS(const char* shaderName)
{
	string shaderSource;
	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource)){
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}
	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &shader, &length);
	glCompileShader(fs);
	char output[1024] = { 0 };
	glGetShaderInfoLog(fs, 1024, &length, output);
	printf("FS compile log: %s\n", output);
	return fs;
}

void initShaders()
{
	gProgram[0] = glCreateProgram();
	gProgram[1] = glCreateProgram();
	gProgram[2] = glCreateProgram();
	gProgram[3] = glCreateProgram();
	gProgram[4] = glCreateProgram();
	GLuint vs2 = createVS("vert2.glsl");
	GLuint fs2 = createFS("frag2.glsl");
	GLuint vs3 = createVS("vert3.glsl");
	GLuint fs3 = createFS("frag3.glsl");
	GLuint vs4 = createVS("vert4.glsl");
	GLuint fs4 = createFS("frag4.glsl");
	GLuint vs5 = createVS("vert5.glsl");
	GLuint fs5 = createFS("frag5.glsl");
	GLuint vs6 = createVS("vert6.glsl");
	GLuint fs6 = createFS("frag6.glsl");
	glAttachShader(gProgram[0], vs2);
	glAttachShader(gProgram[0], fs2);
	glAttachShader(gProgram[1], vs3);
	glAttachShader(gProgram[1], fs3);
	glAttachShader(gProgram[2], vs4);
	glAttachShader(gProgram[2], fs4);
	glAttachShader(gProgram[3], vs5);
	glAttachShader(gProgram[3], fs5);
	glAttachShader(gProgram[4], vs6);
	glAttachShader(gProgram[4], fs6);
	glLinkProgram(gProgram[0]);
	GLint status;
	glGetProgramiv(gProgram[0], GL_LINK_STATUS, &status);
	if (status != GL_TRUE){
		cout << "Program link failed" << endl;
		exit(-1);
	}
	glLinkProgram(gProgram[1]);
	glGetProgramiv(gProgram[1], GL_LINK_STATUS, &status);
	if (status != GL_TRUE){
		cout << "Program link failed" << endl;
		exit(-1);
	}
	glLinkProgram(gProgram[2]);
	glGetProgramiv(gProgram[2], GL_LINK_STATUS, &status);
	if (status != GL_TRUE){
		cout << "Program link failed" << endl;
		exit(-1);
	}
	glLinkProgram(gProgram[3]);
	glGetProgramiv(gProgram[3], GL_LINK_STATUS, &status);
	if (status != GL_TRUE){
		cout << "Program link failed" << endl;
		exit(-1);
	}
	glLinkProgram(gProgram[4]);
	glGetProgramiv(gProgram[4], GL_LINK_STATUS, &status);
	if (status != GL_TRUE){
		cout << "Program link failed" << endl;
		exit(-1);
	}
	for (int i = 0; i < 5; ++i){
		modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
		viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
		projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
		eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
	}
}


void initVBO(GLuint &vao, vector<Vertex> &vertex, vector<Texture> &texture, vector<Normal> &normal, vector<Face> &face, GLuint &attrib, GLuint &index)
{
	glGenVertexArrays(1, &vao);
	cout<<vao<<endl;
	assert(vao> 0);
	glBindVertexArray(vao);
	cout << "vao = " << vao << endl;
	glGenBuffers(1, &attrib);
	glGenBuffers(1, &index);
	assert(attrib > 0 && index > 0);
	glBindBuffer(GL_ARRAY_BUFFER, attrib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	gVertexDataSizeInBytes = vertex.size() * 3 * sizeof(GLfloat);
	gNormalDataSizeInBytes = normal.size() * 3 * sizeof(GLfloat);
	int indexDataSizeInBytes = face.size() * 3 * sizeof(GLuint);
	GLfloat* vertexData = new GLfloat[vertex.size()* 3];
	GLfloat* normalData = new GLfloat[normal.size() * 3];
	GLuint* indexData = new GLuint[(face.size()) * 3];
	float minX = 1e6, maxX = -1e6;
	float minY = 1e6, maxY = -1e6;
	float minZ = 1e6, maxZ = -1e6;
	for (int i = 0; i < vertex.size(); ++i){
		vertexData[3 * i] = vertex[i].x;
		vertexData[3 * i + 1] = vertex[i].y;
		vertexData[3 * i + 2] = vertex[i].z;
		minX = std::min(minX, vertex[i].x);
		maxX = std::max(maxX, vertex[i].x);
		minY = std::min(minY, vertex[i].y);
		maxY = std::max(maxY, vertex[i].y);
		minZ = std::min(minZ, vertex[i].z);
		maxZ = std::max(maxZ, vertex[i].z);
	}
	for (int i = 0; i < normal.size(); ++i){
		normalData[3 * i] = normal[i].x;
		normalData[3 * i + 1] = normal[i].y;
		normalData[3 * i + 2] = normal[i].z;
	}
	for (int i = 0; i < face.size(); ++i){
		indexData[3 * i] = face[i].vIndex[0];
		indexData[3 * i + 1] = face[i].vIndex[1];
		indexData[3 * i + 2] = face[i].vIndex[2];
	}
	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void init()
{
	ParseObj("bunny.obj", roadVertices, roadTextures, roadNormals, roadFaces);
	ParseObj("quad.obj", gVertices, gTextures, gNormals, gFaces);
	ParseObj("cube.obj", cubeVertices, cubeTextures, cubeNormals, cubeFaces);
	glEnable(GL_DEPTH_TEST);
	initShaders();
	initVBO(vao1, gVertices, gTextures, gNormals, gFaces, gVertexAttribBuffer, gIndexBuffer);
	initVBO(vao2, roadVertices, roadTextures, roadNormals, roadFaces, roadVertexAttribBuffer, roadIndexBuffer);
	initVBO(vao3, cubeVertices, cubeTextures, cubeNormals, cubeFaces, cubeVertexAttribBuffer, cubeIndexBuffer);
	
}
int check_collision(float z_box){
	if (z_box - z < -1) return -1;
	if (x == 0) return 1;
	float x_right_bunny = x+0.5, x_left_bunny = x-0.5;
	if (x < 0){
		if (x_right_bunny > -0.5) return 1;
		if (x_left_bunny < -3) return 0;
		return -1;
	}
	else{
		if (x_left_bunny < 0.5) return 1;
		if (x_right_bunny > 3) return 2;
		return -1;
	}
}
void drawModel(int size)
{
	glDrawElements(GL_TRIANGLES, size*3 , GL_UNSIGNED_INT, 0);
}
void jump(){
	if (game_over){
		y = 0;
		return;
	}
	if (isDown){
		if (y <= 0){
			isDown = false;
			y += 0.02*speed;
		}
		else y -= 0.02*speed;
	}
	else{
		if (y >= 0.5){
			isDown = true;
			y -= 0.02*speed;
		}
		else y += 0.02*speed;
	}
}
int get_random_index(){
	srand((unsigned)time(0));
	return rand()%3;
}
void turnBunny(){
	if (game_over && count < 90){
		count += 9;
	}
	if(!turn) return;
	k += 3*speed;
	if(k >= 360){
		k = 0, turn = false;
	}
}
void updateX(){
	if (game_over) return;
	if (d_press){
		x+=0.05*speed;
		if (x > 3.5) x = 3.5;
	} 
	else if (a_press){
		x-=0.05*speed;
		if (x < -3.5) x = -3.5;
	} 
}
void display()
{
	
	glm::mat4 matT, matS, matR;
	glClearColor(0,0,0,1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	jump();
	if (is_checkpoint_passed){
		is_checkpoint_passed = false;
		random_val = get_random_index();
	}
	for(int j = 0; j < 30; j++){
		for (int k = 0; k < 5; k++){
			glUseProgram(gProgram[1+((j+k)%2)]);
			glBindVertexArray(vao1);
			glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);
			matT = glm::translate(glm::mat4(1.0), glm::vec3((-4.5+2*k),-3 ,-3-j*2+coefficient*speed));
			matS = glm::scale(glm::mat4(1.0),glm::vec3(1.0,1.0,1.0));
			matR = glm::rotate<float>(glm::mat4(1.0), (-90./ 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
			modelingMatrix = matT *matS*matR;
			glUniformMatrix4fv(projectionMatrixLoc[1+((j+k)%2)], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniformMatrix4fv(viewingMatrixLoc[1+((j+k)%2)], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
			glUniformMatrix4fv(modelingMatrixLoc[1+((j+k)%2)], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
			glUniform3fv(eyePosLoc[1+((j+k)%2)], 1, glm::value_ptr(eyePos));
			drawModel(gFaces.size());
		}
	}
	float zb = -60+speed*coef2;
	int res = check_collision(zb);
	if (res != -1 && res != random_val) game_over = true;
	else if (res != -1) turn = true;
	turnBunny();
	for (int j = 0; j < 3; j++){
		for (int k = 0; k < 3; k++){
			if (res == random_val){ 
				if (!collision_happened && !game_over) score += 1000;
				collision_happened = true;
			}
			if (j == res) continue;
			int prog = j != random_val ? 3 : 4;
			glUseProgram(gProgram[prog]);
			glBindVertexArray(vao3);
			glBindBuffer(GL_ARRAY_BUFFER, cubeVertexAttribBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBuffer);
			matT = glm::translate(glm::mat4(1.0), glm::vec3(3.5*(j-1),-3+k,-60+speed*coef2));
			matS = glm::scale(glm::mat4(0.5),glm::vec3(0.5,0.5,0.5));
			matR = glm::rotate<float>(glm::mat4(1.0), (0.0/ 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
			modelingMatrix = matT *matS*matR;
			glUniformMatrix4fv(projectionMatrixLoc[prog], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniformMatrix4fv(viewingMatrixLoc[prog], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
			glUniformMatrix4fv(modelingMatrixLoc[prog], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
			glUniform3fv(eyePosLoc[prog], 1, glm::value_ptr(eyePos));
			drawModel(cubeFaces.size());
		}
	}
	updateX();
	matT = glm::translate(glm::mat4(1.0), glm::vec3(x,y-2,z));
	matS = glm::scale(glm::mat4(0.4), glm::vec3(0.4, 0.4, 0.4));
	matR = glm::rotate<float>(glm::mat4(1.0), (-90./ 180.) * M_PI, glm::vec3(0.0, 1.0, 0.25));
	glm::mat4 matRx1 = glm::rotate<float>(glm::mat4(1.0), (k/180.) * M_PI, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 matRz1 = glm::rotate<float>(glm::mat4(1.0), (-count/180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));  
	modelingMatrix = matT * matS * matR * matRx1 * matRz1;
	glUseProgram(gProgram[0]);
	glUniformMatrix4fv(projectionMatrixLoc[0], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glBindVertexArray(vao2);
	glBindBuffer(GL_ARRAY_BUFFER, roadVertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roadIndexBuffer);
	glUniformMatrix4fv(viewingMatrixLoc[0], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
	glUniformMatrix4fv(modelingMatrixLoc[0], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
	glUniform3fv(eyePosLoc[0], 1, glm::value_ptr(eyePos));
	drawModel(roadFaces.size());
}

void reshape(GLFWwindow* window, int w, int h)
{
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;
	gWidth = w;
	gHeight = h;
	glViewport(0, 0, w, h);
	float fovyRad = (float)(90.0 / 180.0) * M_PI;
	projectionMatrix = glm::perspective(fovyRad, w / (float)h, 1.0f, 100.0f);
	viewingMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		glShadeModel(GL_FLAT);
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		glShadeModel(GL_SMOOTH);
	}
	else if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (key == GLFW_KEY_D)
	{
		if (action == GLFW_PRESS) d_press = true;
		else if (action == GLFW_RELEASE) d_press = false;
	}
	else if (key == GLFW_KEY_A)
	{
		if (action == GLFW_PRESS) a_press = true;
		else if (action == GLFW_RELEASE) a_press = false;
	}
	else if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		turn = true;
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS){
		restart = true;
	}

}

void mainLoop(GLFWwindow* window)
{
	while (!glfwWindowShouldClose(window))
	{
		if (restart){
			x  =  0, y = 0, z = -3, k = 0, coefficient = 0, speed_helper = 1, speed = 3, coef2 = 0;
			isDown = false, turn = false, d_press = false, a_press = false, is_checkpoint_passed = true, game_over = false, restart = false;
			count = 0, score = 0;
		}
		if(!game_over){
			c2 += 1;
			if (c2 >= 5){
				score += 1;
				c2 = 0;
			}
			coefficient += 0.1;
			coef2 += 0.1;
			speed += 0.00385;
			if (-60+speed*coef2 >= 0){
				is_checkpoint_passed = true;
				coef2 = 0;
				collision_happened = false;
			}
			if (coefficient*speed>=2*1.96961550602){
				coefficient = (coefficient*speed - 2*1.96961550602)/speed;
			}
		}
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main(int argc, char** argv)
{
	GLFWwindow* window;
	if (!glfwInit()){
		exit(-1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	int width = 1000, height = 800;
	window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);
	if (!window){
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (GLEW_OK != glewInit()){
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}
	gWidth = width, gHeight = height;
	char rendererInfo[512] = { 0 };
	strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER)); // Use strcpy_s on Windows, strcpy on Linux
	strcpy(rendererInfo, " - "); // Use strcpy_s on Windows, strcpy on Linux
	strcpy(rendererInfo, (const char*)glGetString(GL_VERSION)); // Use strcpy_s on Windows, strcpy on Linux
	glfwSetWindowTitle(window, rendererInfo);
	init();
	glfwSetKeyCallback(window, keyboard);
	glfwSetWindowSizeCallback(window, reshape);
	reshape(window, width, height);
	mainLoop(window);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
