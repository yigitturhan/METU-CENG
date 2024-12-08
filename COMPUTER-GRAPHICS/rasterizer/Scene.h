#ifndef __SCENE_H__
#define __SCENE_H__
#include "Vec3.h"
#include "Vec4.h"
#include "Color.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Camera.h"
#include "Mesh.h"
#include "Matrix4.h"
#include <vector>

using namespace std;
struct Matrix3x4
{
    double data[3][4];
    Matrix3x4()
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                data[i][j] = 0.0;
            }
        }
    }
};
struct Triangle2
{
    double vertices[3][3];
    Color colors[3];
    bool visible[3];
};
struct Mesh2
{
    std::vector<Triangle2> triangles;
    int meshId;
    int meshType; // 1 for solid, 0 for wireframe
    int numberOfTriangles;
};
class Scene
{
public:
	Color backgroundColor;
	bool cullingEnabled;

	std::vector<std::vector<Color> > image;
	std::vector<std::vector<double> > depth;
	std::vector<Camera *> cameras;
	std::vector<Vec3 *> vertices;
	std::vector<Color *> colorsOfVertices;
	std::vector<Scaling *> scalings;
	std::vector<Rotation *> rotations;
	std::vector<Translation *> translations;
	std::vector<Mesh *> meshes;

	Scene(const char *xmlPath);

	void assignColorToPixel(int i, int j, Color c);
	void initializeImage(Camera *camera);
	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera *camera);
	void convertPPMToPNG(std::string ppmFileName);
	void forwardRenderingPipeline(Camera *camera);
    Vec3 multiply_3x4_with_4x1(Matrix3x4 &m1, Vec4 &m2, Vec3 &result);
    void translationMatrix(Translation *translation, Matrix4 &matrix);
    void scalingMatrix(Scaling *scaling, Matrix4 &matrix);
    Matrix4 rotationMatrix(Rotation *rotation);
    Matrix4 modeling(Mesh *mesh);
    Matrix4 camera_transformation(Camera *camera);
    Matrix4 ortographic(Camera *camera);
    Matrix4 perspective(Camera *camera);
    Matrix3x4 viewport(Camera *camera);
    vector<Mesh2> transformation_process(Camera *camera);
    void midpoint_for_wireframe(Mesh2 &mesh, Camera &camera);
    double f_01(int x, int y, int x0, int y0, int x1, int y1);
    double f_12(int x, int y, int x1, int y1, int x2, int y2);
    double f_20(int x, int y, int x0, int y0, int x2, int y2);
    void triangle_rasterization_for_solid(Mesh2 &mesh, Camera *camera);
    void rasterization(Camera *camera, std::vector<Mesh2> &meshes);
    bool is_visible(double den, double num, double &te, double &tl);
    void liang_barsky(Camera *camera, std::vector<Mesh2> &meshes, int i);
    std::vector<Mesh2> deepCopyMeshes(std::vector<Mesh2> &meshes);
};

#endif
