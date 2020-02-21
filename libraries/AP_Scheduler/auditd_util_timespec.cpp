#include "auditd_util_timespec.h"
#include <stdlib.h>
#include <signal.h>
#include <iostream>


Task_Timing* timing_results;
int NUM_TASKS;
int FREQUENCY;
uint32_t MAIN_LOOP_MICROS;

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
__uint32_t timespec_get_micro_u64(struct timespec *in) {
	return (__uint32_t)(in->tv_sec*1000*1000) + (__uint32_t)(in->tv_nsec/1000);
}

void dump_timing_results(int s){
    //Should open a file here - so that we have a way to know when the dump operations begin
    FILE *fp;
    fp = fopen ("timing.log", "w+");
    int print_output = 0;
    const char *op_toggle = getenv("ARDU_TIME");
    if(!op_toggle){
	fprintf(stderr,"ARDU_TIME not set\n");
    } else{
	    print_output = atoi(op_toggle);
    }
        
    for(int i = 0; i < NUM_TASKS;i++){
        char* task_name = timing_results[i].name;
	if(print_output == 1){
        	for(int j =0;j < timing_results[i].current_buffer; j++){
            		std::cout << task_name << " " << timing_results[i].timings[j] << " us" << std::endl;
        	}
	} else if(print_output == 2){
        if(i == 0){
            int deadlines_missed_count = 0;
            std::cout << "Missed deadlines" << std::endl;
            for(int j = 0;j < timing_results[i].current_buffer;j++){
                if(timing_results[i].timings[j] > MAIN_LOOP_MICROS){
                    std::cout << task_name << " " << timing_results[i].timings[j] << " us" << std::endl;
                }
            }
        }
    }
        free(timing_results[i].timings);        
    }
    
    free(timing_results);
    fclose(fp);
    exit(1);
}

void setup_sighandler(){
   struct sigaction sigIntHandler;

   sigIntHandler.sa_handler = dump_timing_results;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;

   sigaction(SIGINT, &sigIntHandler, NULL);
}

void setup_timing_capture(char **task_names, int num_tasks, int buffer_capacity, int main_loop_freq){
    timing_results = (Task_Timing*) malloc(sizeof(Task_Timing) * num_tasks);
    NUM_TASKS = num_tasks;
    FREQUENCY = main_loop_freq;
    MAIN_LOOP_MICROS = 1000000 / FREQUENCY;

    for(int i = 0; i < num_tasks;i++){
        timing_results[i].buffer_capacity = buffer_capacity;
        timing_results[i].name = task_names[i];
        timing_results[i].current_buffer = 0;
        timing_results[i].timings = (u64*) malloc(sizeof(u64) * buffer_capacity);
    }

    setup_sighandler();
}

bool check_all_tasks_collected(){
    bool check = false;
    for(int i = 0;i < NUM_TASKS;i++){
        if (timing_results[i].current_buffer >= timing_results[i].buffer_capacity){
            check = true;
	    break;
        }
    }

    return check;
}

void add_time_to_buffer(int task_id, uint32_t loop_time){

    int current_size = timing_results[task_id].current_buffer;
    if(current_size < timing_results[task_id].buffer_capacity){
        timing_results[task_id].timings[current_size] = loop_time;
        timing_results[task_id].current_buffer++;
    }

    if(check_all_tasks_collected()){
        dump_timing_results(0);
    }
}




