#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

int P;
int n;
double *arr_real;
double *arr_im;
double *arr_input;

void* threadFunction(void *var) {
	int thread_id = *(int*)var;
	for (int i = thread_id; i < n; i += P) {
		double re = 0;
		double im = 0;
		for (int j = 0; j < n; ++j) {
			re += arr_input[j] * cos(i * j * 2 * M_PI / n);
			im -= arr_input[j] * sin(i * j * 2 * M_PI / n);
		}
			arr_real[i] = re;
		arr_im[i] = im;
	}
}


int main(int argc, char * argv[]) {
	FILE *fd_input = fopen(argv[1], "r");
	FILE *fd_output = fopen(argv[2], "w");
	P = atoi(argv[3]);
	
	fscanf(fd_input, "%d", &n);
	
	arr_input = (double *) calloc(n, sizeof(double));
	for (int i = 0; i < n; ++i) {
		fscanf(fd_input, "%lf", &arr_input[i]);
	}

	arr_im = (double *) calloc(n, sizeof(double));
	arr_real = (double *) calloc(n, sizeof(double));

	pthread_t tid[P];
	int thread_id[P];
	for(int i = 0;i < P; i++)
		thread_id[i] = i;

	for(int i = 0; i < P; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}

	for(int i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	fprintf(fd_output, "%d\n", n);
	for (int i = 0; i < n; ++i) {
		fprintf(fd_output, "%lf %lf\n", arr_real[i], arr_im[i]);
	}

	free(arr_input);
	free(arr_im);
	free(arr_real);

	fclose(fd_input);
	fclose(fd_output);
	return 0;
}
