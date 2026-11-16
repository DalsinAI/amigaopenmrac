/* The AmiCradle Team's small stand-in for the GLM calls OpenMRac makes
 * (GLM itself is not part of GFOS): vec2/3/4 and mat3/4 of float, stored
 * as GLM stores them (a matrix is four columns), with GLM's meanings for
 * translate, rotate, scale, frustum, perspective, inverse, transpose,
 * normalize, cross, radians, make_vec2/3, value_ptr and pi.
 * tests/host/test_glm.cpp prints every call the game makes, and test_all.sh
 * requires the same output from GLM itself on the host.
 * Copyright (c) 2026 AmiCradle Team. BSD-2-Clause, like OpenMRac. */
#ifndef OMR_GLM_HPP
#define OMR_GLM_HPP
#include <cassert>   /* GLM brings it in, and the game counts on that */
#include <cmath>
#include <cstddef>

namespace glm {

typedef int length_t;

struct vec2 {
    float x, y;
    vec2() : x(0), y(0) {}
    explicit vec2(float s) : x(s), y(s) {}
    vec2(float a, float b) : x(a), y(b) {}
    float& operator[](length_t i) { return (&x)[i]; }
    const float& operator[](length_t i) const { return (&x)[i]; }
    static constexpr length_t length() { return 2; }
};

struct vec4;

struct vec3 {
    float x, y, z;
    vec3() : x(0), y(0), z(0) {}
    explicit vec3(float s) : x(s), y(s), z(s) {}
    vec3(float a, float b, float c) : x(a), y(b), z(c) {}
    vec3(const vec4& v);      // implicit, as in GLM
    float& operator[](length_t i) { return (&x)[i]; }
    const float& operator[](length_t i) const { return (&x)[i]; }
    static constexpr length_t length() { return 3; }
    vec3& operator+=(const vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    vec3& operator-=(const vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    vec3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }
};

struct vec4 {
    float x, y, z, w;
    vec4() : x(0), y(0), z(0), w(0) {}
    explicit vec4(float s) : x(s), y(s), z(s), w(s) {}
    vec4(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) {}
    vec4(const vec3& v, float d) : x(v.x), y(v.y), z(v.z), w(d) {}
    float& operator[](length_t i) { return (&x)[i]; }
    const float& operator[](length_t i) const { return (&x)[i]; }
    static constexpr length_t length() { return 4; }
    vec4& operator+=(const vec4& o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
    vec4& operator*=(float s) { x *= s; y *= s; z *= s; w *= s; return *this; }
};

inline vec3::vec3(const vec4& v) : x(v.x), y(v.y), z(v.z) {}

inline vec2 operator+(vec2 a, const vec2& b) { return vec2(a.x + b.x, a.y + b.y); }
inline vec2 operator-(vec2 a, const vec2& b) { return vec2(a.x - b.x, a.y - b.y); }
inline vec2 operator*(vec2 a, float s) { return vec2(a.x * s, a.y * s); }
inline vec3 operator+(vec3 a, const vec3& b) { return a += b; }
inline vec3 operator-(vec3 a, const vec3& b) { return a -= b; }
inline vec3 operator-(const vec3& a) { return vec3(-a.x, -a.y, -a.z); }
inline vec3 operator*(vec3 a, float s) { return a *= s; }
inline vec3 operator*(float s, vec3 a) { return a *= s; }
inline vec3 operator*(const vec3& a, const vec3& b) { return vec3(a.x * b.x, a.y * b.y, a.z * b.z); }
inline vec3 operator/(vec3 a, float s) { return a /= s; }
inline vec4 operator+(vec4 a, const vec4& b) { return a += b; }
inline vec4 operator*(vec4 a, float s) { return a *= s; }
inline vec4 operator*(float s, vec4 a) { return a *= s; }
inline bool operator==(const vec3& a, const vec3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }

inline float dot(const vec3& a, const vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float length(const vec3& v) { return std::sqrt(dot(v, v)); }
inline vec3 normalize(const vec3& v) { return v * (1.0f / std::sqrt(dot(v, v))); }
inline vec3 cross(const vec3& a, const vec3& b)
{
    return vec3(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y);
}

struct mat3 {
    vec3 c[3];
    mat3() { c[0] = vec3(1, 0, 0); c[1] = vec3(0, 1, 0); c[2] = vec3(0, 0, 1); }
    explicit mat3(float s) { c[0] = vec3(s, 0, 0); c[1] = vec3(0, s, 0); c[2] = vec3(0, 0, s); }
    mat3(const vec3& a, const vec3& b, const vec3& d) { c[0] = a; c[1] = b; c[2] = d; }
    inline mat3(const struct mat4& m);
    vec3& operator[](length_t i) { return c[i]; }
    const vec3& operator[](length_t i) const { return c[i]; }
};

struct mat4 {
    vec4 c[4];
    mat4() { c[0] = vec4(1, 0, 0, 0); c[1] = vec4(0, 1, 0, 0); c[2] = vec4(0, 0, 1, 0); c[3] = vec4(0, 0, 0, 1); }
    explicit mat4(float s) { c[0] = vec4(s, 0, 0, 0); c[1] = vec4(0, s, 0, 0); c[2] = vec4(0, 0, s, 0); c[3] = vec4(0, 0, 0, s); }
    mat4(const vec4& a, const vec4& b, const vec4& d, const vec4& e) { c[0] = a; c[1] = b; c[2] = d; c[3] = e; }
    mat4(const mat3& m)
    {
        c[0] = vec4(m[0], 0); c[1] = vec4(m[1], 0); c[2] = vec4(m[2], 0); c[3] = vec4(0, 0, 0, 1);
    }
    vec4& operator[](length_t i) { return c[i]; }
    const vec4& operator[](length_t i) const { return c[i]; }
    mat4& operator*=(const mat4& o);
};

inline mat3::mat3(const mat4& m) { c[0] = vec3(m[0]); c[1] = vec3(m[1]); c[2] = vec3(m[2]); }

/* GLM adds the four columns in pairs (type_mat4x4.inl) */
inline vec4 operator*(const mat4& m, const vec4& v)
{
    return (m[0] * v.x + m[1] * v.y) + (m[2] * v.z + m[3] * v.w);
}
inline vec3 operator*(const mat3& m, const vec3& v)
{
    return m[0] * v.x + m[1] * v.y + m[2] * v.z;
}
/* and a product's columns from left to right */
inline mat4 operator*(const mat4& a, const mat4& b)
{
    mat4 r;
    for (int i = 0; i < 4; ++i)
        r[i] = a[0] * b[i][0] + a[1] * b[i][1] + a[2] * b[i][2] + a[3] * b[i][3];
    return r;
}
inline mat3 operator*(const mat3& a, const mat3& b)
{
    return mat3(a * b[0], a * b[1], a * b[2]);
}
inline mat4& mat4::operator*=(const mat4& o) { return *this = *this * o; }

inline mat4 transpose(const mat4& m)
{
    mat4 r;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r[i][j] = m[j][i];
    return r;
}
inline mat3 transpose(const mat3& m)
{
    mat3 r;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            r[i][j] = m[j][i];
    return r;
}

/* GLM's own order of operations (func_matrix.inl), so the floats match */
inline mat3 inverse(const mat3& m)
{
    float od = 1.0f / (+ m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
                       - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
                       + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));
    mat3 r;
    r[0][0] = + (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * od;
    r[1][0] = - (m[1][0] * m[2][2] - m[2][0] * m[1][2]) * od;
    r[2][0] = + (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * od;
    r[0][1] = - (m[0][1] * m[2][2] - m[2][1] * m[0][2]) * od;
    r[1][1] = + (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * od;
    r[2][1] = - (m[0][0] * m[2][1] - m[2][0] * m[0][1]) * od;
    r[0][2] = + (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * od;
    r[1][2] = - (m[0][0] * m[1][2] - m[1][0] * m[0][2]) * od;
    r[2][2] = + (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * od;
    return r;
}

inline mat4 inverse(const mat4& m)
{
    float c00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
    float c02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
    float c03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
    float c04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
    float c06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
    float c07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
    float c08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    float c10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
    float c11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
    float c12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
    float c14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
    float c15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
    float c16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    float c18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
    float c19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
    float c20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    float c22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    float c23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];
    vec4 f0(c00, c00, c02, c03), f1(c04, c04, c06, c07), f2(c08, c08, c10, c11);
    vec4 f3(c12, c12, c14, c15), f4(c16, c16, c18, c19), f5(c20, c20, c22, c23);
    vec4 v0(m[1][0], m[0][0], m[0][0], m[0][0]);
    vec4 v1(m[1][1], m[0][1], m[0][1], m[0][1]);
    vec4 v2(m[1][2], m[0][2], m[0][2], m[0][2]);
    vec4 v3(m[1][3], m[0][3], m[0][3], m[0][3]);
    auto mul = [](const vec4& a, const vec4& b) { return vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); };
    auto sub = [](const vec4& a, const vec4& b) { return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); };
    vec4 i0 = sub(mul(v1, f0), mul(v2, f1)) + mul(v3, f2);
    vec4 i1 = sub(mul(v0, f0), mul(v2, f3)) + mul(v3, f4);
    vec4 i2 = sub(mul(v0, f1), mul(v1, f3)) + mul(v3, f5);
    vec4 i3 = sub(mul(v0, f2), mul(v1, f4)) + mul(v2, f5);
    vec4 sa(+1, -1, +1, -1), sb(-1, +1, -1, +1);
    mat4 inv(mul(i0, sa), mul(i1, sb), mul(i2, sa), mul(i3, sb));
    vec4 r0(inv[0][0], inv[1][0], inv[2][0], inv[3][0]);
    vec4 d0 = mul(m[0], r0);
    float d1 = (d0.x + d0.y) + (d0.z + d0.w);
    float od = 1.0f / d1;
    return mat4(inv[0] * od, inv[1] * od, inv[2] * od, inv[3] * od);
}


template <typename T> inline T pi() { return T(3.14159265358979323846264338327950288); }
inline float radians(float deg) { return deg * 0.01745329251994329576923690768489f; }

inline mat4 translate(const mat4& m, const vec3& v)
{
    mat4 r(m);
    r[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
    return r;
}

inline mat4 scale(const mat4& m, const vec3& v)
{
    return mat4(m[0] * v[0], m[1] * v[1], m[2] * v[2], m[3]);
}

inline mat4 rotate(const mat4& m, float angle, const vec3& v)
{
    const float a = angle, c = std::cos(a), s = std::sin(a);
    vec3 axis(normalize(v));
    vec3 temp(axis * (1.0f - c));
    mat4 rot;
    rot[0][0] = c + temp[0] * axis[0];
    rot[0][1] = temp[0] * axis[1] + s * axis[2];
    rot[0][2] = temp[0] * axis[2] - s * axis[1];
    rot[1][0] = temp[1] * axis[0] - s * axis[2];
    rot[1][1] = c + temp[1] * axis[1];
    rot[1][2] = temp[1] * axis[2] + s * axis[0];
    rot[2][0] = temp[2] * axis[0] + s * axis[1];
    rot[2][1] = temp[2] * axis[1] - s * axis[0];
    rot[2][2] = c + temp[2] * axis[2];
    mat4 r;
    r[0] = m[0] * rot[0][0] + m[1] * rot[0][1] + m[2] * rot[0][2];
    r[1] = m[0] * rot[1][0] + m[1] * rot[1][1] + m[2] * rot[1][2];
    r[2] = m[0] * rot[2][0] + m[1] * rot[2][1] + m[2] * rot[2][2];
    r[3] = m[3];
    return r;
}

/* right-handed, depth -1..1: GLM's defaults */
inline mat4 frustum(float l, float r, float b, float t, float n, float f)
{
    mat4 m(0.0f);
    m[0][0] = (2.0f * n) / (r - l);
    m[1][1] = (2.0f * n) / (t - b);
    m[2][0] = (r + l) / (r - l);
    m[2][1] = (t + b) / (t - b);
    m[2][2] = -(f + n) / (f - n);
    m[2][3] = -1.0f;
    m[3][2] = -(2.0f * f * n) / (f - n);
    return m;
}

inline mat4 perspective(float fovy, float aspect, float n, float f)
{
    const float th = std::tan(fovy / 2.0f);
    mat4 m(0.0f);
    m[0][0] = 1.0f / (aspect * th);
    m[1][1] = 1.0f / th;
    m[2][2] = -(f + n) / (f - n);
    m[2][3] = -1.0f;
    m[3][2] = -(2.0f * f * n) / (f - n);
    return m;
}

inline vec2 make_vec2(const float* p) { return vec2(p[0], p[1]); }
inline vec3 make_vec3(const float* p) { return vec3(p[0], p[1], p[2]); }
inline float* value_ptr(mat4& m) { return &m[0].x; }
inline const float* value_ptr(const mat4& m) { return &m[0].x; }
inline float* value_ptr(mat3& m) { return &m[0].x; }
inline const float* value_ptr(const mat3& m) { return &m[0].x; }
inline float* value_ptr(vec3& v) { return &v.x; }
inline const float* value_ptr(const vec3& v) { return &v.x; }
inline float* value_ptr(vec4& v) { return &v.x; }
inline const float* value_ptr(const vec4& v) { return &v.x; }

} // namespace glm
#endif
