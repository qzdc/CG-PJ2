#include "Renderer.h"

#include "ArgParser.h"
#include "Camera.h"
#include "Image.h"
#include "Ray.h"
#include "VecUtils.h"

#include <limits>


Renderer::Renderer(const ArgParser &args) :
    _args(args),
    _scene(args.input_file)
{
}

void
Renderer::Render()
{
    int w = _args.width;
    int h = _args.height;

    Image image(w, h);
    Image nimage(w, h);
    Image dimage(w, h);

    // loop through all the pixels in the image
    // generate all the samples

    // This look generates camera rays and callse traceRay.
    // It also write to the color, normal, and depth images.
    // You should understand what this code does.
    Camera* cam = _scene.getCamera();
    for (int y = 0; y < h; ++y) {
        float ndcy = 2 * (y / (h - 1.0f)) - 1.0f;
        for (int x = 0; x < w; ++x) {
            float ndcx = 2 * (x / (w - 1.0f)) - 1.0f;
            // Use PerspectiveCamera to generate a ray.
            // You should understand what generateRay() does.
            Ray r = cam->generateRay(Vector2f(ndcx, ndcy));

            Hit h;
            Vector3f color = traceRay(r, cam->getTMin(), _args.bounces, h);

            image.setPixel(x, y, color);
            nimage.setPixel(x, y, (h.getNormal() + 1.0f) / 2.0f);
            float range = (_args.depth_max - _args.depth_min);
            if (range) {
                dimage.setPixel(x, y, Vector3f((h.t - _args.depth_min) / range));
            }
        }
    }
    // END SOLN

    // save the files 
    if (_args.output_file.size()) {
        image.savePNG(_args.output_file);
    }
    if (_args.depth_file.size()) {
        dimage.savePNG(_args.depth_file);
    }
    if (_args.normals_file.size()) {
        nimage.savePNG(_args.normals_file);
    }
}



Vector3f Renderer::traceRay(const Ray &r,
    float tmin,
    int bounces,
    Hit &h) const
{
    if (_scene.getGroup()->intersect(r, tmin, h)) {//有交点
        Vector3f I,p;
        p = r.pointAtParameter(h.getT());
        I= _scene.getAmbientLight() * h.getMaterial()->getDiffuseColor();
        if(bounces>0){
            Vector3f N = h.getNormal().normalized();//平面法向量
            Vector3f temp_ray = r.getDirection().normalized();//反射前的光线
            Vector3f newR = (temp_ray + 2 * Vector3f::dot(N,-temp_ray) * N).normalized();//反射后的光线
            Ray newRay(p, newR);
            // 创建新的光线对象，用于继续光线反射过程
            Hit new_h = Hit();
            Vector3f indirect = traceRay(newRay, 0.0001, bounces - 1, new_h);
            // 递归调用traceRay函数，计算间接光照
            I += h.getMaterial()->getSpecularColor() * indirect;
        }
        return I;
    }
    
    return _scene.getBackgroundColor(r.getDirection());
    
}