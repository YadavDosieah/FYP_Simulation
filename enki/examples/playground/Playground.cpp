#include "ShepherdingGUI.h"
#include "Shepherding.h"
#include "ShepherdingNoise.h"
#include "cmaes.h"
#include <libconfig.h++>
#include <mutex>
#include <fstream>
#include <algorithm>
#include <omp.h>
#include <iomanip>
#include "config.h"

// #define Analyis_Log 1
// #define Noise_Analysis 1

// #ifdef USE_SDL
// #include </usr/include/SDL2/SDL.h>
// #endif

using namespace libconfig;
using namespace libcmaes;

std::mutex mtx;
std::ofstream out;
pthread_mutex_t mtx2=PTHREAD_MUTEX_INITIALIZER;

int noOfObjects, NoOfGroups;
vector<int> noOfShepherd;
float GoalRadius, GoalDistance;
int Goalx, Goaly, Xbound, Ybound;
int global_iter;
int generation;
int lambda;
int No_Of_Trials;
int NoOfSteps;

//Function that runs simulation during evolutionary algorithm
FitFunc fitnessfunction = [](const double *x, const int N)
{
  global_iter++;
  if(global_iter>lambda)
  {
    global_iter = global_iter - lambda;
    generation++;
  }
  int iter = global_iter;

  double fitness = 0;

  //Run simulation multiple times for randomness
  #pragma omp parallel for reduction(+:fitness)
  for(int i=0;i<No_Of_Trials;i++)
  {
    start:
    World world(Xbound,Ybound, Color(0.5,0.5,0.5));
    Shepherding simulation(&world,noOfShepherd,noOfObjects,
      Goalx, Goaly,GoalRadius, GoalDistance, x, N);

      for (unsigned j=0; j < NoOfSteps; j++)
      {
        world.step(0.1);
        int ReturnVal = simulation.doPerTick();
        if(ReturnVal == 1)
        {
          cout << "Trial restarting\n";
          goto start;
        }
      }

      pthread_mutex_lock(&mtx2);
      out << generation << "," << iter << "," << i+1;
      for(int k=0; k<N/2; k++)
      {
        out << "," << x[k*2] << ","  << x[(k*2)+1];
      }
      out.precision(std::numeric_limits<long double>::digits10 + 1);
      out << "," << simulation.getTotalFitness() << endl;
      out.precision(-1);
      // cout << "Fitness= " << simulation.getTotalFitness() << endl;
      pthread_mutex_unlock(&mtx2);
      fitness = fitness + simulation.getTotalFitness();
  }
  // cout << "Average Fitness= " <<fitness/No_Of_Trials << endl;
  return fitness/No_Of_Trials;
};

FitFunc ControllerFitness = [](const double *x, const int N)
{
  std::lock_guard<std::mutex> lck (mtx);
  double fval = fitnessfunction(x,N);
  return fval;
};

