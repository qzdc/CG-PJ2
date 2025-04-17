#include "Material.h"
Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{
    // TODO implement Diffuse and Specular phong terms

    Vector3f diffuse;
    for (int i=0;i<3;i++){
        diffuse[i]=_diffuseColor[i]*lightIntensity[i]*std::max(0.0f,Vector3f::dot(hit.getNormal(),dirToLight));
    }

    Vector3f specular;
    auto E = ray.getDirection().normalized();
    auto R = E - 2*Vector3f::dot(E,hit.getNormal())*hit.getNormal();
    auto LRc=std::max(0.0f,Vector3f::dot(dirToLight,R));
    for (int i=0;i<3;i++){  
        specular[i]=_specularColor[i]*lightIntensity[i]*std::pow(LRc,_shininess);
    }

    return diffuse+specular;
}
