#include "simlib.c"

#define EVENT_LOADING_1		1
#define EVENT_LOADING_2		2
#define EVENT_LOADING_3		3
#define EVENT_UNLOADING_1	4
#define EVENT_UNLOADING_2	5
#define EVENT_UNLOADING_3	6
#define EVENT_ENDOFTIME     7 /* Event type for end time. */
#define EVENT_INTERARRIVAL_1	8
#define EVENT_INTERARRIVAL_2	9
#define EVENT_INTERARRIVAL_3	10

#define LIST_QUEUE_1        1
#define LIST_QUEUE_2        2
#define LIST_QUEUE_3        3
#define LIST_BUS_1			4
#define LIST_BUS_2			5
#define LIST_BUS_3			6
#define LIST_TEMP_1			7
#define LIST_TEMP_2			8
#define LIST_TEMP_3			9

#define STREAM_INTERARRIVAL_1	1
#define STREAM_INTERARRIVAL_2	2
#define STREAM_INTERARRIVAL_3	3
#define STREAM_UNLOADING 		4
#define STREAM_LOADING 			5
#define STREAM_DESTINATION		6

#define SAMPST_QUEUE_1_TIME      1
#define SAMPST_QUEUE_2_TIME      2
#define SAMPST_QUEUE_3_TIME      3
#define SAMPST_LOOP_TIME      	 4
#define SAMPST_NUMBER_BUS        5
#define SAMPST_WAITING_TIME_1	 6
#define SAMPST_WAITING_TIME_2	 7
#define SAMPST_WAITING_TIME_3	 8

double mean_interarrival1, mean_interarrival2, mean_interarrival3, min_loading, max_loading, min_unloading, max_unloading, time_end, time_loop;

FILE  *infile, *outfile;

void init_model() {
	double prob_distrib[2];
	prob_distrib[1] = 0.583;
	prob_distrib[2] = 1;

	time_loop = 0;

	event_schedule(time_end, EVENT_ENDOFTIME);
	float time = 0;
	while (time < 4800) {
		event_schedule(time, EVENT_INTERARRIVAL_1);
		list_file(LAST, LIST_QUEUE_1);
		time += expon(mean_interarrival1, STREAM_INTERARRIVAL_1);
	}
	time = 0;
	while (time < 4800) {
		event_schedule(time, EVENT_INTERARRIVAL_2);
		list_file(LAST, LIST_QUEUE_2);
		time += expon(mean_interarrival2, STREAM_INTERARRIVAL_2);
	}
	time = 0;
	while (time < 4800) {
		event_schedule(time, EVENT_INTERARRIVAL_3);
		list_file(LAST, LIST_QUEUE_3);
		time += expon(mean_interarrival3, STREAM_INTERARRIVAL_3);
	}
	
	float j = 0;
	while (j < 5 && (list_size[LIST_BUS_1] + list_size[LIST_BUS_2] + list_size[LIST_BUS_3]) < 20) {
		list_remove(FIRST, LIST_QUEUE_1);
		float temp;
		if (j < 5) {
			int random = random_integer(prob_distrib, STREAM_DESTINATION);
			if (random == 1) {
				list_file (LAST, LIST_BUS_1);
				temp = transfer[1];
			} else if (random == 2) {
				list_file (LAST, LIST_BUS_2);
				temp = transfer[1];
			}
			j += uniform(min_loading, max_loading, STREAM_LOADING);
			if (sim_time + j - temp < 0) {
				sampst(0, SAMPST_WAITING_TIME_1);
			} else {
				sampst(sim_time - temp, SAMPST_WAITING_TIME_1);
			}
		} else if (j >= 5) {
			list_file(FIRST, LIST_QUEUE_1);
		}
	}

	sampst(list_size[LIST_BUS_1] + list_size[LIST_BUS_2] + list_size[LIST_BUS_3], SAMPST_NUMBER_BUS);
	event_schedule(sim_time + j, EVENT_LOADING_1);
	
	j = 0;
	for (int i = 0; i < list_size[LIST_BUS_1]; i++) {
		j += list_size[LIST_BUS_1] * uniform(min_unloading, max_unloading, STREAM_UNLOADING);
	}
	event_schedule(sim_time + j, EVENT_UNLOADING_1);
	while (list_size[LIST_BUS_1] > 0) {
		list_remove(FIRST, LIST_BUS_1);
	}
}

void service_1() {
	double prob_distrib[2];
	prob_distrib[1] = 0.583;
	prob_distrib[2] = 1;

	time_loop = sim_time;

	float j = 0;
	while (j < 5 && (list_size[LIST_BUS_1] + list_size[LIST_BUS_2] + list_size[LIST_BUS_3]) < 20) {
		float temp;
		// if (list_size[LIST_QUEUE_1] > 0) {
		// 	list_remove(FIRST, LIST_QUEUE_1);
		// }
		
		if (j < 5) {
			int random = random_integer(prob_distrib, STREAM_DESTINATION);
			if (random == 1) {
				list_file (LAST, LIST_BUS_1);
				temp = transfer[1];
			} else if (random == 2) {
				list_file (LAST, LIST_BUS_2);
				temp = transfer[1];
			}
			j += uniform(min_loading, max_loading, STREAM_LOADING);
			if (sim_time - temp > 0) {
				sampst(sim_time + j - temp, SAMPST_WAITING_TIME_1);
			} else {
				sampst(0, SAMPST_WAITING_TIME_1);
			}
		} else if (j >= 5) {
			list_file(FIRST, LIST_QUEUE_1);
		}
	}

	sampst(list_size[LIST_BUS_1] + list_size[LIST_BUS_2] + list_size[LIST_BUS_3], SAMPST_NUMBER_BUS);
	event_schedule(sim_time + j, EVENT_LOADING_1);

	float time = 0;
	for (int i = 0; i < list_size[LIST_BUS_1]; i++) {
		time += uniform(min_unloading, max_unloading, STREAM_UNLOADING);
	}
	sampst(time, SAMPST_QUEUE_1_TIME);
	time += 9;
	event_schedule(sim_time + time, EVENT_UNLOADING_1);

	while (list_size[LIST_BUS_1] > 0) {
		list_remove(FIRST, LIST_BUS_1);
	}
	// printf("%f\n", sim_time);
}

