#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "genetic_algorithm.h"
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

typedef struct thread_argument {
	int id;
	int object_count;
	int sack_capacity;
	int generations_count;
	int P;
	sack_object *objects;
	individual *current_generation;
	individual *next_generation;
	pthread_barrier_t* barrier;
} thread_argument;

void *thread_function(void *arg) {
	thread_argument* data = (thread_argument*) arg;

	int count = 0;
	int cursor = 0;
	individual *tmp = NULL;

	int thread_id = data->id;
	int object_count = data->object_count;
	double double_object_count = (double)object_count;
	int P = data->P;

	int start = thread_id * double_object_count / P;
	int startEven = 0;
	int end = min((thread_id + 1) * double_object_count / P, object_count);

	// variables needed for sorting
	int numbers_per_thread = object_count / P;
	int offset = object_count % P;
	int left = thread_id * (numbers_per_thread);
	int right = (thread_id + 1) * (numbers_per_thread) - 1;
	if (thread_id == P - 1) {
			right += offset;
	}
	int middle = left + (right - left) / 2;

	sack_object *objects = data->objects;
	int sack_capacity = data->sack_capacity;
	
	for (int i = start; i < end; i++) {
		data->current_generation[i].fitness = 0;
		data->current_generation[i].num_items = 0;
		data->current_generation[i].chromosomes = (int*) calloc(object_count, sizeof(int));
		data->current_generation[i].chromosomes[i] = 1;
		data->current_generation[i].index = i;
		data->current_generation[i].chromosome_length = object_count;

		data->next_generation[i].fitness = 0;
		data->next_generation[i].num_items = 0;
		data->next_generation[i].chromosomes = (int*) calloc(object_count, sizeof(int));
		data->next_generation[i].index = i;
		data->next_generation[i].chromosome_length = object_count;
	}

	// iterate for each generation
	for (int k = 0; k < data->generations_count; k++) {

		pthread_barrier_wait(data->barrier);

		start = thread_id * double_object_count / P;
	  end = min((thread_id + 1) * double_object_count / P, object_count);

		// compute fitness and sort by it
		compute_fitness_function(objects, data->current_generation, sack_capacity, start, end);

		pthread_barrier_wait(data->barrier);

		merge_sort(data->current_generation, left, right);
		merge_sort(data->current_generation, left + 1, right);
		merge(data->current_generation, left, middle, right);

		pthread_barrier_wait(data->barrier);

		if (thread_id == 0) {
			merge_sections_of_array(data->current_generation, P, 1, numbers_per_thread, object_count);
		}

		pthread_barrier_wait(data->barrier);

		// keep first 30% children (elite children selection)
		count = object_count * 3 / 10;
		start = thread_id * (double)count / P;
		end = min((thread_id + 1) * (double)count / P, count);

		for (int i = start; i < end; ++i) {
			copy_individual(data->current_generation + i, data->next_generation + i);
		}
		cursor = count;

		// mutate first 20% children with the first version of bit string mutation
		count = object_count * 2 / 10;
		start = thread_id * (double)count / P;
		end = min((thread_id + 1) * (double)count / P, count);

		for (int i = start; i < end; ++i) {
			copy_individual(data->current_generation + i, data->next_generation + cursor + i);
			mutate_bit_string_1(data->next_generation + cursor + i, k);
		}
		cursor += count;

		// mutate next 20% children with the second version of bit string mutation
		count = object_count * 2 / 10;
		start = thread_id * (double)count / P;
		end = min((thread_id + 1) * (double)count / P, count);

		for (int i = start; i < end; ++i) {
			copy_individual(data->current_generation + i + count, data->next_generation + cursor + i);
			mutate_bit_string_2(data->next_generation + cursor + i, k);
		}
		cursor += count;

		// crossover first 30% parents with one-point crossover
		// (if there is an odd number of parents, the last one is kept as such)
		count = object_count * 3 / 10;

		if (count % 2 == 1) {
			copy_individual(data->current_generation + object_count - 1, data->next_generation + cursor + count - 1);
			count--;
		}

		start = thread_id * (double)count / P;
		end = min((thread_id + 1) * (double)count / P, count);
		startEven = (start % 2 == 0) ? start : start + 1;

		for (int i = startEven; i < end; i += 2) {
			crossover(data->current_generation + i, data->next_generation + cursor + i, k);
		}

		// switch to new generation
		tmp = data->current_generation;
		data->current_generation = data->next_generation;
		data->next_generation = tmp;

		start = thread_id * double_object_count / P;
		end = min((thread_id + 1) * double_object_count / P, object_count);

		for (int i = start; i < end; ++i) {
			data->current_generation[i].index = i;
		}

		if (thread_id == 0 && k % 5 == 0) {
			print_best_fitness(data->current_generation);
		}
	}

	start = thread_id * double_object_count / P;
	end = min((thread_id + 1) * double_object_count / P, object_count);
	compute_fitness_function(objects, data->current_generation, sack_capacity, start, end);

	pthread_barrier_wait(data->barrier);

	merge_sort(data->current_generation, left, right);
	merge_sort(data->current_generation, left + 1, right);
	merge(data->current_generation, left, middle, right);

	pthread_barrier_wait(data->barrier);

	if (thread_id == 0) {
		merge_sections_of_array(data->current_generation, P, 1, numbers_per_thread, object_count);
		print_best_fitness(data->current_generation);
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	// array with all the objects that can be placed in the sack
	sack_object *objects = NULL;

	// number of objects
	int object_count = 0;

	// maximum weight that can be carried in the sack
	int sack_capacity = 0;

	// number of generations
	int generations_count = 0;

	// number of threads
	int P = 0;

	if (!read_input(&objects, &object_count, &sack_capacity, &generations_count, &P, argc, argv)) {
		return 0;
	}

	pthread_t *threads = (pthread_t*) malloc(P * sizeof(pthread_t));
	thread_argument* arguments = (thread_argument*) malloc(P * sizeof(thread_argument));

	individual *current_generation = (individual*) calloc(object_count, sizeof(individual));
	individual *next_generation = (individual*) calloc(object_count, sizeof(individual));

	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, P);

	int r = 0;
	void *status = NULL;

	for (int i = 0; i < P; i++) {
		arguments[i].id = i;
		arguments[i].object_count = object_count;
		arguments[i].sack_capacity = sack_capacity;
		arguments[i].generations_count = generations_count;
		arguments[i].P = P;
		arguments[i].objects = objects;
		arguments[i].current_generation = current_generation;
		arguments[i].next_generation = next_generation;
		arguments[i].barrier = &barrier;

		r = pthread_create(&threads[i], NULL, thread_function, &arguments[i]);

		if (r) {
			printf("Error creating thread %d\n", i);
			exit(-1);
		}
	}

	for (int i = 0; i < P; i++) {
		r = pthread_join(threads[i], &status);

		if (r) {
			printf("Error waiting thread %d\n", i);
			exit(-1);
		}
	}

	pthread_barrier_destroy(&barrier);
	free(threads);
	free(arguments);
	free(objects);

	free_generation(current_generation);
	free_generation(next_generation);

	free(current_generation);
	free(next_generation);

	return 0;
}
