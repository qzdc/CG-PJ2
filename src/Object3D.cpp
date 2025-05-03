#include "Object3D.h"

bool Sphere::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER

    // We provide sphere intersection code for you.
    // You should model other intersection implementations after this one.

    // Locate intersection point ( 2 pts )
    const Vector3f &rayOrigin = r.getOrigin(); //Ray origin in the world coordinate
    const Vector3f &dir = r.getDirection();

    Vector3f origin = rayOrigin - _center;      //Ray origin in the sphere coordinate

    float a = dir.absSquared();
    float b = 2 * Vector3f::dot(dir, origin);
    float c = origin.absSquared() - _radius * _radius;

    // no intersection
    if (b * b - 4 * a * c < 0) {
        return false;
    }

    float d = sqrt(b * b - 4 * a * c);

    float tplus = (-b + d) / (2.0f*a);
    float tminus = (-b - d) / (2.0f*a);

    // the two intersections are at the camera back
    if ((tplus < tmin) && (tminus < tmin)) {
        return false;
    }

    float t = 10000;
    // the two intersections are at the camera front
    if (tminus > tmin) {
        t = tminus;
    }

    // one intersection at the front. one at the back 
    if ((tplus > tmin) && (tminus < tmin)) {
        t = tplus;
    }

    if (t < h.getT()) {
        Vector3f normal = r.pointAtParameter(t) - _center;
        normal = normal.normalized();
        h.set(t, this->material, normal);
        return true;
    }
    // END STARTER
    return false;
}

// Add object to group
void Group::addObject(Object3D *obj) {
    m_members.push_back(obj);
}

// Return number of objects in group
int Group::getGroupSize() const {
    return (int)m_members.size();
}

bool Group::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER
    // we implemented this for you
    bool hit = false;
    for (Object3D* o : m_members) {
        if (o->intersect(r, tmin, h)) {
            hit = true;
            if(h.getMaterial() == nullptr) {
                printf("?????\n");
                std::cout << o->getType() << std::endl;
                printf("?????\n");
            }
        }
    }
    return hit;
    // END STARTER
}


// Plane::Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
//     // TODO implement Plane constructor
// }
bool Plane::intersect(const Ray &r, float tmin, Hit &h) const
{
    auto O = r.getOrigin();
    auto D = r.getDirection();
    auto N = _normal.normalized();
    auto d = _d;
    auto t = -(-d + Vector3f::dot(N, O)) / Vector3f::dot(N, D);
    if (t < tmin) {
        return false;
    }
    if (t < h.getT()) {
        Vector3f normal = N;
        h.set(t, this->material, normal);
        return true;
    }
    return false;
}
bool Triangle::intersect(const Ray &r, float tmin, Hit &h) const 
{
    auto O = r.getOrigin();
    auto D = r.getDirection();

    Matrix3f A;
    A.setCol(0, -D);
    A.setCol(1, _v[1] - _v[0]);
    A.setCol(2, _v[2] - _v[0]);
    auto b = O - _v[0];
    auto x = A.inverse() * b;
    auto u=x[1], v=x[2], t=x[0];
    auto w=1-u-v;
    if (u<0 || v<0 || w<0 || u>1 || v>1 || w>1 || t<tmin || t>h.getT()) {
        return false;
    }
    if (t < h.getT()) {
        Vector3f normal = w*_normals[0] + u*_normals[1] + v*_normals[2];
        h.set(t, this->material, normal.normalized());
        return true;
    }
    return false;
}


// Transform::Transform(const Matrix4f &m,
//     Object3D *obj) : _object(obj) {
//     // TODO implement Transform constructor
// }
bool Transform::intersect(const Ray &r, float tmin, Hit &h) const
{
    auto O = r.getOrigin();
    auto D = r.getDirection();
    auto O_ = (_invTransform * Vector4f(O, 1)).xyz();
    auto D_ = (_invTransform * Vector4f(D, 0)).xyz();
    Ray r_(O_, D_);
    if (_object->intersect(r_, tmin, h)) {
        auto N = h.getNormal();
        Vector3f normal = (_invTransform.transposed() * Vector4f(N, 0)).xyz().normalized();
        h.set(h.getT(), h.getMaterial(), normal);
        return true;
    }
    return false;
}