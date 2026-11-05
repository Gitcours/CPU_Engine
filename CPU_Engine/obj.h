#pragma once

#include "vec.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Mesh{ //pitier, mettez les noms a coter de "struct", c'est vraiment horrible de mettre ca a la fin
    vec3* verts;
    int vert_count;
    vec2* uvs;
    int uv_count;
    vec3* normals;
    int normal_count;
    int* faces;
    int face_count;
    int* face_uvs;
    int* face_normals;
};

static int obj__resolve_index(int index, int count)
{
    if (index > 0) return index - 1;
    if (index < 0) return count + index;
    return -1;
}

static void obj__parse_face_token(
    const char** p, int* vi, int* ti, int* ni,
    int vert_count, int uv_count, int normal_count)
{
    while (**p == ' ' || **p == '\t') ++(*p);

    int raw_v = atoi(*p);
    while (**p && **p != '/' && **p != ' ' && **p != '\t' && **p != '\n' && **p != '\r') ++(*p);

    int raw_t = 0, raw_n = 0;
    if (**p == '/') {
        ++(*p);
        if (**p != '/') {
            raw_t = atoi(*p);
            while (**p && **p != '/' && **p != ' ' && **p != '\t' && **p != '\n' && **p != '\r') ++(*p);
        }
        if (**p == '/') {
            ++(*p);
            raw_n = atoi(*p);
            while (**p && **p != ' ' && **p != '\t' && **p != '\n' && **p != '\r') ++(*p);
        }
    }

    *vi = obj__resolve_index(raw_v, vert_count);
    *ti = raw_t ? obj__resolve_index(raw_t, uv_count) : -1;
    *ni = raw_n ? obj__resolve_index(raw_n, normal_count) : -1;
}

static int obj__add_triangle(Mesh* out, int* fcap, int va, int ta, int na, int vb, int tb, int nb, int vc, int tc, int nc)
{
    if (out->face_count >= *fcap) {
        int new_cap = (*fcap) * 2;
        int* new_faces = (int*)realloc(out->faces, (size_t)new_cap * 3 * sizeof(int));
        if (!new_faces) return 0;
        out->faces = new_faces;

        int* new_uvs = (int*)realloc(out->face_uvs, (size_t)new_cap * 3 * sizeof(int));
        if (!new_uvs) return 0;
        out->face_uvs = new_uvs;

        int* new_normals = (int*)realloc(out->face_normals, (size_t)new_cap * 3 * sizeof(int));
        if (!new_normals) return 0;
        out->face_normals = new_normals;
        *fcap = new_cap;
    }

    int fi = out->face_count * 3;
    out->faces[fi + 0] = va; out->faces[fi + 1] = vb; out->faces[fi + 2] = vc;
    out->face_uvs[fi + 0] = ta; out->face_uvs[fi + 1] = tb; out->face_uvs[fi + 2] = tc;
    out->face_normals[fi + 0] = na; out->face_normals[fi + 1] = nb; out->face_normals[fi + 2] = nc;
    out->face_count++;
    return 1;
}

static void mesh_free(Mesh* m)
{
    free(m->verts);
    free(m->uvs);
    free(m->faces);
    free(m->face_uvs);
    free(m->normals);
    free(m->face_normals);

    //pas besoin de faire m->verts = NULL, m->uv_count = 0... memset set tout la memoir ici de *m a 0, ca permet de rendre le code plus clean (par contre je sais pas ce que ca vaut niveau performance) par contre faut quand meme utiliser free();
    memset(m, 0, sizeof(*m));
}

static int mesh_load(const char* path, Mesh* out)
{
    memset(out, 0, sizeof(*out));
    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "could not open %s\n", path);
        return 0;
    }

    int vcap = 1024, vtcap = 1024, vncap = 1024, fcap = 1024;
    out->verts = (vec3*)malloc((size_t)vcap * sizeof(vec3));
    out->uvs = (vec2*)malloc((size_t)vtcap * sizeof(vec2));
    out->normals = (vec3*)malloc((size_t)vncap * sizeof(vec3));
    out->faces = (int*)malloc((size_t)fcap * 3 * sizeof(int));
    out->face_uvs = (int*)malloc((size_t)fcap * 3 * sizeof(int));
    out->face_normals = (int*)malloc((size_t)fcap * 3 * sizeof(int));
    if (!out->verts || !out->uvs || !out->normals || !out->faces || !out->face_uvs || !out->face_normals) {
        fclose(f); mesh_free(out); return 0;
    }

    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == 'v' && line[1] == ' ') {
            vec3 v;
            if (sscanf(line + 2, "%f %f %f", &v.x, &v.y, &v.z) == 3) {
                if (out->vert_count == vcap) {
                    vcap *= 2;
                    vec3* p = (vec3*)realloc(out->verts, (size_t)vcap * sizeof(vec3));
                    if (!p) { fclose(f); mesh_free(out); return 0; }
                    out->verts = p;
                }
                out->verts[out->vert_count++] = v;
            }
        }
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
            vec2 uv;
            if (sscanf(line + 3, "%f %f", &uv.x, &uv.y) >= 2) {
                if (out->uv_count == vtcap) {
                    vtcap *= 2;
                    vec2* p = (vec2*)realloc(out->uvs, (size_t)vtcap * sizeof(vec2));
                    if (!p) { fclose(f); mesh_free(out); return 0; }
                    out->uvs = p;
                }
                out->uvs[out->uv_count++] = uv;
            }
        }
        else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
            vec3 n;
            if (sscanf(line + 3, "%f %f %f", &n.x, &n.y, &n.z) == 3) {
                if (out->normal_count == vncap) {
                    vncap *= 2;
                    vec3* p = (vec3*)realloc(out->normals, (size_t)vncap * sizeof(vec3));
                    if (!p) { fclose(f); mesh_free(out); return 0; }
                    out->normals = p;
                }
                out->normals[out->normal_count++] = vec3_normalize(n);
            }
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            const char* p = line + 2;
            const int MAX_FACE_VERTS = 128;
            int vis[MAX_FACE_VERTS], tis[MAX_FACE_VERTS], nis[MAX_FACE_VERTS];
            int count = 0;
            while (*p && *p != '\n' && *p != '\r' && *p != '#') {
                while (*p == ' ' || *p == '\t') ++p;
                if (!*p || *p == '\n' || *p == '\r' || *p == '#') break;
                if (count == MAX_FACE_VERTS) {
                    fprintf(stderr, "OBJ: face exceeds %d vertices\n", MAX_FACE_VERTS);
                    break;
                }
                int vi, ti, ni;
                obj__parse_face_token(&p, &vi, &ti, &ni,
                    out->vert_count, out->uv_count, out->normal_count);
                if (vi < 0 || vi >= out->vert_count) {
                    fprintf(stderr, "OBJ: invalid vertex index in face\n");
                    count = 0; break;
                }
                if (ti < -1 || ti >= out->uv_count) ti = -1;
                if (ni < -1 || ni >= out->normal_count) ni = -1;
                vis[count] = vi; tis[count] = ti; nis[count] = ni;
                ++count;
            }
            for (int i = 1; i + 1 < count; ++i) {
                if (!obj__add_triangle(out, &fcap,
                    vis[0], tis[0], nis[0],
                    vis[i], tis[i], nis[i],
                    vis[i + 1], tis[i + 1], nis[i + 1])) {
                    fclose(f); mesh_free(out); return 0;
                }
            }
        }
    }

    fclose(f);
    printf("OBJ loaded: %d vertices, %d UVs, %d normals, %d triangles\n",
        out->vert_count, out->uv_count, out->normal_count, out->face_count);
    return 1;
}