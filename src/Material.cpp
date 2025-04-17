#include "Material.h"
Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{
    // TODO implement Diffuse and Specular phong terms
    auto N = hit.getNormal().normalized();
    Vector3f diffuse=_diffuseColor*lightIntensity*std::max(0.0f,Vector3f::dot(N,dirToLight));

    Vector3f specular;
    auto E = ray.getDirection().normalized();
    auto R = E - 2*Vector3f::dot(E,N)*N;
    auto LRc=std::max(0.0f,Vector3f::dot(dirToLight,R));
    specular=_specularColor*lightIntensity*std::pow(LRc,_shininess);
    // for (int i=0;i<3;i++){  
    //     specular[i]=_specularColor[i]*lightIntensity[i]*std::pow(LRc,_shininess);
    // }

    return diffuse+specular;
}
