// ============================================================
//  mat.h — 4x4 matrix type and operations
// ------------------------------------------------------------
//  See chapter README sections 4.8 to 4.10. We use 4x4 (not
//  3x3) so a single matrix can rotate, scale, AND translate
//  via homogeneous coordinates — and, from chapter 5, project.
//
//  Storage is row-major: m[row][col].
// ============================================================
#pragma once
#include "vec.h"
#include <math.h>

typedef struct { float m[4][4]; } mat4;

// The identity matrix: the "do nothing" machine. Applying it to a
// point returns the point unchanged.
static inline mat4 mat4_identity(void) {
    mat4 r = {{{0}}};
    r.m[0][0] = 1.0f;
    r.m[1][1] = 1.0f;
    r.m[2][2] = 1.0f;
    r.m[3][3] = 1.0f;
    return r;
}

// Multiply two matrices: combine two machines into one.
// (a * b) applies b first, then a, to whatever it acts on.
static inline mat4 mat4_mul(mat4 a, mat4 b) {
    mat4 r = {{{0}}};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++)
                sum += a.m[i][k] * b.m[k][j];
            r.m[i][j] = sum;
        }
    return r;
}

// Apply a matrix to a POINT (homogeneous w = 1, so translation
// affects it). The 4th output coordinate (w) is computed but, for
// the affine matrices in this chapter, stays 1, so we just return
// xyz. Chapter 5's projection matrix will make w meaningful.
static inline vec3 mat4_apply_point(mat4 a, vec3 p) {
    float x = a.m[0][0]*p.x + a.m[0][1]*p.y + a.m[0][2]*p.z + a.m[0][3];
    float y = a.m[1][0]*p.x + a.m[1][1]*p.y + a.m[1][2]*p.z + a.m[1][3];
    float z = a.m[2][0]*p.x + a.m[2][1]*p.y + a.m[2][2]*p.z + a.m[2][3];
    return vec3_make(x, y, z);
}

// Apply a matrix to a DIRECTION (homogeneous w = 0, so the
// translation column is ignored). Use this for normals and other
// "which way does it point" vectors.
static inline vec3 mat4_apply_dir(mat4 a, vec3 v) {
    float x = a.m[0][0]*v.x + a.m[0][1]*v.y + a.m[0][2]*v.z;
    float y = a.m[1][0]*v.x + a.m[1][1]*v.y + a.m[1][2]*v.z;
    float z = a.m[2][0]*v.x + a.m[2][1]*v.y + a.m[2][2]*v.z;
    return vec3_make(x, y, z);
}

// Translation: slide points by (tx, ty, tz). The offset lives in
// the 4th column. (No effect on directions — see mat4_apply_dir.)
static inline mat4 mat4_translation(float tx, float ty, float tz) {
    mat4 r = mat4_identity();
    r.m[0][3] = tx;
    r.m[1][3] = ty;
    r.m[2][3] = tz;
    return r;
}

// Uniform scale by s on all three axes.
static inline mat4 mat4_scale(float s) {
    mat4 r = mat4_identity();
    r.m[0][0] = s;
    r.m[1][1] = s;
    r.m[2][2] = s;
    return r;
}

// Rotation about the Z axis by angle a (radians). Columns record
// where the x and y axes land. See README section 4.8.
//
// Convention: this is the standard math rotation matrix. With the
// math y-up axis, a positive angle turns counter-clockwise. Our
// framebuffer is y-down (chapter 1), so on screen the same matrix
// turns clockwise. The matrix is identical either way; only the
// visual direction differs. (rotation_x/y below follow the same
// standard convention.)
static inline mat4 mat4_rotation_z(float a) {
    mat4 r = mat4_identity();
    float c = cosf(a), s = sinf(a);
    r.m[0][0] = c; r.m[0][1] = -s;
    r.m[1][0] = s; r.m[1][1] =  c;
    return r;
}

// Rotation about the X axis (y and z rotate, x stays put).
static inline mat4 mat4_rotation_x(float a) {
    mat4 r = mat4_identity();
    float c = cosf(a), s = sinf(a);
    r.m[1][1] = c; r.m[1][2] = -s;
    r.m[2][1] = s; r.m[2][2] =  c;
    return r;
}

// Rotation about the Y axis (x and z rotate, y stays put).
static inline mat4 mat4_rotation_y(float a) {
    mat4 r = mat4_identity();
    float c = cosf(a), s = sinf(a);
    r.m[0][0] =  c; r.m[0][2] = s;
    r.m[2][0] = -s; r.m[2][2] = c;
    return r;
}