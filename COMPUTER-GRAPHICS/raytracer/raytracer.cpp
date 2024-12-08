#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <cmath>
#include <chrono>
#include <thread>
using namespace parser;

/*
November 2023
Middle East Technical University
Contributors : Ahmet Yiğit Turhan, Furkan Numanoğlu
Ray Trace Project
*/

typedef struct Ray
{
    Vec3f origin;
    Vec3f direction;
    int depth;

} ray;

typedef struct Hit
{
    Vec3f hitPoint;
    Vec3f normalVector;
    int materialId;
    float t;
    int obj; // sphere = 0, triangle = 1, mesh = 2
    int num;
} hit;

float dotProduct(const Vec3f &v1, const Vec3f &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float determinantCalculator3(const Vec3f &v1,const Vec3f &v2, const Vec3f &v3)
{
    return v1.x * (v2.y * v3.z - v3.y * v2.z) + v1.y * (v3.x * v2.z - v2.x * v3.z) + v1.z * (v2.x * v3.y - v3.x * v2.y);
}

Vec3f crossProduct(const Vec3f &v1, const Vec3f &v2)
{
    Vec3f resultVector;
    resultVector.x = v1.y * v2.z - v1.z * v2.y;
    resultVector.y = v1.z * v2.x - v1.x * v2.z;
    resultVector.z = v1.x * v2.y - v1.y * v2.x;
    return resultVector;
}

Vec3f addVector(const Vec3f &v1, const Vec3f &v2)
{
    Vec3f resultVector;
    resultVector.x = v1.x + v2.x;
    resultVector.y = v1.y + v2.y;
    resultVector.z = v1.z + v2.z;
    return resultVector;
}

Vec3f subtractVector(const Vec3f &v1, const Vec3f &v2)
{
    Vec3f resultVector;
    resultVector.x = v1.x - v2.x;
    resultVector.y = v1.y - v2.y;
    resultVector.z = v1.z - v2.z;
    return resultVector;
}

Vec3f scalarMultiplication(const Vec3f &v1, float k)
{
    Vec3f resultVector;
    resultVector.x = v1.x * k;
    resultVector.y = v1.y * k;
    resultVector.z = v1.z * k;
    return resultVector;
}
float findLength(const Vec3f &v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
Vec3f normalizationFunc(const Vec3f &v)
{
    Vec3f resultRay;
    float length = findLength(v);
    if (length == 0)
        resultRay = {0, 0, 0};
    else
    {
        resultRay.x = v.x / length;
        resultRay.y = v.y / length;
        resultRay.z = v.z / length;
    }
    return resultRay;
}

float findDiscriminant(const Ray &ray, const Sphere &sphere, const Vec3f &center)
{
    Vec3f direction = ray.direction;
    Vec3f or_min_cent = subtractVector(ray.origin, center);
    return pow(dotProduct(direction, or_min_cent), 2) - dotProduct(direction, direction) * (dotProduct(or_min_cent, or_min_cent) - pow(sphere.radius, 2));
}
Vec3f irradienceFunc(const PointLight &p, const Vec3f &intersectionPoint)
{
    Vec3f result;
    Vec3f intensity = p.intensity;
    Vec3f dist = subtractVector(p.position, intersectionPoint);
    float dist_square = dotProduct(dist, dist);
    if (dist_square == 0)
        return {0, 0, 0};
    result.x = intensity.x / dist_square;
    result.y = intensity.y / dist_square;
    result.z = intensity.z / dist_square;
    return result;
}
Vec3f diffuseCalc(const Scene &scene, const  PointLight &p, const Hit* hit, int materialId)
{
    Vec3f result;
    Vec3f hitPoint = hit->hitPoint;
    Vec3f irradience = irradienceFunc(p, hitPoint);
    Vec3f point_to_light = subtractVector(p.position, hitPoint);
    Vec3f surface_normal = hit->normalVector;
    point_to_light = normalizationFunc(point_to_light);
    float wi_dot_n = dotProduct(point_to_light, surface_normal);
    if (wi_dot_n <= 0)
    {
        return {0, 0, 0};
    }
    Vec3f diffuse = scene.materials[materialId - 1].diffuse;
    result.x = diffuse.x * wi_dot_n * irradience.x;
    result.y = diffuse.y * wi_dot_n * irradience.y;
    result.z = diffuse.z * wi_dot_n * irradience.z;
    return result;
}
Vec3f ambientCalc(const Scene &scene, int materialId)
{
    Vec3f result;
    Vec3f material_ambient = scene.materials[materialId - 1].ambient;
    Vec3f ambient_light = scene.ambient_light;
    result.x = material_ambient.x * ambient_light.x;
    result.y = material_ambient.y * ambient_light.y;
    result.z = material_ambient.z * ambient_light.z;
    return result;
}
Vec3f specularCalc(const Scene &scene,const PointLight &p, int materialId, const Hit* hit, const Ray &ray)
{
    Vec3f result;
    Material material = scene.materials[materialId - 1];
    Vec3f irr = irradienceFunc(p, hit->hitPoint);
    Vec3f w_i = subtractVector(p.position, hit->hitPoint);
    w_i = normalizationFunc(w_i);
    Vec3f wi_minus_w0 = subtractVector(w_i, ray.direction);
    Vec3f h = normalizationFunc(wi_minus_w0);
    float h_dot_n = dotProduct(h, hit->normalVector);
    if (h_dot_n < 0)
        return {0, 0, 0};
    float phong = pow(h_dot_n, material.phong_exponent);
    Vec3f specular = material.specular;
    result.x = specular.x * phong * irr.x;
    result.y = specular.y * phong * irr.y;
    result.z = specular.z * phong * irr.z;
    return result;
}
Vec3f sFinder(const Camera &camera, int i, int j)
{
    float l = camera.near_plane.x, r = camera.near_plane.y, b = camera.near_plane.z, t = camera.near_plane.w;
    Vec3f v = camera.up, gaze = camera.gaze;
    Vec3f u = crossProduct(v, scalarMultiplication(gaze, -1));
    gaze = normalizationFunc(gaze);
    v = normalizationFunc(v);
    u = normalizationFunc(u);
    Vec3f m = addVector(camera.position, scalarMultiplication(gaze, camera.near_distance));
    Vec3f lu = scalarMultiplication(u, l);
    Vec3f tv = scalarMultiplication(v, t);
    Vec3f q = addVector(m, addVector(lu, tv));
    float s_u = (i + 0.5) * (r - l) / camera.image_width;
    float s_v = (j + 0.5) * (t - b) / camera.image_height;
    Vec3f u_t_su = scalarMultiplication(u, s_u);
    Vec3f v_t_sv = scalarMultiplication(v, s_v);
    Vec3f s = addVector(q, subtractVector(u_t_su, v_t_sv));
    return s;
}
float sphereIntersectionHelper(const Scene &scene, const Ray &ray, const Sphere &sphere, float discriminant)
{
    Vec3f direction = ray.direction;
    Vec3f or_min_center = subtractVector(ray.origin, scene.vertex_data.at(sphere.center_vertex_id - 1));
    float ray_dot_ray = dotProduct(direction, direction);
    float fr = -1 * dotProduct(direction, or_min_center);
    float t1 = (fr + sqrtf(discriminant)) / ray_dot_ray;
    float t2 = (fr - sqrtf(discriminant)) / ray_dot_ray;
    return fmin(t1,t2);
}
Hit* sphereIntersection(const Scene &scene, const Ray &ray, const Sphere &sphere, int num)
{
    float radius = sphere.radius;
    Vec3f direction = ray.direction, origin = ray.origin;
    Vec3f center = scene.vertex_data.at(sphere.center_vertex_id - 1);
    float discriminant = findDiscriminant(ray, sphere, center);
    if (discriminant < 0) return nullptr;
    float t = sphereIntersectionHelper(scene, ray, sphere, discriminant);
    if (t < 0) return nullptr;
    Hit* resultHit = new Hit();
    resultHit->t = t;
    Vec3f dir_t_t = scalarMultiplication(direction, t);
    resultHit->hitPoint = addVector(origin, dir_t_t);
    resultHit->normalVector = subtractVector(resultHit->hitPoint, center);
    resultHit->normalVector.x /= radius;
    resultHit->normalVector.y /= radius;
    resultHit->normalVector.z /= radius;
    resultHit->materialId = sphere.material_id;
    resultHit->obj = 0;
    resultHit->num = num;
    return resultHit;
}
Hit* triangleIntersection(const Scene &scene,const Ray &ray, const Vec3f &a, const Vec3f &b, const Vec3f &c, int materialId, int num)
{
    Vec3f origin = ray.origin, direction = ray.direction;
    Vec3f a_min_o = subtractVector(a, origin);
    Vec3f a_min_c = subtractVector(a, c);
    Vec3f a_min_b = subtractVector(a, b);
    float determinantA = determinantCalculator3(a_min_b, a_min_c, direction);
    if (determinantA == 0)
        return nullptr;
    float t = determinantCalculator3(a_min_b, a_min_c, a_min_o) / determinantA; // calculation of t
    if ( t < scene.shadow_ray_epsilon)
        return nullptr;
    float gamma = determinantCalculator3(a_min_b, a_min_o, direction) / determinantA; // calculation of gamma
    if (gamma > 1 || gamma < 0)
        return nullptr;
    float beta = determinantCalculator3(a_min_o, a_min_c, direction) / determinantA; // calculation of beta
    if (beta < 0 || beta + gamma > 1)
        return nullptr;
    Hit* hitResult = new Hit();
    hitResult->t = t;
    hitResult->hitPoint = addVector(origin, scalarMultiplication(direction, t));
    Vec3f normal = crossProduct(subtractVector(b, a), subtractVector(c, a));
    hitResult->normalVector = normalizationFunc(normal);
    hitResult->materialId = materialId;
    hitResult->obj = 1;
    hitResult->num = num;
    return hitResult;
}
Hit* getClosestHit(std::vector<Hit*> &v)
{
    Hit* resultHit = nullptr;
    if (v.empty()) return nullptr;
    resultHit = v[0];
    Hit* current;
    float min_t = v[0]->t;
    for (int i = 1; i < v.size(); i++)
    {
        current = v.at(i);
        if (min_t > current->t)
        {
            min_t = current->t;
            resultHit = current;
        }
    }
    for (int i = 0; i < v.size(); i++){
        if (v[i] != resultHit) delete v[i];
    }
    return resultHit;
}
Hit* meshIntersection(const Scene &scene, const Ray &ray, const Mesh &mesh, int num)
{
    Hit* hitResult = nullptr;
    std::vector<Hit*> hits;
    Vec3f a, b, c;
    Face face;
    int size = mesh.faces.size();
    for (int f = 0; f < size; f++)
    {
    	face = mesh.faces[f];
    	a = scene.vertex_data[face.v0_id - 1];
    	b = scene.vertex_data[face.v1_id - 1];
    	c = scene.vertex_data[face.v2_id - 1];
        hitResult = triangleIntersection(scene, ray, a, b, c, mesh.material_id, num);
        if (hitResult) hits.push_back(hitResult);
    }
    if (hits.empty()) return hitResult;
    hitResult = getClosestHit(hits);
    hitResult->obj = 2;
    hitResult->num = num;
    return hitResult;
}
bool isShadow(const Scene &scene, const Ray &ray)
{
    Hit* hit;
    Vec3f a, b, c;
    Triangle t1;
    int size = scene.spheres.size();
    for (int s = 0; s < size; s++)
    {
        hit = sphereIntersection(scene, ray, scene.spheres[s],s);
        if (hit){
        	delete hit;
        	return true;
        }
    }
    size = scene.triangles.size();
    for (int t = 0; t < size; t++)
    {
    	t1 = scene.triangles[t];
    	a = scene.vertex_data[t1.indices.v0_id - 1];
    	b = scene.vertex_data[t1.indices.v1_id - 1];
    	c = scene.vertex_data[t1.indices.v2_id - 1];
        hit = triangleIntersection(scene, ray, a,b,c,t1.material_id,t);
        if (hit){
        	delete hit;
        	return true;
        }
    }
    size = scene.meshes.size();
    for (int m = 0; m < size; m++)
    {
        hit = meshIntersection(scene, ray, scene.meshes[m],m);
        if (hit){
          	delete hit;
        	return true;
        }
    }
    return false;
}

Hit* getIntersection(const Scene &scene, const Ray &ray)
{
    Vec3f a,b,c;
    Triangle t1;
    Hit* hit;
    std::vector<Hit*> hit_vector;
    int size = scene.spheres.size();
    for (int s = 0; s < size; s++)
    {
        hit = sphereIntersection(scene, ray, scene.spheres[s],s);
        if (hit) hit_vector.push_back(hit);
    }
    size = scene.triangles.size();
    for (int t = 0; t < size; t++)
    {
    	t1 = scene.triangles[t];
    	a = scene.vertex_data[t1.indices.v0_id - 1];
    	b = scene.vertex_data[t1.indices.v1_id - 1];
    	c = scene.vertex_data[t1.indices.v2_id - 1];
        hit = triangleIntersection(scene, ray, a,b,c,t1.material_id,t);
        if (hit) hit_vector.push_back(hit);
    }
    size = scene.meshes.size();
    for (int m = 0; m < size; m++)
    {
        hit = meshIntersection(scene, ray, scene.meshes[m],m);
        if (hit) hit_vector.push_back(hit);
    }
    Hit* finalHit = getClosestHit(hit_vector);
    hit_vector.clear();
    return finalHit;
}
Ray generateShadowRay(const Scene &scene, const Hit* hit, const PointLight &p)
{
    Ray ray;
    Vec3f hitPoint = hit->hitPoint;
    Vec3f w_i = subtractVector(p.position, hitPoint);
    Vec3f norm_t_eps = scalarMultiplication(hit->normalVector, scene.shadow_ray_epsilon);
    ray.origin = addVector(hitPoint, norm_t_eps);
    ray.direction = normalizationFunc(w_i);
    return ray;
}
Ray generateReflectionRay(const Scene &scene, const Ray &ray, const Hit* hit)
{
    Ray res;
    Vec3f rayDirection = ray.direction;
    Vec3f normal = hit->normalVector;
    Vec3f n_2_n_w0 = scalarMultiplication(normal, -2 * dotProduct(normal, rayDirection));
    Vec3f normalized_w_r = normalizationFunc(addVector(n_2_n_w0, ray.direction));
    Vec3f w_r_epsilon = scalarMultiplication(normalized_w_r, scene.shadow_ray_epsilon);
    res.origin = addVector(hit->hitPoint, w_r_epsilon);
    res.direction = normalized_w_r;
    return res;
}
Vec3f pixelCalc(const Scene &scene, const Hit* hit, int materialId, const Ray &ray)
{
    Vec3f res;
    Ray shadow;
    Vec3f dt, st;
    Vec3f amb = ambientCalc(scene, materialId);
    Vec3f diff = {0, 0, 0};
    Vec3f spec = {0, 0, 0};
    Vec3f mirr = {0, 0, 0};
    int size = scene.point_lights.size();
    for (int i = 0; i < size; i++)
    {
        shadow = generateShadowRay(scene, hit, scene.point_lights[i]);
        if (isShadow(scene, shadow)) continue;
        dt = diffuseCalc(scene, scene.point_lights[i], hit, materialId);
        st = specularCalc(scene, scene.point_lights[i], materialId, hit, ray);
        diff = addVector(diff, dt);
        spec = addVector(spec, st);
    }
    if (scene.materials[materialId - 1].is_mirror && ray.depth <= scene.max_recursion_depth)
    {
        Ray reflection = generateReflectionRay(scene, ray, hit);
        Hit* final = getIntersection(scene, reflection);
        if (final)
        {
            if (hit->obj != final->obj || hit->num != final->num)
            {
            	Vec3f mirror = scene.materials[materialId - 1].mirror;
                reflection.depth = ray.depth + 1;
                delete hit;
                mirr = pixelCalc(scene, final, final->materialId, reflection);
                mirr.x *= mirror.x;
                mirr.y *= mirror.y;
                mirr.z *= mirror.z;
            }
        }
    }
    res.x = amb.x + diff.x + spec.x + mirr.x;
    res.y = amb.y + diff.y + spec.y + mirr.y;
    res.z = amb.z + diff.z + spec.z + mirr.z;
    return res;
}
Ray rayGenerator(const Camera &camera, int i, int j)
{
    Vec3f pixel = sFinder(camera, i, j);
    Ray resultRay;
    Vec3f pos = camera.position;
    Vec3f d = subtractVector(pixel, pos);
    d = normalizationFunc(d);
    resultRay.origin = pos;
    resultRay.direction = d;
    resultRay.depth = 0;
    return resultRay;
}
void mainHelper(const Scene &scene, const Camera &camera, Vec3i* result, const int min_height, const int max_height, const int width){
	Ray ray;
	Hit* finalHit;
	Vec3i background_color = scene.background_color;
	int pixelNumber = 0;
	for (int i = min_height; i < max_height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            ray = rayGenerator(camera, j, i);
            finalHit = getIntersection(scene, ray);
            if (finalHit)
            {
                Vec3f color = pixelCalc(scene, finalHit, finalHit -> materialId, ray);
                result[i*width+j].x = std::min(255, (int)round(color.x));
                result[i*width+j].y = std::min(255, (int)round(color.y));
                result[i*width+j].z = std::min(255, (int)round(color.z));
            }
            else
            {
                result[i*width+j].x = background_color.x;
                result[i*width+j].y = background_color.y;
                result[i*width+j].z = background_color.z;
            }
            pixelNumber += 3;
        }
    }
}

int main(int argc, char *argv[])
{
    parser::Scene scene;
    scene.loadFromXml(argv[1]);
    Camera camera;
    int cameraNumber = scene.cameras.size();
    int width, height, pixelNumber;
    Vec3i background_color = {scene.background_color.x, scene.background_color.y, scene.background_color.z};
    Ray ray;
    Hit* finalHit;
    for (int c = 0; c < cameraNumber; c++)
    {
        camera = scene.cameras.at(c);
        width = camera.image_width;
        height = camera.image_height;
        Vec3i* image1 = new Vec3i[width*height];
        int num_of_cores = std::thread::hardware_concurrency();
        std::thread* threads = new std::thread[num_of_cores];
        const int height_incr = height / num_of_cores;
        pixelNumber = 0;
        for (int r = 0; r < num_of_cores;r++){
        	int min_height = r * height_incr;
            int max_height;
            if (r == num_of_cores -1) max_height = height;
            else max_height = (r+1)*height_incr;
        	threads[r] = std::thread(mainHelper,scene,camera,image1,min_height,max_height,width);
        }
        for (int i = 0; i < num_of_cores; i++) threads[i].join();
        delete[] threads;
        pixelNumber = 0;
        unsigned char *image = new unsigned char[width * 3 * height];
        for (int i = 0; i < width;i++){
            for(int j = 0; j < height;j++){
                const Vec3i pixel = image1[i*height + j];
            	image[pixelNumber] = pixel.x;
            	image[pixelNumber + 1] = pixel.y;
            	image[pixelNumber + 2] = pixel.z;
                pixelNumber += 3;
            }
        }
        delete [] image1;
        write_ppm(camera.image_name.c_str(), image, width, height);
        delete [] image;
    }
    return 0;
}