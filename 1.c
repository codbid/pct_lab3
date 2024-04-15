#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

const double eps = 1E-5;
const int n0 = 500000000;

double S(double time_nomp,double time_omp)
{
    return time_nomp/time_omp;
}

double wtime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1E-9;
}

double func(double x)
{
    return log(1 + x) / x;
}

double run_serial() 
{
    double t = wtime();
    const double a = 0.1;
    const double b = 1.0;
    int n = n0, k;
    double sq[2], delta = 1;
    for (k = 0; delta > eps; n *= 2, k ^= 1) {
        double h = (b - a) / n;
        double s = 0.0;
        for (int i = 0; i < n; i++)
            s += func(a + h * (i + 0.5));
        sq[k] = s * h;
        if (n > n0)
            delta = fabs(sq[k] - sq[k ^ 1]) / 3.0;
    }
    printf("Result: %.12f; Runge rule: EPS %e, n %d\n", sq[k] * sq[k], eps, n / 2);
    t = wtime() - t;
    return t;
}

double run_parallel(int f) 
{
    double t = wtime();
    const double a = 0.1;
    const double b = 1.0;
    double sq[2];
    #pragma omp parallel num_threads(f)
    {
        int n = n0, k;
        double delta = 1;
        for (k = 0; delta > eps; n *= 2, k ^= 1) {
            double h = (b - a) / n;
            double s = 0.0;
            sq[k] = 0;
            #pragma omp barrier
            #pragma omp for nowait
            for (int i = 0; i < n; i++)
                s += func(a + h * (i + 0.5));
            #pragma omp atomic
            sq[k] += s * h;
            #pragma omp barrier
            if (n > n0)
                delta = fabs(sq[k] - sq[k ^ 1]) / 3.0;
        }
        #pragma omp master
        printf("Result: %.12f; Runge rule: EPS %e, n %d\n", sq[k] * sq[k], eps, n / 2);
    }
    t = wtime() - t;
    return t;
}

void write(double S, int n) 
{ 
  FILE *f; 
  f = fopen("1.dat", "a"); 
  fprintf(f, "%d %f\n", n, S); 
  fclose(f); 
} 

int main(int argc, char **argv)
{
    double time_nomp = run_serial();
    double time_omp;
    printf("Time serial: %.6f\n\n", time_nomp);
    for(int i = 2; i <= 8; i += 2) {
        time_omp = run_parallel(i);
        printf("Time parallel: %.6f\n", time_omp);
        printf("S: %.6f\n\n", S(time_nomp,time_omp));
        write(S(time_nomp,time_omp), i);
    }
    return 0;
}