void service_2() {
	float j = 0;
	while (j < 5 && (list_size[LIST_BUS_1] + list_size[LIST_BUS_2] + list_size[LIST_BUS_3]) < 20) {
		float temp;
		// if (list_size[LIST_QUEUE_2] > 0) {
		// 	list_remove(FIRST, LIST_QUEUE_2);
		// }
		temp = transfer[1];
		if (j < 5) {
			list_file (LAST, LIST_BUS_3);
			j += uniform(min_loading, max_loading, STREAM_LOADING);
			if (sim_time - temp > 0) {
				sampst(sim_time + j - temp, SAMPST_WAITING_TIME_2);
			} else {
				sampst(0, SAMPST_WAITING_TIME_2);
			}
		} else if (j >= 5) {
			list_file(FIRST, LIST_QUEUE_2);
		}
	}

	sampst(list_size[LIST_BUS_1] + list_size[LIST_BUS_2] + list_size[LIST_BUS_3], SAMPST_NUMBER_BUS);
	event_schedule(sim_time + j, EVENT_LOADING_2);
	
	float time = 0;
	for (int i = 0; i < list_size[LIST_BUS_2]; i++) {
		time += uniform(min_unloading, max_unloading, STREAM_UNLOADING);
	}
	sampst(time, SAMPST_QUEUE_2_TIME);
	time += 2;
	event_schedule(sim_time + time, EVENT_UNLOADING_2);

	while (list_size[LIST_BUS_2] > 0) {
		list_remove(FIRST, LIST_BUS_2);
	}
}

void service_3() {
	float j = 0;
	while (j < 5 && (list_size[LIST_BUS_1] + list_size[LIST_BUS_2] + list_size[LIST_BUS_3]) < 20) {
		float temp;
		// if (list_size[LIST_QUEUE_3] > 0) {
		// 	list_remove(FIRST, LIST_QUEUE_3);
		// }
		temp = transfer[1];
		if (j < 5) {
			list_file (LAST, LIST_BUS_3);
			j += uniform(min_loading, max_loading, STREAM_LOADING);
			if (sim_time - temp > 0) {
				sampst(sim_time - temp, SAMPST_WAITING_TIME_3);
			} else {
				sampst(0, SAMPST_WAITING_TIME_3);
			}
		} else if (j >= 5) {
			list_file(FIRST, LIST_QUEUE_3);
		}
	}

	sampst(list_size[LIST_BUS_1] + list_size[LIST_BUS_2] + list_size[LIST_BUS_3], SAMPST_NUMBER_BUS);
	event_schedule(sim_time + j, EVENT_LOADING_3);

	float time = 0;
	for (int i = 0; i < list_size[LIST_BUS_3]; i++) {
		time += uniform(min_unloading, max_unloading, STREAM_UNLOADING);
	}
	sampst(time, SAMPST_QUEUE_3_TIME);
	time += 9;
	sampst(sim_time - time_loop, SAMPST_LOOP_TIME);

	event_schedule(sim_time + time, EVENT_UNLOADING_3);
	while (list_size[LIST_BUS_3] > 0) {	
		list_remove(FIRST, LIST_BUS_3);
	}
	// printf("%f\n", sim_time);
}

void report() {
	fprintf (outfile, "\nTime simulation ended:%12.3f minutes\n", sim_time);

	//fprintf (outfile, "\na. Average and maximum number in each queue\n");
	//out_filest (outfile, LIST_TEMP_1, LIST_TEMP_3);

	fprintf (outfile, "\nc. Average and maximum number on the bus\n");
	out_sampst (outfile, SAMPST_NUMBER_BUS, SAMPST_NUMBER_BUS);

	fprintf (outfile, "\nd. Average, maximum, and minimum time the bus stopped in each location\n");
	out_sampst (outfile, SAMPST_QUEUE_1_TIME, SAMPST_QUEUE_3_TIME);
	
	fprintf (outfile, "\ne. Average, maximum, and minimum time for the bus to make a loop\n");
	out_sampst (outfile, SAMPST_LOOP_TIME, SAMPST_LOOP_TIME);

	fprintf (outfile, "\nf. Average, maximum, and minimum time a person is in the system by arrival location\n");
	out_sampst (outfile, SAMPST_WAITING_TIME_1, SAMPST_WAITING_TIME_3);
}

int main() {
	infile = fopen ("01-15.in", "r");
  	outfile = fopen ("01-15smlb.out", "w");

  	fscanf (infile, "%lg %lg %lg %lg %lg %lg %lg %lg", &mean_interarrival1, &mean_interarrival2, &mean_interarrival3,
  		&min_loading, &max_loading, &min_unloading, &max_unloading, &time_end);

  	maxatr = 4;

	init_simlib();
	init_model ();

	while (sim_time < 4800) {
        timing();

        switch (next_event_type) {
            case EVENT_UNLOADING_1:
                service_2();
                break;
            case EVENT_UNLOADING_2:
                service_3();
                break;
            case EVENT_UNLOADING_3:
                service_1();
                break;
        }
    }

    report();

    fclose (infile);
	fclose (outfile);

	return 0;
}