#include "auditd_util_timespec.h"
#include <stdlib.h>
#include <signal.h>
#include <iostream>


u64* timing_results;
int BUFFER_CAPACITY;
int CURR_BUFFER;

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

void setup_timing_capture(int buffer_capacity){
    BUFFER_CAPACITY = buffer_capacity;
    CURR_BUFFER = 0;
    timing_results = (u64*) malloc(BUFFER_CAPACITY * sizeof(u64));
    setup_sighandler();
}

void dump_timing_results(int s){
    std::cout << "Caught signal " << s << std::endl;
    
    for(int i = 0; i < CURR_BUFFER;i++){
        std::cout << timing_results[i] << "ns" << std::endl;
    }
    
    free(timing_results);
    exit(1);
}

void setup_sighandler(){
   struct sigaction sigIntHandler;

   sigIntHandler.sa_handler = dump_timing_results;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;

   sigaction(SIGINT, &sigIntHandler, NULL);
}

void add_time_to_buffer(struct timespec *result){
    if(CURR_BUFFER >= BUFFER_CAPACITY){
        dump_timing_results(0); //Manually shutting down the process and dump timing results if buffer is overflown
    } else{
        timing_results[CURR_BUFFER] = timespec_get_micro_u64(result);
        CURR_BUFFER++;
    }
}


