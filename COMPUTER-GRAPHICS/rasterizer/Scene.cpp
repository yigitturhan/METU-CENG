#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>

#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"

using namespace tinyxml2;
using namespace std;

/*
    Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
    const char *str;
    XMLDocument xmlDoc;
    XMLElement *xmlElement;

    xmlDoc.LoadFile(xmlPath);

    XMLNode *rootNode = xmlDoc.FirstChild();

    // read background color
    xmlElement = rootNode->FirstChildElement("BackgroundColor");
    str = xmlElement->GetText();
    sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

    // read culling
    xmlElement = rootNode->FirstChildElement("Culling");
    if (xmlElement != NULL)
    {
        str = xmlElement->GetText();

        if (strcmp(str, "enabled") == 0)
        {
            this->cullingEnabled = true;
        }
        else
        {
            this->cullingEnabled = false;
        }
    }

    // read cameras
    xmlElement = rootNode->FirstChildElement("Cameras");
    XMLElement *camElement = xmlElement->FirstChildElement("Camera");
    XMLElement *camFieldElement;
    while (camElement != NULL)
    {
        Camera *camera = new Camera();

        camElement->QueryIntAttribute("id", &camera->cameraId);

        // read projection type
        str = camElement->Attribute("type");

        if (strcmp(str, "orthographic") == 0)
        {
            camera->projectionType = ORTOGRAPHIC_PROJECTION;
        }
        else
        {
            camera->projectionType = PERSPECTIVE_PROJECTION;
        }

        camFieldElement = camElement->FirstChildElement("Position");
        str = camFieldElement->GetText();
        sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

        camFieldElement = camElement->FirstChildElement("Gaze");
        str = camFieldElement->GetText();
        sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

        camFieldElement = camElement->FirstChildElement("Up");
        str = camFieldElement->GetText();
        sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

        camera->gaze = normalizeVec3(camera->gaze);
        camera->u = crossProductVec3(camera->gaze, camera->v);
        camera->u = normalizeVec3(camera->u);

        camera->w = inverseVec3(camera->gaze);
        camera->v = crossProductVec3(camera->u, camera->gaze);
        camera->v = normalizeVec3(camera->v);

        camFieldElement = camElement->FirstChildElement("ImagePlane");
        str = camFieldElement->GetText();
        sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
               &camera->left, &camera->right, &camera->bottom, &camera->top,
               &camera->near, &camera->far, &camera->horRes, &camera->verRes);

        camFieldElement = camElement->FirstChildElement("OutputName");
        str = camFieldElement->GetText();
        camera->outputFilename = string(str);

        this->cameras.push_back(camera);

        camElement = camElement->NextSiblingElement("Camera");
    }

    // read vertices
    xmlElement = rootNode->FirstChildElement("Vertices");
    XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
    int vertexId = 1;

    while (vertexElement != NULL)
    {
        Vec3 *vertex = new Vec3();
        Color *color = new Color();

        vertex->colorId = vertexId;

        str = vertexElement->Attribute("position");
        sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

        str = vertexElement->Attribute("color");
        sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

        this->vertices.push_back(vertex);
        this->colorsOfVertices.push_back(color);

        vertexElement = vertexElement->NextSiblingElement("Vertex");

        vertexId++;
    }

    // read translations
    xmlElement = rootNode->FirstChildElement("Translations");
    XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
    while (translationElement != NULL)
    {
        Translation *translation = new Translation();

        translationElement->QueryIntAttribute("id", &translation->translationId);

        str = translationElement->Attribute("value");
        sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

        this->translations.push_back(translation);

        translationElement = translationElement->NextSiblingElement("Translation");
    }

    // read scalings
    xmlElement = rootNode->FirstChildElement("Scalings");
    XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
    while (scalingElement != NULL)
    {
        Scaling *scaling = new Scaling();

        scalingElement->QueryIntAttribute("id", &scaling->scalingId);
        str = scalingElement->Attribute("value");
        sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

        this->scalings.push_back(scaling);

        scalingElement = scalingElement->NextSiblingElement("Scaling");
    }

    // read rotations
    xmlElement = rootNode->FirstChildElement("Rotations");
    XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
    while (rotationElement != NULL)
    {
        Rotation *rotation = new Rotation();

        rotationElement->QueryIntAttribute("id", &rotation->rotationId);
        str = rotationElement->Attribute("value");
        sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

        this->rotations.push_back(rotation);

        rotationElement = rotationElement->NextSiblingElement("Rotation");
    }

    // read meshes
    xmlElement = rootNode->FirstChildElement("Meshes");

    XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
    while (meshElement != NULL)
    {
        Mesh *mesh = new Mesh();

        meshElement->QueryIntAttribute("id", &mesh->meshId);

        // read projection type
        str = meshElement->Attribute("type");

        if (strcmp(str, "wireframe") == 0)
        {
            mesh->type = WIREFRAME_MESH;
        }
        else
        {
            mesh->type = SOLID_MESH;
        }

        // read mesh transformations
        XMLElement *meshTransformationsElement = meshElement->FirstChildElement("Transformations");
        XMLElement *meshTransformationElement = meshTransformationsElement->FirstChildElement("Transformation");

        while (meshTransformationElement != NULL)
        {
            char transformationType;
            int transformationId;

            str = meshTransformationElement->GetText();
            sscanf(str, "%c %d", &transformationType, &transformationId);

            mesh->transformationTypes.push_back(transformationType);
            mesh->transformationIds.push_back(transformationId);

            meshTransformationElement = meshTransformationElement->NextSiblingElement("Transformation");
        }

        mesh->numberOfTransformations = mesh->transformationIds.size();

        // read mesh faces
        char *row;
        char *cloneStr;
        int v1, v2, v3;
        XMLElement *meshFacesElement = meshElement->FirstChildElement("Faces");
        str = meshFacesElement->GetText();
        cloneStr = strdup(str);

        row = strtok(cloneStr, "\n");
        while (row != NULL)
        {
            int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

            if (result != EOF)
            {
                mesh->triangles.push_back(Triangle(v1, v2, v3));
            }
            row = strtok(NULL, "\n");
        }
        mesh->numberOfTriangles = mesh->triangles.size();
        this->meshes.push_back(mesh);

        meshElement = meshElement->NextSiblingElement("Mesh");
    }
}

void Scene::assignColorToPixel(int i, int j, Color c)
{
    this->image[i][j].r = c.r;
    this->image[i][j].g = c.g;
    this->image[i][j].b = c.b;
}

/*
    Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
    if (this->image.empty())
    {
        for (int i = 0; i < camera->horRes; i++)
        {
            vector<Color> rowOfColors;
            vector<double> rowOfDepths;

            for (int j = 0; j < camera->verRes; j++)
            {
                rowOfColors.push_back(this->backgroundColor);
                rowOfDepths.push_back(1.01);
            }

            this->image.push_back(rowOfColors);
            this->depth.push_back(rowOfDepths);
        }
    }
    else
    {
        for (int i = 0; i < camera->horRes; i++)
        {
            for (int j = 0; j < camera->verRes; j++)
            {
                assignColorToPixel(i, j, this->backgroundColor);
                this->depth[i][j] = 1.01;
                this->depth[i][j] = 1.01;
                this->depth[i][j] = 1.01;
            }
        }
    }
}

/*
    If given value is less than 0, converts value to 0.
    If given value is more than 255, converts value to 255.
    Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
    if (value >= 255.0)
        return 255;
    if (value <= 0.0)
        return 0;
    return (int)(value);
}

/*
    Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
    ofstream fout;

    fout.open(camera->outputFilename.c_str());

    fout << "P3" << endl;
    fout << "# " << camera->outputFilename << endl;
    fout << camera->horRes << " " << camera->verRes << endl;
    fout << "255" << endl;

    for (int j = camera->verRes - 1; j >= 0; j--)
    {
        for (int i = 0; i < camera->horRes; i++)
        {
            fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
                 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
                 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
        }
        fout << endl;
    }
    fout.close();
}

/*
    Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
*/
void Scene::convertPPMToPNG(string ppmFileName)
{
    string command;

    // TODO: Change implementation if necessary.
    command = "./magick convert " + ppmFileName + " " + ppmFileName + ".png";
    system(command.c_str());
}

