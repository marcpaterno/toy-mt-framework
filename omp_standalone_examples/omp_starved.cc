//g++ -std=c++17 -O3 omp_starved.cc -fopenmp
#include "omp.h"
#include <cmath>
#include <atomic>
#include <iostream>
#include <sys/time.h>

namespace demo {
  template<typename T>
    void parallel_for(unsigned int iRanges, T&& iFunctor) {
    //Using the following causes the program to starve with no progress
//#pragma omp for
//Works, but is inefficient
#pragma omp parallel for
//Using the following works efficiently
//#pragma omp taskloop
    for(unsigned int i=0; i<iRanges; ++i) {
      iFunctor(i);
    }
  }
}

int main() {

  omp_set_num_threads(4);

  std::atomic<int> count{0};

  struct timeval startRealTime;
  gettimeofday(&startRealTime, 0);

#pragma omp parallel default(shared)
  {
#pragma omp single
    {
      for(int i = 1; i< 5; ++i) {
#pragma omp task untied default(shared)
        {
          demo::parallel_for(100, [&count](auto j) {
              double v = 0;
              constexpr int nIterations = 1000000;
              for(int k=0; k<nIterations; ++k) {
                v += std::sin(3.14159/2.*k/nIterations)/double(nIterations);
              }
              if(v>0.5) {
                ++count;
              }
            });
        }
      }
    }
  }

  struct timeval tp;
  gettimeofday(&tp, 0);
  
  constexpr double microsecToSec = 1E-6;

  double realTime = tp.tv_sec - startRealTime.tv_sec + microsecToSec * (tp.tv_usec - startRealTime.tv_usec);

  std::cout <<count.load()<<" "<<realTime<<std::endl;
  return 0;
}