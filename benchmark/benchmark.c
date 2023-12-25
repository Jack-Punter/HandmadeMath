#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#include "HandmadeMath.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h> 
#include <unistd.h> 
#include <float.h>

typedef struct Vec4Array {
    HMM_Vec4 *data;
    long      count; 
} Vec4Array;

Vec4Array LoadObjMesh(const char *obj_file) {
    // Load the mesh
    fastObjMesh *mesh = fast_obj_read(obj_file);
    
    // convert the mesh to HMM_Vec4s
    Vec4Array result = {};
    result.data = calloc(mesh->position_count, sizeof(HMM_Vec4));
    result.count = mesh->position_count;
    assert(result.data); 
    
    HMM_Vec4 *cursor = result.data;
    for (unsigned int i = 0; i < mesh->position_count; ++i) {
        unsigned int pos_start = i * 3; 
        *cursor++ = HMM_V4(mesh->positions[pos_start],
                           mesh->positions[pos_start + 1],
                           mesh->positions[pos_start + 2], 1.0f);
    }

    fast_obj_destroy(mesh);

    return result;
}

void DeleteObjMesh(Vec4Array *array) {
    free(array->data);
    array->data = 0;
    array->count = 0;
}

double CalclateDurationS(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int RunTransformTest(const char *obj_file, double duration) {
    
    Vec4Array verts = LoadObjMesh(obj_file);

    HMM_Mat4 look_at = HMM_LookAt_LH(HMM_V3(-1.0f, -1.0f, 1.0f),
                                     HMM_V3(0.0f, 0.0f, 0.0f),
                                     HMM_V3(0.0f, 0.0f, 1.0f));
    
    HMM_Mat4 perspective = HMM_Perspective_LH_NO(60.0f, 16.0f/9.0f, 0.001, 100.0f);
    Vec4Array transformed = {};
    transformed.data = calloc(verts.count, sizeof(HMM_Vec4));
    transformed.count = verts.count;
    assert(transformed.data);
    
    int random_x_sum = 0;
    struct timespec start, last, end;
    
    double average_time = 0.0;
    double min_time = DBL_MAX;
    double max_time = 0.0f;
    clock_gettime(CLOCK_MONOTONIC, &start);
    last = start; 
    unsigned long counter = 0;

    // This is essentially our main loop, eventually I'd like to copy this into a simple
    // rendering engine and create some kind of scene with a camera following some kind
    // of path around a scene with a few objects to look at, and create an implementaiton 
    // that uses GLM and potentially some other common math libs and compare the results.
    do {
        HMM_Vec4 *in_cursor = verts.data;
        HMM_Vec4 *out_cursor = transformed.data;
        for (long i = 0; i < verts.count; ++i) {
            *out_cursor++ = HMM_MulM4V4(perspective, HMM_MulM4V4(look_at, *in_cursor++));
        }
        counter++;

        clock_gettime(CLOCK_MONOTONIC, &end);
        double this_time = CalclateDurationS(last, end);
        min_time = (min_time < this_time) ? min_time : this_time;
        max_time = (max_time > this_time) ? max_time : this_time;
        average_time += this_time;
        last = end;
        
        // The compiler can't optimise the calculation away if it doesn't know
        // what we're going to read
        random_x_sum += transformed.data[rand() % transformed.count].X;
    } while (CalclateDurationS(start, end) < duration);

    // convert summed avarage into average us time
    average_time *= 1e6 / counter;
    min_time *= 1e6;
    max_time *= 1e6;
    DeleteObjMesh(&verts);
    DeleteObjMesh(&transformed);
    
    printf("%10lu|%10.3f|%10.3f|%10.3f|%s\n",
           counter, min_time, max_time, average_time,
           obj_file + strlen("assets/"));
    return random_x_sum;
}

int main() {
    int variable = 0;
    double testDuration = 5;
    printf("Running transform test for %.2fs per object:\n", testDuration);
    printf("%10s|%10s|%10s|%10s|%s\n", "Num Frames", "min us", "max us", "average us", "asset");
    variable += RunTransformTest("assets/suzanne.obj", testDuration);
    variable += RunTransformTest("assets/teapot.obj", testDuration);
    variable += RunTransformTest("assets/stanford-bunny.obj", testDuration);
    variable += RunTransformTest("assets/xyzrgb_dragon.obj", testDuration);
    return 0;
}
