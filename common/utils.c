#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "utils.h"

void print_bar_space_widths(int64_t *widths, int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("%lld ", widths[i]);
    }
    printf("\n");
}

// Function to calculate mean
double calculate_mean(int64_t arr[], int size)
{
    double sum = 0.0;
    int empty_bytes_count = 0;
    for (int i = 0; i < size; i++)
    {
        sum += arr[i];
        if (arr[i] <= 0)
            empty_bytes_count += 1;
    }

    return sum / (size - empty_bytes_count);
}

// Function to calculate standard deviation
double calculate_std_dev(int64_t arr[], int size, double mean)
{
    double sum = 0.0;
    int empty_bytes_count = 0;
    for (int i = 0; i < size; i++)
    {
        if (arr[i] > 0)
            sum += pow(arr[i] - mean, 2);
        else
            empty_bytes_count += 1;
    }
    return sqrt(sum / (size - empty_bytes_count));
}

// Function to calculate median
double calculate_median(int64_t arr[], int size)
{
    int temp;
    int *sorted = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
    {
        sorted[i] = arr[i];
    }
    // Sort the array to find the median
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            if (sorted[i] > sorted[j])
            {
                temp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = temp;
            }
        }
    }
    double median;
    if (size % 2 == 0)
    {
        median = (sorted[size / 2 - 1] + sorted[size / 2]) / 2.0;
    }
    else
    {
        median = sorted[size / 2];
    }
    free(sorted);
    return median;
}

// Classification based on mean threshold
void classify_mean(int64_t arr[], int size, char result[])
{
    double mean = calculate_mean(arr, size);
    // printf("mean: %f", mean);
    for (int i = 0; i < size; i++)
    {
        result[i] = arr[i] > mean ? '1' : '0';
    }
}

// Classification based on median threshold
void classify_median(int64_t arr[], int size, char result[])
{
    double median = calculate_median(arr, size);
    for (int i = 0; i < size; i++)
    {
        result[i] = arr[i] > median ? '1' : '0';
    }
}

// Classification based on mean + std deviation threshold
void classify_mean_std(int64_t arr[], int size, char result[], double factor)
{
    double mean = calculate_mean(arr, size);
    double std_dev = calculate_std_dev(arr, size, mean);
    double threshold = mean + factor * std_dev;
    // printf("Mean: %f, Std Dev: %f, Threshold: %f\n", mean, std_dev, threshold);
    for (int i = 0; i < size; i++)
    {
        result[i] = arr[i] > threshold ? '1' : '0';
    }
}