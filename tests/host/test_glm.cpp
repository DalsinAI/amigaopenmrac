// Host test of amiga/compat/glm (our stand-in for the GLM calls OpenMRac
// makes). Built twice by tests/host/test_all.sh, on our headers and on the
// host's GLM, with the same compiler and flags; the two outputs must be the
// same, value for value (printed with nine significant digits, the whole
// of a float). The inputs are the kinds the game gives: its camera and
// model matrices (translate, rotate, scale, their products), its
// projections (frustum, perspective), the normal matrix
// transpose(inverse(modelview)) as a mat3, vectors through matrices,
// normals, tangents and cross products from model data.
// Copyright (c) 2026 AmiCradle Team. BSD-2-Clause, like OpenMRac.
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>

static void p(const char* n, const float* f, int k)
{
    printf("%s:", n);
    for (int i = 0; i < k; ++i)
        printf(" %.9g", f[i]);
    printf("\n");
}
static void p(const char* n, const glm::mat4& m) { p(n, glm::value_ptr(m), 16); }
static void p(const char* n, const glm::mat3& m) { p(n, glm::value_ptr(m), 9); }
static void p(const char* n, const glm::vec3& v) { p(n, &v[0], 3); }
static void p(const char* n, const glm::vec4& v) { p(n, &v[0], 4); }

int main()
{
    const float data[] = { 1.5f, -2.25f, 3.0f, 0.3f, 0.7f, -0.2f, 10.0f, 0.01f, -4.0f };
    glm::vec3 a = glm::make_vec3(data), b = glm::make_vec3(data + 3), c = glm::make_vec3(data + 6);
    glm::vec2 t = glm::make_vec2(data + 1);
    printf("t: %.9g %.9g\n", t[0], t[1]);
    p("a+b", a + b); p("a-b", a - b); p("a*2.5", a * 2.5f); p("a/3", a / 3.0f); p("-a", -a);
    p("a*b", a * b);
    p("cross", glm::cross(a, b));
    p("normalize", glm::normalize(c));
    p("normalize cross", glm::normalize(glm::cross(b - a, c - a)));
    printf("radians: %.9g %.9g %.9g\n", glm::radians(90.0f), glm::radians(-37.5f), glm::radians(720.0f));
    printf("pi: %.9g\n", glm::pi<float>());

    glm::mat4 m(1.0f);
    p("identity", m);
    m = glm::translate(m, glm::vec3(1.0f, -2.0f, 0.5f));
    p("translate", m);
    m = glm::rotate(m, glm::radians(33.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    p("rotate y", m);
    m = glm::rotate(m, -0.7f, glm::vec3(1.0f, 0.2f, -0.4f));
    p("rotate any", m);
    m = glm::scale(m, glm::vec3(2.0f, 0.5f, -1.0f));
    p("scale", m);
    glm::mat4 f = glm::frustum(-0.1f, 0.1f, -0.075f, 0.075f, 0.1f, 1000.0f);
    p("frustum", f);
    glm::mat4 q = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.5f, 500.0f);
    p("perspective", q);
    glm::mat4 pmv = f * m;
    p("f*m", pmv);
    p("q*m*m", q * m * m);
    glm::mat4 mm = m;
    mm *= q;
    p("m*=q", mm);
    p("transpose", glm::transpose(m));
    p("inverse", glm::inverse(m));
    glm::mat3 norm = glm::transpose(glm::inverse(m));
    p("normal mat3", norm);
    glm::mat3 m3 = m;
    p("mat3(m)", m3);
    p("inverse mat3", glm::inverse(m3));
    p("transpose mat3", glm::transpose(m3));
    p("m3*m3", m3 * glm::transpose(m3));
    glm::mat4 back = m3;
    p("mat4(mat3)", back);
    glm::vec4 v(a, 1.0f);
    p("m*v", m * v);
    p("f*m*v", pmv * v);
    glm::vec3 w = m * glm::vec4(b, 0.0f);
    p("vec3(m*v)", w);
    p("m3*v", m3 * c);
    p("m[3]", m[3]);
    m[3][1] = 7.0f;
    p("m[3][1]=7", m);
    // a camera like the game's: look along a rotated direction, then move
    glm::mat4 cam(1.0f);
    for (int i = 0; i < 8; ++i) {
        cam = glm::rotate(cam, glm::radians(11.25f * (i + 1)), glm::vec3(0, 0, 1));
        cam = glm::translate(cam, glm::vec3(0.5f * i, -0.25f, 1.0f / (i + 1)));
    }
    p("camera", cam);
    p("camera normal", glm::mat3(glm::transpose(glm::inverse(cam))));
    return 0;
}
