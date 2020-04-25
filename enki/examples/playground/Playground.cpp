#include "ShepherdingGUI.h"
#include "Shepherding.h"
#include "cmaes.h"
#include <libconfig.h++>
#include <mutex>
#include <fstream>
#include <algorithm>
#include <omp.h>
#include <iomanip>
// #ifdef USE_SDL
// #include </usr/include/SDL2/SDL.h>
// #endif

using namespace libconfig;
using namespace libcmaes;

std::mutex mtx;
std::ofstream out;
pthread_mutex_t mtx2=PTHREAD_MUTEX_INITIALIZER;

int noOfSheep, noOfShepherd, noOfObjects, Csheep, Cshepherd, mode;
float Ksheep, K1, K2, KWall, GoalRadius, GoalDistance;
int Goalx, Goaly, Xbound, Ybound;
int global_iter;
int generation;
int lambda;
int evolution = 0;
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
    Shepherding simulation(&world,mode,noOfSheep,noOfShepherd,noOfObjects,Csheep,
      Cshepherd,Ksheep,K1, K2, KWall, Goalx, Goaly,GoalRadius, GoalDistance, x, N);

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
  //Read configuration file
	Config configfile;
	configfile.readFile("Parameters.cfg");
  mode = configfile.lookup("mode");
	noOfSheep = configfile.lookup("noOfSheep");
	noOfShepherd = configfile.lookup("noOfShepherd");
  noOfObjects = configfile.lookup("noOfObjects");
	Csheep = configfile.lookup("Csheep");
	Cshepherd = configfile.lookup("Cshepherd");
	Ksheep = configfile.lookup("Ksheep");
	K1 = configfile.lookup("K1");
	K2 = configfile.lookup("K2");
  KWall = configfile.lookup("KWall");
  //cout << KWall;
	Goalx = configfile.lookup("Goalx");
	Goaly = configfile.lookup("Goaly");
  GoalRadius = configfile.lookup("GoalRadius");
  GoalDistance = configfile.lookup("GoalDistance");
  Xbound = configfile.lookup("Xbound");
  Ybound = configfile.lookup("Ybound");
  bool GUI = configfile.lookup("GUI");
  bool Analysis = configfile.lookup("Analysis");
  bool logData = configfile.lookup("logData");
  bool Optimise = configfile.lookup("Optimise");
  bool Stop = configfile.lookup("Stop");
  No_Of_Trials = configfile.lookup("No_Of_Trials");
  NoOfSteps = configfile.lookup("NoOfSteps");
  int NoOfEvolutions = configfile.lookup("NoOfEvolutions");
  int MaxIter = configfile.lookup("MaxIter");
  lambda = configfile.lookup("lambda"); // offsprings at each generation.
  int No_Of_Threads = configfile.lookup("No_Of_Threads");
  omp_set_num_threads(No_Of_Threads);
  omp_set_dynamic(false);

  // double x0[16] = { 4.47020,  2.25992,
  //                   -2.03707, 1.71585,
  //                   1.29139, 1.060159,
  //                   5.32278,  3.63879,
  //                   -1.25734, 2.58292,
  //                   -1.24890, 0.390242,
  //                   0,0,0,0}; //Anil's

  double x0[16] = { 5.1175,  1.49066,
                    -1.02706,  1.72113,
                    1.42577,  0.19968,
                    0.658893,  4.20533,
                    -1.28859, 0.402441,
                    0.968842,   1.1955,
                    0,0,0,0};


  double x1[16] = { 1.62085,   4.87631,
                    7.00202,   3.59515,
                    0.253233,  0.375119,
                    -2.51459,  0.194949,
                    6.61425,   3.94885,
                    -1.80797, 0.0526062,
                    0,0,0,0};

  // double x1[16] = { 9.72565,  1.25369,
  //                   3.40784,  9.64805,
  //                   -0.39977, -0.36701,
  //                   3.65284, 0.918264,
  //                   4.14598,  1.10587,
  //                   5.41277, -3.71182,
  //                   0,0,0,0};

  double x2[16] = { 5.5242,    1.4171,
                    3.35711,   11.8144,
                    -0.839286,   -2.24036,
                    -0.928745,   7.15171,
                    5.05223,   -0.335071,
                    6.69508,   5.08438,
                    0.523579,   -5.96249,
                    -0.539203,   4.00641};

  double xn[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  //Run simulation with GUI
  if(GUI)
  {
    QApplication app(argc, argv);

    // Create the world and the viewer
    bool igt(app.arguments().size() > 1);
    QImage gt;
    if (igt)
      gt = QGLWidget::convertToGLFormat(QImage(app.arguments().last()));
    igt = !gt.isNull();
    #if QT_VERSION >= QT_VERSION_CHECK(4,7,0)
    const uint32_t *bits = (const uint32_t*)gt.constBits();
    #else
    uint32_t *bits = (uint32_t*)gt.bits();
    #endif

    //Size of world (arena)
    World world(Xbound,Ybound, Color(0.5,0.5,0.5), igt ? World::GroundTexture(gt.width(), gt.height(), bits) : World::GroundTexture());

    double x[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int dim;
    if(mode == 0)
    {
      std::copy ( x0, x0+16, x);
      dim = 12;
    }
    else if(mode == 1)
    {
      std::copy ( x1, x1+16, x);
      dim = 12;
    }
    else if(mode == 2)
    {
      std::copy ( x2, x2+16, x);
      dim = 16;
    }
    else
    {
      std::copy ( xn, xn+16, x);
      dim = 16;
    }
    cout << "Mode = " << mode << endl;
    ShepherdingGUI viewer(&world, mode, noOfSheep,noOfShepherd,noOfObjects,Csheep,
      Cshepherd,Ksheep, K1, K2, KWall, Goalx, Goaly,GoalRadius, x, dim);

      viewer.show();
      return app.exec();
  }
  //Analysis
  else if(Analysis)
  {
    double x[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int dim;
    if(mode == 0)
    {
      std::copy ( x0, x0+16, x);
      dim = 12;
    }
    else if(mode == 1)
    {
      std::copy ( x1, x1+16, x);
      dim = 12;
    }
    else if(mode == 2)
    {
      std::copy ( x2, x2+16, x);
      dim = 16;
    }
    else
    {
      std::copy ( xn, xn+16, x);
      dim = 16;
    }

    int No_Of_Success = 0;
    float SuccessRate = 0;
    omp_set_num_threads(No_Of_Threads);
    omp_set_dynamic(false);
    #pragma omp parallel for reduction(+:No_Of_Success,SuccessRate)
    for(int i=0;i<No_Of_Trials;i++)
    {
      cout << "Trial " << i << endl;
      startAnalysis:
      World world(Xbound,Ybound, Color(0.5,0.5,0.5));
      Shepherding simulation(&world,mode,noOfSheep,noOfShepherd,noOfObjects,Csheep,
        Cshepherd,Ksheep,K1, K2, KWall, Goalx, Goaly,GoalRadius, GoalDistance, x, dim);

        if(logData)
        {
          simulation.printHeaders();
        }

        for (unsigned j=0; j < NoOfSteps; j++)
        {
          world.step(0.1);
          int ReturnVal = simulation.doPerTick();
          if(ReturnVal == 1)
          {
            cout << "Trial restarting\n";
            goto startAnalysis;
          }

          if(logData)
          {
            simulation.printPositions();
          }

          if(simulation.getSuccessRate()==1 && Stop)
          {
            cout << "Target Reached" << endl;
            cout << "Time Taken = " << j << "steps" << endl;
            j = NoOfSteps;
            No_Of_Success++;
          }
        }
        float SR = simulation.getSuccessRate();
        cout << SR << endl;
        SuccessRate = SuccessRate + SR;

    }
    // cout << No_Of_Success << endl;
    cout << "Success rate = " << SuccessRate/No_Of_Trials << endl;
  }

  //Run optimization algorithm
  else if(Optimise)
  {
    omp_set_num_threads(No_Of_Threads);
    omp_set_dynamic(false);
    std::ofstream Parameters_Outfile;
    Parameters_Outfile.open("Results/mode" + std::to_string(mode) + "/Parameters_File.csv");
    for(int i = 0; i < NoOfEvolutions; i++)
    {
      global_iter = 0;
      generation = 1;
      evolution++;
      out.open("Results/mode" + std::to_string(mode) + "/Optimisation_Logfile" + std::to_string(evolution) + ".csv");
      int dim;
      if(mode == 2)
      {
        dim = 16; // problem dimensions.
        out << "Generation, Iteration, Trial, Vl_1, Vr_1, Vl_2, Vr_2, Vl_3, Vr_3, Vl_4, Vr_4,Vl_5, Vr_5,Vl_6, Vr_6,Vl_7, Vr_7,Vl_8, Vr_8, Fitness Value" << endl;
      }
      else
      {
        dim = 12; // problem dimensions.
        out << "Generation, Iteration, Trial, Vl_1, Vr_1, Vl_2, Vr_2, Vl_3, Vr_3, Vl_4, Vr_4,Vl_5, Vr_5,Vl_6, Vr_6, Fitness Value" << endl;
      }
      std::vector<double> x0(dim,0);
      double sigma = 0.72;

      CMAParameters<> cmaparams(x0,sigma,lambda);
      //cmaparams._algo = BIPOP_CMAES;
      cmaparams.set_mt_feval(false); //Paralell eval
      cmaparams.set_quiet(false);
      cmaparams.set_fplot("Results/mode" + std::to_string(mode) + "/youroutput" + std::to_string(evolution) + ".dat");
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
      Parameters_Outfile << evolution << ",best solution: " << cmasols << "\n";
      Parameters_Outfile << "optimization took " << cmasols.elapsed_time() / 1000.0 << " seconds\n";
      out.close();
      //return cmasols.run_status();
    }
    Parameters_Outfile.close();
  }
}
