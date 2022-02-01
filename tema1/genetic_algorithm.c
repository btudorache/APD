#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "genetic_algorithm.h"

int read_input(sack_object **objects, int *object_count, int *sack_capacity, int *generations_count, int *P, int argc, char *argv[])
{
	FILE *fp;

	if (argc < 4) {
		fprintf(stderr, "Usage:\n\t./tema1 in_file generations_count num_threads\n");
		return 0;
	}

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		return 0;
	}

	if (fscanf(fp, "%d %d", object_count, sack_capacity) < 2) {
		fclose(fp);
		return 0;
	}

	if (*object_count % 10) {
		fclose(fp);
		return 0;
	}

	sack_object *tmp_objects = (sack_object *) calloc(*object_count, sizeof(sack_object));

	for (int i = 0; i < *object_count; ++i) {
		if (fscanf(fp, "%d %d", &tmp_objects[i].profit, &tmp_objects[i].weight) < 2) {
			free(objects);
			fclose(fp);
			return 0;
		}
	}

	fclose(fp);

	*generations_count = (int) strtol(argv[2], NULL, 10);
	
	if (*generations_count == 0) {
		free(tmp_objects);

		return 0;
	}

	*P = (int) strtol(argv[3], NULL, 10);
	
	if (*P == 0) {
		free(tmp_objects);

		return 0;
	}

	*objects = tmp_objects;

	return 1;
}

void print_objects(const sack_object *objects, int object_count)
{
	for (int i = 0; i < object_count; ++i) {
		printf("%d %d\n", objects[i].weight, objects[i].profit);
	}
}

void print_generation(const individual *generation, int limit)
{
	for (int i = 0; i < limit; ++i) {
		for (int j = 0; j < generation[i].chromosome_length; ++j) {
			printf("%d ", generation[i].chromosomes[j]);
		}

		printf("\n%d - %d\n", i, generation[i].fitness);
	}
}

void print_best_fitness(const individual *generation)
{
	printf("%d\n", generation[0].fitness);
}

void compute_fitness_function(const sack_object *objects, individual *generation, int sack_capacity, int start, int end)
{
	int weight;
	int profit;
	int num_items;

	for (int i = start; i < end; ++i) {
		weight = 0;
		profit = 0;
		num_items = 0;

		for (int j = 0; j < generation[i].chromosome_length; ++j) {
			if (generation[i].chromosomes[j]) {
				num_items += 1;
				weight += objects[j].weight;
				profit += objects[j].profit;
			}
		}

		generation[i].num_items = num_items;
		generation[i].fitness = (weight <= sack_capacity) ? profit : 0;
	}
}

int cmpfunc(const void *a, const void *b)
{
	individual *first = (individual *) a;
	individual *second = (individual *) b;

	int res = second->fitness - first->fitness; // decreasing by fitness
	if (res == 0) {
		res = first->num_items - second->num_items; // increasing by number of objects in the sack
		if (res == 0) {
			return second->index - first->index;
		}
	}

	return res;
}

void mutate_bit_string_1(const individual *ind, int generation_index)
{
	int i, mutation_size;
	int step = 1 + generation_index % (ind->chromosome_length - 2);

	if (ind->index % 2 == 0) {
		// for even-indexed individuals, mutate the first 40% chromosomes by a given step
		mutation_size = ind->chromosome_length * 4 / 10;
		for (i = 0; i < mutation_size; i += step) {
			ind->chromosomes[i] = 1 - ind->chromosomes[i];
		}
	} else {
		// for even-indexed individuals, mutate the last 80% chromosomes by a given step
		mutation_size = ind->chromosome_length * 8 / 10;
		for (i = ind->chromosome_length - mutation_size; i < ind->chromosome_length; i += step) {
			ind->chromosomes[i] = 1 - ind->chromosomes[i];
		}
	}
}

void mutate_bit_string_2(const individual *ind, int generation_index)
{
	int step = 1 + generation_index % (ind->chromosome_length - 2);

	// mutate all chromosomes by a given step
	for (int i = 0; i < ind->chromosome_length; i += step) {
		ind->chromosomes[i] = 1 - ind->chromosomes[i];
	}
}

void crossover(individual *parent1, individual *child1, int generation_index)
{
	individual *parent2 = parent1 + 1;
	individual *child2 = child1 + 1;
	int count = 1 + generation_index % parent1->chromosome_length;

	memcpy(child1->chromosomes, parent1->chromosomes, count * sizeof(int));
	memcpy(child1->chromosomes + count, parent2->chromosomes + count, (parent1->chromosome_length - count) * sizeof(int));

	memcpy(child2->chromosomes, parent2->chromosomes, count * sizeof(int));
	memcpy(child2->chromosomes + count, parent1->chromosomes + count, (parent1->chromosome_length - count) * sizeof(int));
}

void copy_individual(const individual *from, const individual *to)
{
	memcpy(to->chromosomes, from->chromosomes, from->chromosome_length * sizeof(int));
}

void merge(individual *current_generation, int left, int middle, int right) {
    int i = 0;
    int j = 0;
    int k = 0;
    int left_length = middle - left + 1;
    int right_length = right - middle;
    individual left_array[left_length];
    individual right_array[right_length];
    
    /* copy values to left array */
    for (int i = 0; i < left_length; i ++) {
        left_array[i] = current_generation[left + i];
    }
    
    /* copy values to right array */
    for (int j = 0; j < right_length; j ++) {
        right_array[j] = current_generation[middle + 1 + j];
    }
    
    i = 0;
    j = 0;
    /** chose from right and left arrays and copy */
    while (i < left_length && j < right_length) {
        if (cmpfunc(left_array + i, right_array + j) <= 0) {
            current_generation[left + k] = left_array[i];
            i++;
        } else {
            current_generation[left + k] = right_array[j];
            j++;
        }
        k++;
    }
    
    /* copy the remaining values to the array */
    while (i < left_length) {
        current_generation[left + k] = left_array[i];
        k++;
        i++;
    }

    while (j < right_length) {
        current_generation[left + k] = right_array[j];
        k++;
        j++;
    }
}

void merge_sections_of_array(individual *current_generation, int number, int aggregation, int numbers_per_thread, int length) {
    for(int i = 0; i < number; i = i + 2) {
        int left = i * (numbers_per_thread * aggregation);
        int right = ((i + 2) * numbers_per_thread * aggregation) - 1;
        int middle = left + (numbers_per_thread * aggregation) - 1;
        if (right >= length) {
            right = length - 1;
        }
        merge(current_generation, left, middle, right);
    }
    if (number / 2 >= 1) {
        merge_sections_of_array(current_generation, number / 2, aggregation * 2, numbers_per_thread, length);
    }
}

void merge_sort(individual *current_generation, int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;
        merge_sort(current_generation, left, middle);
        merge_sort(current_generation, middle + 1, right);
        merge(current_generation, left, middle, right);
    }
}

void free_generation(individual *generation)
{
	int i;

	for (i = 0; i < generation->chromosome_length; ++i) {
		free(generation[i].chromosomes);
		generation[i].chromosomes = NULL;
		generation[i].fitness = 0;
	}
}