/*
    Transformations, clipping, culling, rasterization are done here.
*/
void Scene::forwardRenderingPipeline(Camera *camera)
{
    // TODO: Implement this function
    vector<Mesh2> res = this->transformation_process(camera);
    this->rasterization(camera, res);
}

std::vector<Mesh2> Scene::deepCopyMeshes(std::vector<Mesh2> &meshes)
{
    std::vector<Mesh2> copied_meshes;
    for (int i = 0; i < meshes.size(); i++){
        Mesh2 copied_mesh;
        copied_mesh.triangles = meshes[i].triangles;
        copied_mesh.meshId = meshes[i].meshId;
        copied_mesh.meshType = meshes[i].meshType;
        copied_mesh.numberOfTriangles = meshes[i].numberOfTriangles;
        copied_meshes.push_back(copied_mesh);
    }
    return copied_meshes;
}

Vec3 Scene::multiply_3x4_with_4x1(Matrix3x4 &m1, Vec4 &m2, Vec3 &result)
{
    result.x = m1.data[0][0] * m2.x + m1.data[0][1] * m2.y + m1.data[0][2] * m2.z + m1.data[0][3] * m2.t;
    result.y = m1.data[1][0] * m2.x + m1.data[1][1] * m2.y + m1.data[1][2] * m2.z + m1.data[1][3] * m2.t;
    result.z = m1.data[2][0] * m2.x + m1.data[2][1] * m2.y + m1.data[2][2] * m2.z + m1.data[2][3] * m2.t;
    return result;
}
void Scene::translationMatrix(Translation *translation, Matrix4 &matrix)
{
    double array[4] = {translation->tx, translation->ty, translation->tz, 1};
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (i == j) matrix.values[i][j] = 1;
            else if (j == 3) matrix.values[i][j] = array[i];
            else matrix.values[i][j] = 0;
        }
    }
}
void Scene::scalingMatrix(Scaling *scaling, Matrix4 &matrix)
{
    double array[4] = {scaling->sx, scaling->sy, scaling->sz, 1};
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (i == j) matrix.values[i][j] = array[i];
            else matrix.values[i][j] = 0;
        }
    }
}
Matrix4 Scene::rotationMatrix(Rotation *rotation)
{
    Vec3 u(rotation->ux, rotation->uy, rotation->uz);
    Vec3 v;
    u = normalizeVec3(u);
    int min_axis = 0;
    double min_val = abs(u.x);
    if (abs(u.y) < min_val){
        min_axis = 1;
        min_val = abs(u.y);
    }
    if (abs(u.z) < min_val) min_axis = 2;
    if (min_axis == 0){
        v.x = 0;
        v.y = u.z;
        v.z = -u.y;
    }
    else if (min_axis == 1){
        v.y = 0;
        v.x = -u.z;
        v.z = u.x;
    }
    else{
        v.z = 0;
        v.x = -u.y;
        v.y = u.x ;
    }
    Vec3 w = crossProductVec3(u, v);
    v = normalizeVec3(v);
    w = normalizeVec3(w);
    double M_array[4][4] = {{u.x, u.y, u.z, 0}, {v.x, v.y, v.z, 0}, {w.x, w.y, w.z, 0}, {0, 0, 0, 1}};
    Matrix4 M(M_array);
    double M_inverse_array[4][4] = {{u.x, v.x, w.x, 0}, {u.y, v.y, w.y, 0}, {u.z, v.z, w.z, 0}, {0, 0, 0, 1}};
    Matrix4 M_inverse(M_inverse_array);
    double theta = rotation->angle * M_PI / 180.0;
    double Rx_theta[4][4] = {{1, 0, 0, 0}, {0, cos(theta), -sin(theta), 0}, {0, sin(theta), cos(theta), 0}, {0, 0, 0, 1}};
    Matrix4 Rx(Rx_theta);
    Matrix4 Rx_M = multiplyMatrixWithMatrix(Rx, M);
    Matrix4 Minv_Rx_M = multiplyMatrixWithMatrix(M_inverse, Rx_M);
    return Minv_Rx_M;
}
Matrix4 Scene::modeling(Mesh *mesh)
{
    Matrix4 transformation = getIdentityMatrix();
    int n = mesh->numberOfTransformations;
    for (int i = 0; i < n; i++){
        char type = mesh->transformationTypes[i];
        int id = mesh->transformationIds[i];
        if (type == 't'){
            Matrix4 translation_matrix;
            translationMatrix(this->translations[id - 1], translation_matrix);
            transformation = multiplyMatrixWithMatrix(translation_matrix, transformation);
        }
        else if (type == 's'){
            Matrix4 scaling_matrix;
            scalingMatrix(this->scalings[id - 1], scaling_matrix);
            transformation = multiplyMatrixWithMatrix(scaling_matrix, transformation);
        }
        else if (type == 'r'){
            Matrix4 rotation_matrix = rotationMatrix(this->rotations[id - 1]);
            transformation = multiplyMatrixWithMatrix(rotation_matrix, transformation);
        }
    }
    return transformation;
}
Matrix4 Scene::camera_transformation(Camera *camera)
{
    Vec3 u = camera->u, v = camera->v, gaze = camera->gaze, e = camera->position;
    Vec3 w = multiplyVec3WithScalar(gaze, -1);
    double temp[4][4] = {{u.x, u.y, u.z, -(u.x * e.x + u.y * e.y + u.z * e.z)},
                         {v.x, v.y, v.z, -(v.x * e.x + v.y * e.y + v.z * e.z)},
                         {w.x, w.y, w.z, -(w.x * e.x + w.y * e.y + w.z * e.z)},
                         {0, 0, 0, 1}};
    Matrix4 M_cam(temp);
    return M_cam;
}
Matrix4 Scene::ortographic(Camera *camera)
{
    double l = camera->left, r = camera->right, b = camera->bottom, t = camera->top, n = camera->near, f = camera->far;
    double temp[4][4] = {{2 / (r - l), 0, 0, (-r - l) / (r - l)},
                         {0, 2 / (t - b), 0, (-t - b) / (t - b)},
                         {0, 0, -2 / (f - n), (-f - n) / (f - n)},
                         {0, 0, 0, 1}};
    Matrix4 M_cam(temp);
    return M_cam;
}
Matrix4 Scene::perspective(Camera *camera)
{
    double n = camera->near, t = camera->top, r = camera->right, l = camera->left, b = camera->bottom, f = camera->far;
    double temp[4][4] = {{2 * n / (r - l), 0, (r + l) / (r - l), 0},
                         {0, 2 * n / (t - b), (t + b) / (t - b), 0},
                         {0, 0, (-f - n) / (f - n), (-2 * f * n) / (f - n)},
                         {0, 0, -1, 0}};
    Matrix4 M_per(temp);
    return M_per;
}
Matrix3x4 Scene::viewport(Camera *camera)
{
    int nx = camera->horRes, ny = camera->verRes;
    Matrix3x4 M_vp;
    M_vp.data[0][0] = nx / 2;
    M_vp.data[0][3] = (nx - 1) / 2;
    M_vp.data[1][1] = ny / 2;
    M_vp.data[1][3] = (ny - 1) / 2;
    M_vp.data[2][2] = 0.5;
    M_vp.data[2][3] = 0.5;
    return M_vp;
}
vector<Mesh2> Scene::transformation_process(Camera *camera)
{
    vector<Mesh2> transformed_mesh_vector;
    long number_of_models = this->meshes.size();
    Matrix4 camera_matrix = camera_transformation(camera);
    Matrix4 projection_matrix;
    if (camera->projectionType == 1) projection_matrix = perspective(camera);
    else if (camera->projectionType == 0) projection_matrix = ortographic(camera);
    Matrix3x4 viewport_matrix = viewport(camera);
    for (int i = 0; i < number_of_models; i++){
        Matrix4 modeling_matrix = modeling(this->meshes[i]);
        Matrix4 Mprojection_Mcam_Mmodel = multiplyMatrixWithMatrix(projection_matrix, multiplyMatrixWithMatrix(camera_matrix, modeling_matrix));
        Mesh2 transformed_mesh;
        transformed_mesh.meshId = this->meshes[i]->meshId;
        transformed_mesh.meshType = this->meshes[i]->type;
        transformed_mesh.numberOfTriangles = this->meshes[i]->numberOfTriangles;
        for (int j = 0; j < this->meshes[i]->triangles.size(); j++){
            Triangle2 resulting_triangle;
            Triangle triangle = this->meshes[i]->triangles[j];
            bool visible = true;
            Vec3 v0, v1, v2;
            for (int k = 0; k < 3; k++){
                Vec3 vertex = *(this->vertices[triangle.vertexIds[k] - 1]);
                Vec4 homogenous_vertex(vertex.x, vertex.y, vertex.z, 1);
                Vec4 res = multiplyMatrixWithVec4(Mprojection_Mcam_Mmodel, homogenous_vertex);
                switch (k){
                case 0:
                    v0.x = res.x;
                    v0.y = res.y;
                    v0.z = res.z ;
                    break;
                case 1:
                    v1.x = res.x;
                    v1.y = res.y;
                    v1.z = res.z ;
                    break;
                case 2:
                    v2.x = res.x;
                    v2.y = res.y;
                    v2.z = res.z ;
                    break;
                }
                res.x = res.x / res.t;
                res.y = res.y / res.t;
                res.z = res.z / res.t;
                res.t = 1;
                Vec3 r;
                multiply_3x4_with_4x1(viewport_matrix, res, r);
                resulting_triangle.vertices[k][0] = r.x;
                resulting_triangle.vertices[k][1] = r.y;
                resulting_triangle.vertices[k][2] = r.z;
                resulting_triangle.colors[k] = *(this->colorsOfVertices[triangle.vertexIds[k] - 1]);
            }
            if (this->cullingEnabled){
                Vec3 n = crossProductVec3(subtractVec3(v1, v0), subtractVec3(v2, v0));
                Vec3 v = subtractVec3(v0, camera->v);
                visible = (dotProductVec3(n, v) > 0);
            }
            if (visible) transformed_mesh.triangles.push_back(resulting_triangle);
            else transformed_mesh.numberOfTriangles--;
        }
        transformed_mesh_vector.push_back(transformed_mesh);
    }
    return transformed_mesh_vector;
}
void Scene::midpoint_for_wireframe(Mesh2 &mesh, Camera &camera)
{

    Color c0, c1, temp;
    int x, y, d, max_x, dx, dy, x_x1, x_x0, x0, y0, max_y, y_y1, y_y0, x1, y1;
    double z, m, dz, pixel_depth, z0, z1, max_z;
    bool cond;
    for (int i = 0; i < mesh.numberOfTriangles; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (!mesh.triangles[i].vertices[j]) continue;
            x0 = mesh.triangles[i].vertices[j][0], x1 = mesh.triangles[i].vertices[(j + 1) % 3][0];
            y0 = mesh.triangles[i].vertices[j][1], y1 = mesh.triangles[i].vertices[(j + 1) % 3][1];
            z0 = mesh.triangles[i].vertices[j][2], z1 = mesh.triangles[i].vertices[(j + 1) % 3][2];
            c0 = mesh.triangles[i].colors[j], c1 = mesh.triangles[i].colors[(j + 1) % 3];
            m = (double)(y1 - y0) / (double)(x1 - x0);
            dy = abs(y1 - y0), dx = abs(x1 - x0);
            if (0 < m && m < 1){
                max_x = max(x0,x1), x = min(x0,x1);
                if (x0 >= x1){
                    max_z = z0;
                    z = z1;
                }
                else{
                    max_z = z1;
                    z = z0;
                }
                y = min(y0, y1);
                d = 2 * dy - dx;
                dz = (double)(max_z - z) / (double)(max_x - x);
                while (x < max_x)
                {
                    x_x1 = abs(x - x1), x_x0 = abs(x - x0);
                    temp.r = (double)(c0.r * x_x1 + c1.r * x_x0) / (double)dx;
                    temp.g = (double)(c0.g * x_x1 + c1.g * x_x0) / (double)dx;
                    temp.b = (double)(c0.b * x_x1 + c1.b * x_x0) / (double)dx;
                    pixel_depth = z;
                    cond = x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0;
                    if (cond && this->depth[x][y] > pixel_depth)
                    {
                        this->depth[x][y] = pixel_depth;
                        this->assignColorToPixel(x, y, temp);
                    }
                    if (d <= 0) d += 2 * dy;
                    else{
                        d += 2 * (dy - dx);
                        y++;
                    }
                    x++;
                    z += dz;
                }
            }
            else if (m >= 1)
            {
                x = min(x0, x1), max_y = max(y0, y1), d = 2 * dx - dy, y = min(y0,y1);
                if (y0 <= y1){
                    z = z0;
                    max_z = z1;
                }
                else{
                    z = z1;
                    max_z = z0;
                }
                dz = (max_z - z) / (max_y - y);
                while (y < max_y){
                    y_y1 = abs(y - y1), y_y0 = abs(y - y0);
                    temp.r = (double)(c0.r * y_y1 + c1.r * y_y0) / (double)dy;
                    temp.g = (double)(c0.g * y_y1 + c1.g * y_y0) / (double)dy;
                    temp.b = (double)(c0.b * y_y1 + c1.b * y_y0) / (double)dy;
                    pixel_depth = z;
                    cond = x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0;
                    if (cond && this->depth[x][y] > pixel_depth)
                    {
                        this->depth[x][y] = pixel_depth;
                        this->assignColorToPixel(x, y, temp);
                    }
                    if (d <= 0) d += 2 * dx;
                    else {
                        d += 2 * (dx - dy);
                        x++;
                    }
                    y++;
                    z += dz;
                }
            }
            else if (m <= 0 && m >= -1)
            {
                y = max(y0, y1), d = 2 * dy - dx, max_x = max(x0, x1), x = min(x0,x1);
                if (x0 <= x1){
                    z = z0;
                    max_z = z1;
                }
                else{
                    z = z1;
                    max_z = z0;
                }
                dz = (max_z - z) / (max_x - x);
                while (x < max_x){
                    x_x1 = abs(x - x1), x_x0 = abs(x - x0);
                    temp.r = (double)(c0.r * x_x1 + c1.r * x_x0) / (double)dx;
                    temp.g = (double)(c0.g * x_x1 + c1.g * x_x0) / (double)dx;
                    temp.b = (double)(c0.b * x_x1 + c1.b * x_x0) / (double)dx;
                    pixel_depth = z;
                    cond = x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0;
                    if (cond && this->depth[x][y] > pixel_depth)
                    {
                        this->depth[x][y] = pixel_depth;
                        this->assignColorToPixel(x, y, temp);
                    }
                    if (d <= 0) d += 2 * dy;
                    else{
                        d += 2 * (dy - dx);
                        y--;
                    }
                    x++;
                    z += dz;
                }
            }
            else if (m < -1)
            {
                x = max(x0, x1), max_y = max(y0,y1), y = min(y0,y1), d = 2 * dx - dy;
                if (y0 <= y1){
                    max_z = z1;
                    z = z0;
                }
                else{
                    max_z = z0;
                    z = z1;
                }
                dz = (max_z - z) / (max_y - y);
                while (y < max_y){
                    y_y1 = abs(y - y1), y_y0 = abs(y - y0);
                    temp.r = (double)(c0.r * y_y1 + c1.r * y_y0) / (double)dy;
                    temp.g = (double)(c0.g * y_y1 + c1.g * y_y0) / (double)dy;
                    temp.b = (double)(c0.b * y_y1 + c1.b * y_y0) / (double)dy;
                    pixel_depth = z;
                    cond = x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0;
                    if (cond && this->depth[x][y] > pixel_depth)
                    {
                        this->depth[x][y] = pixel_depth;
                        this->assignColorToPixel(x, y, temp);
                    }
                    if (d <= 0) d += 2 * dx;
                    else{
                        d += 2 * (dx - dy);
                        x--;
                    }
                    y++;
                    z += dz;
                }
            }
        }
    }
}
double Scene::f_01(int x, int y, int x0, int y0, int x1, int y1)
{
    return x * (y0 - y1) + y * (x1 - x0) + x0 * y1 - y0 * x1;
}
double Scene::f_12(int x, int y, int x1, int y1, int x2, int y2)
{
    return x * (y1 - y2) + y * (x2 - x1) + x1 * y2 - x2 * y1;
}
double Scene::f_20(int x, int y, int x0, int y0, int x2, int y2)
{
    return x * (y2 - y0) + y * (x0 - x2) + x2 * y0 - y2 * x0;
}
void Scene::triangle_rasterization_for_solid(Mesh2 &mesh, Camera *camera)
{
    int x0, x1, x2, y0, y1, y2, x_min, x_max, y_min, y_max;
    double z0, z1, z2, alpha, beta, gamma, alpha_var, beta_var, gamma_var, pixel_depth;
    Color c0, c1, c2, temp;
    bool cond;
    for (int i = 0; i < mesh.numberOfTriangles; i++)
    {
        Triangle2 temp_triangle = mesh.triangles[i];
        x0 = temp_triangle.vertices[0][0], x1 = temp_triangle.vertices[1][0], x2 = temp_triangle.vertices[2][0];
        y0 = temp_triangle.vertices[0][1], y1 = temp_triangle.vertices[1][1], y2 = temp_triangle.vertices[2][1];
        z0 = temp_triangle.vertices[0][2], z1 = temp_triangle.vertices[1][2], z2 = temp_triangle.vertices[2][2];
        c0 = temp_triangle.colors[0], c1 = temp_triangle.colors[1], c2 = temp_triangle.colors[2];
        x_min = min(min(x0, x1), x2), x_max = max(max(x0, x1), x2), y_min = min(min(y0, y1), y2), y_max = max(max(y0, y1), y2);
        alpha_var = f_12(x0, y0, x1, y1, x2, y2);
        beta_var = f_20(x1, y1, x0, y0, x2, y2);
        gamma_var = f_01(x2, y2, x0, y0, x1, y1);
        for (int y = y_min; y <= y_max; y++){
            for (int x = x_min; x <= x_max; x++){
                alpha = f_12(x, y, x1, y1, x2, y2) / alpha_var;
                beta = f_20(x, y, x0, y0, x2, y2) / beta_var;
                gamma = f_01(x, y, x0, y0, x1, y1) / gamma_var;
                if (alpha >= 0 && gamma >= 0 && beta >= 0){
                    temp.r = c0.r * alpha + c1.r * beta + c2.r * gamma;
                    temp.g = c0.g * alpha + c1.g * beta + c2.g * gamma;
                    temp.b = c0.b * alpha + c1.b * beta + c2.b * gamma;
                    pixel_depth = alpha * z0 + beta * z1 + gamma * z2;
                    cond = x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0;
                    if (cond && this->depth[x][y] > pixel_depth){
                        this->depth[x][y] = pixel_depth;
                        this->assignColorToPixel(x, y, temp);
                    }
                }
            }
        }
    }
}
void Scene::rasterization(Camera *camera, std::vector<Mesh2> &meshes)
{
    for (int i = 0; i < meshes.size(); i++){
        if (meshes[i].meshType == 1) triangle_rasterization_for_solid(meshes[i], camera);
        else{
            liang_barsky(camera, meshes, i);
            midpoint_for_wireframe(meshes[i], *camera);
        }
    }
}
bool Scene::is_visible(double den, double num, double &te, double &tl)
{
    double t = num / den;
    if (den > 0){
        if (t > tl) return false;
        if (t > te) te = t;
    }
    else if (den < 0){
        if (t < te) return false;
        if (t < tl) tl = t;
    }
    else if (num > 0) return false;
    return true;
}
void Scene::liang_barsky(Camera *camera, std::vector<Mesh2> &meshes, int i)
{
    double te = 0, tl = 1, x_min, x_max, y_min, y_max, dx, dy,p1_x, p1_y, p2_x, p2_y;
    Color diff;
    x_max = camera->right, x_min = camera->left, y_min = camera->bottom, y_max = camera->top;
    bool visible;
    for (int j = 0; j < meshes[i].numberOfTriangles; j++){
        for (int k = 0; k < 3; k++){
            te = 0, tl = 1, visible = false;
            p1_x = meshes[i].triangles[j].vertices[k][0], p1_y = meshes[i].triangles[j].vertices[k][1];
            p2_x = meshes[i].triangles[j].vertices[(k + 1) % 3][0], p2_y = meshes[i].triangles[j].vertices[(k + 1) % 3][1];
            dx = p2_x - p1_x, dy = p2_y - p1_y;
            diff.r = meshes[i].triangles[j].colors[(k + 1) % 3].r - meshes[i].triangles[j].colors[k].r;
            diff.g = meshes[i].triangles[j].colors[(k + 1) % 3].g - meshes[i].triangles[j].colors[k].g;
            diff.b = meshes[i].triangles[j].colors[(k + 1) % 3].b - meshes[i].triangles[j].colors[k].b;
            if (is_visible(dx, x_min - p1_x, te, tl)){
                if (is_visible(-dx, p1_x - x_max, te, tl)){
                    if (is_visible(dy, y_min - p1_y, te, tl)){
                        if (is_visible(-dy, p1_y - y_max, te, tl)){
                            visible = true;
                            if (tl < 1){
                                meshes[i].triangles[j].vertices[(k + 1) % 3][0] = p1_x + tl * dx;
                                meshes[i].triangles[j].vertices[(k + 1) % 3][1] = p1_y + tl * dy;
                                meshes[i].triangles[j].colors[(k + 1) % 3].r = meshes[i].triangles[j].colors[k].r + diff.r * tl;
                                meshes[i].triangles[j].colors[(k + 1) % 3].g = meshes[i].triangles[j].colors[k].g + diff.g * tl;
                                meshes[i].triangles[j].colors[(k + 1) % 3].b = meshes[i].triangles[j].colors[k].b + diff.b * tl;
                            }
                            if (te > 0){
                                meshes[i].triangles[j].vertices[k][0] = p1_x + te * dx;
                                meshes[i].triangles[j].vertices[k][1] = p1_y + te * dy;
                                meshes[i].triangles[j].colors[k].r += diff.r * te;
                                meshes[i].triangles[j].colors[k].g += diff.g * te;
                                meshes[i].triangles[j].colors[k].b += diff.b * te;
                            }
                        }
                    }
                }
            }
            meshes[i].triangles[j].visible[k] = visible;
        }
    }
}