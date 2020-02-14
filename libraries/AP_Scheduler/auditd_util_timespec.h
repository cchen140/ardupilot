#ifndef _UTILITY_TIMESPEC
#define _UTILITY_TIMESPEC

#include <time.h>	// For timespec and clock_gettime(). Remember to use -lrt when compiling for ARM CPU.
#include <signal.h>

//#include <stdint.h>	// for typedef like uint32_t
//typedef uint32_t u32;

typedef unsigned long long u64;

typedef struct {
    char *name;
    u64* timings; 
    int buffer_capacity;
    int current_buffer;
} Task_Timing;

void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result);
void timespec_deep_copy(struct timespec *src, struct timespec *dest);
void timespec_get_time(struct timespec *instance);
__uint32_t timespec_get_micro_u64(struct timespec *in);

void setup_timing_capture(char **task_names, int num_tasks ,int buffer_capacity);
void add_time_to_buffer(int task_id, __uint32_t loop_time);

void setup_timing_capture(int buffer_capacity);
void add_time_to_buffer(struct timespec *result);

#endif // _UTILITY_TIMESPEC