int main(int argc, char *argv[])
{
  // cout << omp_get_max_threads() << endl;
  //Read configuration file
	Config configfile;
	configfile.readFile("Parameters.cfg");

  Setting& settings = configfile.lookup("noOfShepherd");
  int val;
  cout << "Number of Shepherds: ";
  for(int i=0; i < settings.getLength(); i++)
  {
    settings[i].lookupValue("Value",val);
    noOfShepherd.push_back(val);
    cout << val << ", ";
  }
  cout << endl;
  NoOfGroups = noOfShepherd.size();
  noOfObjects = configfile.lookup("noOfObjects");
	Goalx = configfile.lookup("Goalx");
	Goaly = configfile.lookup("Goaly");
  GoalRadius = configfile.lookup("GoalRadius");
  GoalDistance = configfile.lookup("GoalDistance");
  Xbound = configfile.lookup("Xbound");
  Ybound = configfile.lookup("Ybound");
  No_Of_Trials = configfile.lookup("No_Of_Trials");
  NoOfSteps = configfile.lookup("NoOfSteps");
  int NoOfEvolutions = configfile.lookup("NoOfEvolutions");
  int evolution = configfile.lookup("Evolution");
  int MaxIter = configfile.lookup("MaxIter");
  lambda = configfile.lookup("lambda"); // offsprings at each generation.
  int No_Of_Threads = configfile.lookup("No_Of_Threads");
  omp_set_num_threads(No_Of_Threads);
  omp_set_dynamic(false);

  #ifdef Noise_Analysis
    float Noise_Level = configfile.lookup("Noise_Level");
    int Noise_Scenario = configfile.lookup("Noise_Scenario");
  #endif

  #if(Analysis)
    #ifdef Analyis_Log
      std::ofstream Analysis_Logfile;
      Analysis_Logfile.open("Analysis.csv",std::ios_base::app);
    #endif

    #ifdef Noise_Analysis
      std::ofstream Noise_Logfile;
      Noise_Logfile.open("Noise.csv",std::ios_base::app);
    #endif

    double x[NoOfGroups*12];
    int dim = NoOfGroups*12;

    switch (NoOfGroups) {
      case 2:
        static double y[24] = {configfile.lookup("x0"),configfile.lookup("x1"),
                                          configfile.lookup("x2"),configfile.lookup("x3"),
                                          configfile.lookup("x4"),configfile.lookup("x5"),
                                          configfile.lookup("x6"),configfile.lookup("x7"),
                                          configfile.lookup("x8"),configfile.lookup("x9"),
                                          configfile.lookup("x10"),configfile.lookup("x11"),
                                          configfile.lookup("x12"),configfile.lookup("x13"),
                                          configfile.lookup("x14"),configfile.lookup("x15"),
                                          configfile.lookup("x16"),configfile.lookup("x17"),
                                          configfile.lookup("x18"),configfile.lookup("x19"),
                                          configfile.lookup("x20"),configfile.lookup("x21"),
                                          configfile.lookup("x22"),configfile.lookup("x23")};
        copy(y,y+24,x);
        break;
    }

    int No_Of_Success = 0;
    float SuccessRate = 0;
    float maxSR = 0;
    omp_set_num_threads(No_Of_Threads);
    omp_set_dynamic(false);
    #pragma omp parallel for reduction(+:No_Of_Success,SuccessRate,maxSR)
    for(int i=0;i<No_Of_Trials;i++)
    {
      maxSR = 0;
      //cout << "Trial " << i << endl;
      startAnalysis:
      World world(Xbound,Ybound, Color(0.5,0.5,0.5));
      #ifdef Noise_Analysis
        ShepherdingNoise simulation(&world,mode,noOfSheep,noOfShepherd,noOfObjects,Csheep,
          Cshepherd,Ksheep,K1, K2, KWall, Goalx, Goaly,GoalRadius, GoalDistance, x, dim,Noise_Level,Noise_Scenario);
      #else
        Shepherding simulation(&world,noOfShepherd,noOfObjects,
          Goalx, Goaly,GoalRadius, GoalDistance, x, dim);
      #endif

        #if !defined(Analyis_Log) && !defined(Noise_Analysis)
          #if(logData)
          {
            simulation.printHeaders();
          }
          #endif
        #endif

        for (unsigned j=0; j < NoOfSteps; j++)
        {
          world.step(0.1);
          int ReturnVal = simulation.doPerTick();
          if(ReturnVal == 1)
          {
            //cout << "Trial restarting\n";
            goto startAnalysis;
          }

          #if !defined(Analyis_Log) && !defined(Noise_Analysis)
            #if(logData)
            {
              simulation.printPositions();
            }
            #endif
          #endif

          #if !defined(Analyis_Log) && !defined(Noise_Analysis)
            #if Stop
              if(simulation.getSuccessRate()==1)
              {
                cout << "Target Reached" << endl;
                cout << "Time Taken = " << j << "steps" << endl;
                j = NoOfSteps;
                No_Of_Success++;
              }
            #endif
          #endif
          maxSR = max(maxSR,simulation.getSuccessRate());
        }

        #if !defined(Analyis_Log) && !defined(Noise_Analysis)
          //float SR = simulation.getSuccessRate();
          //cout << maxSR << endl;
          SuccessRate = SuccessRate + simulation.getSuccessRate();
        #endif


        #ifdef Analyis_Log
          pthread_mutex_lock(&mtx2);
          Analysis_Logfile << NoOfGroups << "," ;

          for(int i = 0; i < NoOfGroups; i++)
      		{
            Analysis_Logfile << noOfShepherd[i] << ",";
          }
          Analysis_Logfile << noOfObjects <<  ",";

          Analysis_Logfile.precision(std::numeric_limits<long double>::digits10 + 1);
          Analysis_Logfile << simulation.getTotalFitness();
          Analysis_Logfile.precision(-1);

          Analysis_Logfile << "," << maxSR << "," << simulation.getSuccessRate() << "\n";
          pthread_mutex_unlock(&mtx2);
        #endif

        #ifdef Noise_Analysis
          pthread_mutex_lock(&mtx2);
          Noise_Logfile << mode << "," << Noise_Level << "," << Noise_Scenario
          << ",";

          Noise_Logfile.precision(std::numeric_limits<long double>::digits10 + 1);
          Noise_Logfile << simulation.getTotalFitness();
          Noise_Logfile.precision(-1);

          Noise_Logfile << "," << maxSR << "," << simulation.getSuccessRate() << "\n";
          pthread_mutex_unlock(&mtx2);
        #endif
    }
    // cout << No_Of_Success << endl;
    #if !defined(Analyis_Log) && !defined(Noise_Analysis)
      cout << "Success rate = " << SuccessRate/No_Of_Trials << endl;
    #endif

    #ifdef Analyis_Log
      Analysis_Logfile.close();
    #endif

    #ifdef Noise_Analysis
      Noise_Logfile.close();
    #endif


  //Run optimization algorithm
  #elif(Optimise)
    omp_set_num_threads(No_Of_Threads);
    omp_set_dynamic(false);
    std::ofstream Parameters_Outfile;
    for(int i = 0; i < NoOfEvolutions; i++)
    {
      Parameters_Outfile.open("Results/Division_Of_Labour/Parameters_File.csv",std::ofstream::app);
      global_iter = 0;
      generation = 1;
      evolution++;
      out.open("Results/Division_Of_Labour/Optimisation_Logfile" + std::to_string(evolution) + ".csv");
      int dim = NoOfGroups*12; // problem dimensions.
      out << "Generation, Iteration, Trial,";
      for(int j=0; j<NoOfGroups; j++)
      {
        out << "Vl_1, Vr_1,Vl_2, Vr_2,Vl_3, Vr_3,Vl_4, Vr_4,Vl_5, Vr_5,Vl_6, Vr_6,";
      }
      out << "Fitness Value" << endl;

      std::vector<double> x0(dim,0);
      double sigma = 0.72;

      CMAParameters<> cmaparams(x0,sigma,lambda);
      //cmaparams._algo = BIPOP_CMAES;
      cmaparams.set_mt_feval(false); //Paralell eval
      cmaparams.set_quiet(false);
      cmaparams.set_fplot("Results/Division_Of_Labour/youroutput" + std::to_string(evolution) + ".dat");
      cmaparams.set_max_iter(MaxIter);
      /*{AUTOMAXITER,"The automatically set maximal number of iterations per run has been reached"},
      {TOLHISTFUN,"[Success] The optimization has converged"},
      {EQUALFUNVALS,"[Partial Success] The objective function values are the same over too many iterations, check the formulation of your objective function"},
      {TOLX,"[Partial Success] All components of covariance matrix are very small (e.g. < 1e-12)"},
      {TOLUPSIGMA,"[Error] Mismatch between step size increase and decrease of all eigenvalues in covariance matrix. Try to restart the optimization."},
      {STAGNATION,"[Partial Success] Median of newest values is not smaller than the median of older values"},
      {CONDITIONCOV,"[Error] The covariance matrix's condition number exceeds 1e14. Check out the formulation of your problem"},
      {NOEFFECTAXIS,"[Partial Success] Mean remains constant along search axes"},
      {NOEFFECTCOOR,"[Partial Success] Mean remains constant in coordinates"},
      {MAXFEVALS,"The maximum number of function evaluations allowed for optimization has been reached"},
      {MAXITER,"The maximum number of iterations specified for optimization has been reached"},
      {FTARGET,"[Success] The objective function target value has been reached"}};*/
      cmaparams.set_stopping_criteria(TOLHISTFUN,false);
      cmaparams.set_stopping_criteria(AUTOMAXITER,false);
      cmaparams.set_stopping_criteria(EQUALFUNVALS,false);
      cmaparams.set_stopping_criteria(FTARGET,false);
      CMASolutions cmasols = cmaes<>(ControllerFitness,cmaparams);
      std::cout << "best solution: " << cmasols << std::endl;
      std::cout << "optimization took " << cmasols.elapsed_time() / 1000.0 << " seconds\n";
      Parameters_Outfile << "Evolution "<< evolution << ",best solution: " << cmasols << "\n";
      Parameters_Outfile << "optimization took " << cmasols.elapsed_time() / 1000.0 << " seconds\n";
      out.close();
      //return cmasols.run_status();
      Parameters_Outfile.close();
    }
  #endif
}
