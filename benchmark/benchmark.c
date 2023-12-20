#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#include "HandmadeMath.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h> 
#include <unistd.h> 

int mesh_to_v4_arr(HMM_Vec4 **vec, fastObjMesh *mesh) {
    int count = 0;
    *vec = calloc(mesh->position_count / 3, sizeof(HMM_Vec4));
    assert(*vec); 
    
    HMM_Vec4 *cursor = *vec;
    for (int i = 0; i < mesh->position_count - 2; i += 3) {
        *cursor++ = HMM_V4(mesh->positions[i], mesh->positions[i + 1],
                           mesh->positions[i + 2], 1.0f);
        ++count;
    }
    return count;
}

double calc_secs(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int run_transform_test(const char *obj_file) {
    // Load the mesh
    fastObjMesh *mesh = fast_obj_read(obj_file);
    // printf("Loaded mesh with:\n  %d faces\n", mesh->face_count);
    // printf("  %d verts\n", mesh->position_count);
    
    // convert the mesh to HMM_Vec4s
    HMM_Vec4 *positions;
    int pos_count = mesh_to_v4_arr(&positions, mesh);
    // assert(pos_count - 1 == mesh->position_count / 3);
    fast_obj_destroy(mesh);
    
    HMM_Mat4 look_at = HMM_LookAt_LH(HMM_V3(-1.0f, -1.0f, 1.0f),
                                     HMM_V3(0.0f, 0.0f, 0.0f),
                                     HMM_V3(0.0f, 0.0f, 1.0f));
    
    HMM_Mat4 perspective = HMM_Perspective_LH_NO(60.0f, 16.0f/9.0f, 0.001, 100.0f);
    HMM_Vec4 *transformed = calloc(pos_count, sizeof(HMM_Vec4));
    assert(transformed);
    
    int random_x_sum = 0;
    struct timespec start, end;
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    unsigned long counter = 0;
    do {
        for (int i = 0; i < pos_count; ++i) {
            transformed[i] = HMM_MulM4V4(perspective, HMM_MulM4V4(look_at, positions[i]));
        }
        counter++;
        
        // The compiler cant optimize out the array writes if we read it
        random_x_sum += transformed[rand() % pos_count].X;
        
        HMM_Vec4 *tmp = transformed;
        transformed = positions;
        positions = tmp;
        
        clock_gettime(CLOCK_MONOTONIC, &end);
    } while (calc_secs(start, end)< 10.0);
    
    free(positions);
    free(transformed);
    
    printf("Transformed %lu times in 10 seconds (%s)\n", counter, obj_file);
    return random_x_sum;
}

int main() {
    int variable = 0;
    printf("Running transform test:\n");
    
    variable += run_transform_test("assets/suzanne.obj"); // 500
    variable += run_transform_test("assets/teapot.obj"); //6320
    variable += run_transform_test("assets/stanford-bunny.obj"); // 69.4k
    variable += run_transform_test("assets/xyzrgb_dragon.obj"); // 1.1Mil
    
    return variable;
}