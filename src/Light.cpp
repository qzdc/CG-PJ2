#include "Light.h"
    void DirectionalLight::getIllumination(const Vector3f &p, 
                                 Vector3f &tolight, 
                                 Vector3f &intensity, 
                                 float &distToLight) const
    {
        // the direction to the light is the opposite of the
        // direction of the directional light source

        // BEGIN STARTER
        tolight = -_direction;
        intensity  = _color;
        distToLight = std::numeric_limits<float>::max();
        // END STARTER
    }
    void PointLight::getIllumination(const Vector3f &p, 
                                 Vector3f &tolight, 
                                 Vector3f &intensity, 
                                 float &distToLight) const
    {
        // TODO Implement point light source
        auto d=_position-p;

        tolight=d.normalized();

        distToLight = d.abs();

        intensity=_color/(_falloff*distToLight*distToLight);

        // tolight, intensity, distToLight are outputs
    }

