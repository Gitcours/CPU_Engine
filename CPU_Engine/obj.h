
#pragma once

#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

// ============================================================
//  obj.h — Minimal Wavefront OBJ loader (Chapter 6)
// ------------------------------------------------------------
//  Supported lines:
//    v x y z  -> vertex
//    f i j k  -> triangular face
//
//  Face tokens can be "i", "i/t", or "i/t/n".
//  OBJ indices are converted from 1-based to 0-based.
//
//  Limitations:
//    - Triangular faces only
//    - Positive vertex indices only
//    - No materials or textures
// ============================================================

typedef struct {
    vec3* verts;     int vert_count;
    vec2* uvs;       int uv_count;     // Wavefront 'vt' lines
    int* faces;     int face_count;   // 3 vertex indices per face (0-based)
    int* face_uvs;
} Mesh;

// Forward declaration
static void mesh_free(Mesh* m);

// ------------------------------------------------------------
// Read the leading integer of a face token.
//
// Examples:
//   "12"     -> 12
//   "12/3/4" -> 12
//
// Advances *p to the next token.
// ------------------------------------------------------------
static int obj__read_index(const char** p) {
    // Skip whitespace
    while (**p == ' ' || **p == '\t') {
        (*p)++;
    }

    int sign = 1;

    if (**p == '-') {
        sign = -1;
        (*p)++;
    }

    int value = 0;

    while (**p >= '0' && **p <= '9') {
        value = value * 10 + (**p - '0');
        (*p)++;
    }

    // Skip texture/normal information
    while (**p &&
        **p != ' ' &&
        **p != '\t' &&
        **p != '\n' &&
        **p != '\r') {
        (*p)++;
    }

    return sign * value;
}

// ------------------------------------------------------------
// Load a mesh from an OBJ file.
//
// Returns:
//   1 -> success
//   0 -> failure
// ------------------------------------------------------------
static int mesh_load(const char* path, Mesh* out) {
    if (path == NULL || out == NULL) {
        return 0;
    }

    // Initialize output
    out->verts = NULL;
    out->vert_count = 0;
    out->faces = NULL;
    out->face_count = 0;

    FILE* f = fopen(path, "r");

    if (f == NULL) {
        fprintf(stderr, "Could not open %s\n", path);
        return 0;
    }

    // Initial capacities
    int vcap = 1024;
    int fcap = 1024;

    // Allocate initial arrays
    out->verts = (vec3*)malloc(
        (size_t)vcap * sizeof(vec3)
    );

    out->faces = (int*)malloc(
        (size_t)fcap * 3 * sizeof(int)
    );

    // Check allocation failures
    if (out->verts == NULL || out->faces == NULL) {
        fprintf(stderr, "Memory allocation failed\n");

        mesh_free(out);
        fclose(f);
        return 0;
    }

    char line[256];

    while (fgets(line, sizeof line, f) != NULL) {

        // ----------------------------------------------------
        // Vertex: v x y z
        // ----------------------------------------------------
        if (line[0] == 'v' && line[1] == ' ') {
            vec3 v;

            if (sscanf(
                line + 2,
                "%f %f %f",
                &v.x,
                &v.y,
                &v.z
            ) == 3) {

                // Grow vertex array if necessary
                if (out->vert_count == vcap) {
                    if (vcap > 2147483647 / 2) {
                        fprintf(stderr, "Too many vertices\n");
                        mesh_free(out);
                        fclose(f);
                        return 0;
                    }

                    int new_cap = vcap * 2;

                    vec3* temp = (vec3*)realloc(
                        out->verts,
                        (size_t)new_cap * sizeof(vec3)
                    );

                    if (temp == NULL) {
                        fprintf(stderr,
                            "Failed to resize vertex array\n");

                        mesh_free(out);
                        fclose(f);
                        return 0;
                    }

                    out->verts = temp;
                    vcap = new_cap;
                }

                out->verts[out->vert_count++] = v;
            }
        }

        // ----------------------------------------------------
        // Face: f i j k
        // ----------------------------------------------------
        else if (line[0] == 'f' && line[1] == ' ') {
            const char* p = line + 2;

            int a = obj__read_index(&p) - 1;
            int b = obj__read_index(&p) - 1;
            int c = obj__read_index(&p) - 1;

            // Grow face array if necessary
            if (out->face_count == fcap) {
                if (fcap > 2147483647 / 2) {
                    fprintf(stderr, "Too many faces\n");
                    mesh_free(out);
                    fclose(f);
                    return 0;
                }

                int new_cap = fcap * 2;

                int* temp = (int*)realloc(
                    out->faces,
                    (size_t)new_cap * 3 * sizeof(int)
                );

                if (temp == NULL) {
                    fprintf(stderr,
                        "Failed to resize face array\n");

                    mesh_free(out);
                    fclose(f);
                    return 0;
                }

                out->faces = temp;
                fcap = new_cap;
            }

            int i = out->face_count * 3;

            out->faces[i] = a;
            out->faces[i + 1] = b;
            out->faces[i + 2] = c;

            out->face_count++;
        }

        // All other line types are ignored
    }

    // Close file
    fclose(f);

    return 1;
}

// ------------------------------------------------------------
// Free all mesh memory
// ------------------------------------------------------------
static void mesh_free(Mesh* m) {
    if (m == NULL) {
        return;
    }

    free(m->verts);
    free(m->faces);

    m->verts = NULL;
    m->faces = NULL;

    m->vert_count = 0;
    m->face_count = 0;
}
