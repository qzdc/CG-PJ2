#include "Renderer.h"

#include "ArgParser.h"
#include "Camera.h"
#include "Image.h"
#include "Ray.h"
#include "VecUtils.h"

#include <limits>
#include <random>
#include <vector>


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
            if(_args.jitter) {
                Vector3f color, n_val, d_val;
                for(int i = 0;i < 16;i++) {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_real_distribution<float> distr(-0.5, 0.5);
                    float dx = distr(gen);
                    float dy = distr(gen);
                    float temp_ndcx = ndcx + 2 * dx / (w - 1.0f);
                    float temp_ndcy = ndcy + 2 * dy / (h - 1.0f);

                    Ray r = cam->generateRay(Vector2f(temp_ndcx, temp_ndcy));

                    Hit h;
                    Vector3f color_new = traceRay(r, cam->getTMin(), _args.bounces, h);
                    n_val += (h.getNormal() + 1.0f) / 2.0f;

                    d_val += Vector3f(h.t - _args.depth_min);
                    color += color_new;

                    if (i == 15) {
                        image.setPixel(x, y, color / 16);
                        nimage.setPixel(x, y, n_val / 16);
                        dimage.setPixel(x, y, d_val / 16 / (_args.depth_max - _args.depth_min));
                    }
                }
            }
            else {
                Ray r = cam->generateRay(Vector2f(ndcx, ndcy));

                Hit h;
                Vector3f color = traceRay(r, cam->getTMin(), _args.bounces, h);

                image.setPixel(x, y, color);
                nimage.setPixel(x, y, (h.getNormal() + 1.0f) / 2.0f);
                dimage.setPixel(x, y, Vector3f((h.t - _args.depth_min) / (_args.depth_max - _args.depth_min)));
            }
        }
    }
    // END SOLN

    if (_args.filter) {
        // Downsample the image using a Gaussian filter
        Image downsample_image(w / 3, h / 3);
        Image downsample_nimage(w / 3, h / 3);
        Image downsample_dimage(w / 3, h / 3);
        std::vector<float> gaussian_matrix = {1, 2, 1, 2, 4, 2, 1, 2, 1};//高斯核


        for (int y = 0; y < h / 3; y++) {
            for (int x = 0; x < w / 3; x++) {
                std::vector<Vector3f> image_block;
                Vector3f color;
                for (int i = 0;i <= 2;i++) {
                    for (int j = 0;j <= 2;j++) {
                        image_block.push_back(image.getPixel(3 * x + i, 3 * y + j));
                    }
                }
                for(int i = 0; i < 9; i++){
                    color += image_block[i] * gaussian_matrix[i];
                }
                color = color / 16;
                downsample_image.setPixel(x, y, color);

                std::vector<Vector3f> nimage_block;
                for(int i = 0;i <= 2;i++) {
                    for (int j = 0;j <= 2;j++) {
                        nimage_block.push_back(nimage.getPixel(3 * x + i, 3 * y + j));
                    }
                }
                Vector3f ncolor;
                for(int i = 0; i < 9; i++){
                    ncolor += nimage_block[i] * gaussian_matrix[i];
                }
                ncolor = ncolor / 16;
                downsample_nimage.setPixel(x, y, ncolor);

                std::vector<Vector3f> dimage_block;
                Vector3f dcolor;
                for(int i = 0;i <= 2;i++) {
                    for (int j = 0;j <= 2;j++) {
                        dimage_block.push_back(dimage.getPixel(3 * x + i, 3 * y + j));
                    }
                }
                for(int i = 0; i < 9; i++){
                    dcolor += dimage_block[i] * gaussian_matrix[i];
                }
                dcolor = dcolor / 16;
                downsample_dimage.setPixel(x, y, dcolor);   
            }
        }
        image = downsample_image;
        dimage = downsample_dimage;
        nimage = downsample_nimage;
    }

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
        Vector3f color,p;
        p = r.pointAtParameter(h.getT());
        color= _scene.getAmbientLight() * h.getMaterial()->getDiffuseColor();
        for(auto& light:_scene.lights){
            Vector3f tolight, intensity;
            float distToLight;//必须为float，否则调用getIllumination()时出错
            light->getIllumination(p, tolight, intensity, distToLight);
            Vector3f ILight(0);//先不调用shade(),避免无效计算
            if (_args.shadows) {
                Vector3f shadowOrigin = p;
                Ray shadowRay(shadowOrigin, tolight);
                Hit shadowHit;
                bool Intersected = _args.shadows&&_scene.getGroup()->intersect(shadowRay,0.0001f,shadowHit);//是否相交
                double distToIntersection = (shadowRay.pointAtParameter(shadowHit.getT()) - shadowOrigin).abs();
                if (Intersected&&distToIntersection<distToLight) {
                }
                else{
                    ILight = h.getMaterial()->shade(r, h, tolight, intensity);
                    color += ILight;
                }
            }
            else{//没有阴影参数，正常计算
                ILight = h.getMaterial()->shade(r, h, tolight, intensity);
                color += ILight;
            }
            ILight = h.getMaterial()->shade(r, h, tolight, intensity);
            color += ILight;
        }
        if(bounces>0){
            Vector3f N = h.getNormal().normalized();
            Vector3f E = r.getDirection().normalized();
            Vector3f R = E - 2 * Vector3f::dot(N,E) * N;
            R = R.normalized();
            // Vector3f R = (temp_ray + 2 * Vector3f::dot(N,-temp_ray) * N).normalized();//反射后的光线
            Ray newRay(p, R);
            // 创建新的光线对象，用于继续光线反射过程
            Hit new_h = Hit();
            Vector3f indirect = traceRay(newRay, 0.0001, bounces - 1, new_h);
            // 递归调用traceRay函数，计算间接光照
            color += h.getMaterial()->getSpecularColor() * indirect;
        }
        return color;
    }
    return _scene.getBackgroundColor(r.getDirection());
    
}