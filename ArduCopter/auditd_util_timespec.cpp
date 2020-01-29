#include "auditd_util_timespec.h"


// Source code is from https://gist.github.com/diabloneo/9619917
void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }

    return;
}

void timespec_deep_copy(struct timespec *src, struct timespec *dest) {
	dest->tv_sec = src->tv_sec;
	dest->tv_nsec = src->tv_nsec;
}

void timespec_get_time(struct timespec *instance) {
	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, instance);
	//clock_gettime(CLOCK_MONOTONIC_RAW, instance);
	clock_gettime(CLOCK_MONOTONIC, instance);
}

//TODO: Obviously there is limit for this conversion when the time is super large.
u64 timespec_get_micro_u64(struct timespec *in) {
	return (u64)(in->tv_sec*1000*1000) + (u64)(in->tv_nsec/1000);
}

