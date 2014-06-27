#include <iostream>
#include <stdio.h>
#include <cstring>
#include <thread>
#include <random>
#include <chrono>

void s_lock(int * lock)
{
  while (!*lock)
  {
    // Attempt to get out of deadlock by giving other threads breathing space
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
  }
  (*lock)--;
}

void s_unlock(int * lock)
{
  (*lock)++;
}

int get_num_cores ()
{
    FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
    int cores = 0;
    char *arg = 0;
    const char *cpu_cores_str = "cpu cores";
    char *cpu_cores = 0;
    size_t size = 0;
    while(getdelim(&arg, &size, (int)'\0', cpuinfo) != -1)
    {
      cpu_cores = strstr (arg, cpu_cores_str);
      cpu_cores = strchr (cpu_cores, (int)':');
      if (cpu_cores)
        cores = (int)(cpu_cores[2] - '0');
    }
    free(arg);
    fclose(cpuinfo);

    return cores;
}

int monte_carlo(unsigned int n, double r, double *answer, int *lock)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    const double lower_bound = 0;
    const double upper_bound = r;
    std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
    std::default_random_engine re;
    re.seed(seed);

    float ret = 0;
    float x(0.0), y(0.0);
    unsigned int i(0), M(0);

    if (n == 0)
    {
      return 0;
    }

    for (i = 0; i < n; ++i)
    {
      x = unif(re);
      y = unif(re);
      if (x * x + y * y <= r * r)
      {
        ++M;
      }
    }

    ret = (double)((4.0 * (double)M) / (double)n);

    s_lock (lock);
    *answer += ret;
    s_unlock (lock);
    printf ("thread exiting with ret value: %f\n", ret);

    return 0;
}

int main()
{
  std::vector<std::thread> threads;
  double answer = 0.0;
  int lock = 1;
  unsigned int number_of_rands = 0;
  const float r = 10.0;
  int num_threads = 0;

  std::cout << "system CPU cores: " << get_num_cores() << std::endl;

  std::cout << "Enter Number of threads: " << std::endl;
  std::cin >> num_threads;

  std::cout << "Enter Number of randoms: " << std::endl;
  std::cin >> number_of_rands;

  std::cout << "Starting threads... " << std::endl;
  for (int i = 0; i < num_threads; ++i)
    threads.emplace_back(monte_carlo, number_of_rands, r, &answer, &lock);

  for (std::vector<std::thread>::iterator i = threads.begin(); i != threads.end(); ++i)
    i->join();

  answer /= num_threads;
  std::cout << "Approximation of pi: " << answer << std::endl;

  return 0;
}
