#include "ShepherdingGUI.h"
#include "Shepherding.h"
#include "cmaes.h"
#include <libconfig.h++>
#include <mutex>
#include <fstream>
#include <algorithm>
#include <omp.h>
// #ifdef USE_SDL
// #include </usr/include/SDL2/SDL.h>
// #endif

using namespace libconfig;
using namespace libcmaes;

pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
std::ofstream out;

int noOfSheep, noOfShepherd, noOfObjects, Csheep, Cshepherd, mode;
float Ksheep, K1, K2, KWall;
int Goalx, Goaly;
int global_iter;
int generation;
int evolution = 0;
int No_Of_Trials;
int NoOfSteps;
int dim;

//Function that runs simultion during evolutionary algorithm
FitFunc ControllerFitness = [](const double *x, const int N)
{
  pthread_mutex_lock(&mtx);
  global_iter++;
  if(global_iter>20)
  {
    global_iter = global_iter - 20;
    generation++;
  }
  int iter = global_iter;
  pthread_mutex_unlock(&mtx);
  int trial = 0;
  double fitness = 0;

  //Run simulation multiple times for randomness
  for(int i=0;i<No_Of_Trials;i++)
  {
    trial++;
    World world(300,300, Color(0.5,0.5,0.5));
    Shepherding simulation(&world,mode,noOfSheep,noOfShepherd,noOfObjects,Csheep,
      Cshepherd,Ksheep,K1, K2, KWall, Goalx, Goaly, x, dim);

      for (unsigned j=0; j < NoOfSteps; j++)
      {
        world.step(0.1);
        simulation.doPerTick();
      }
      pthread_mutex_lock(&mtx);
      out << generation << "," << iter << "," << trial << "," << x[0]
                << "," << x[1] << ","  << x[2] << "," << x[3] << "," << x[4]
                << "," << x[5] << ","  << x[6] << "," << x[7] << "," << x[8]
                << "," << x[9] << ","  << x[10] << "," << x[11] << "," << x[12]
                << "," << x[13] << "," << x[14]<< "," << x[15] << "," << simulation.getTotalFitness() << endl;
      pthread_mutex_unlock(&mtx);
      fitness = fitness + simulation.getTotalFitness();
  }
  return fitness/No_Of_Trials;
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
  bool GUI = configfile.lookup("GUI");
  bool Analysis = configfile.lookup("Analysis");
  bool logData = configfile.lookup("logData");
  bool Optimise = configfile.lookup("Optimise");
  No_Of_Trials = configfile.lookup("No_Of_Trials");
  NoOfSteps = configfile.lookup("NoOfSteps");
  int NoOfEvolutions = configfile.lookup("NoOfEvolutions");
  int MaxIter = configfile.lookup("MaxIter");
  int lambda = configfile.lookup("lambda"); // offsprings at each generation.
  int No_Of_Threads = configfile.lookup("No_Of_Threads");

  double x0[16] = { 4.47020,  2.25992,
                    -2.03707, 1.71585,
                    1.29139, 1.060159,
                    5.32278,  3.63879,
                    -1.25734, 2.58292,
                    -1.24890, 0.390242,
                    0,0,0,0}; //Anil's

  // double x0[16] = { 2.04404,  12.2577,
  //                   2.28314, -1.40369,
  //                   -1.11563, -6.81307,
  //                   4.50014,  7.06904,
  //                   1.77243, -4.61716,
  //                   -2.35163, -4.76965,
  //                   0,0,0,0};

  // double x0[16] =  {7.62906,  2.12052,
  //                   -0.208291, 4.76392,
  //                   -12.4398,  2.2009,
  //                   8.03278,  8.97639,
  //                   10.9182,  6.82779,
  //                   9.40439,  2.39981,
  //                   0,0,0,0};
  // double x0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  double x1[16] = { 1.62085,   4.87631,
                    7.00202,   3.59515,
                    0.253233,  0.375119,
                    -2.51459,  0.194949,
                    6.61425,   3.94885,
                    -1.80797, 0.0526062,
                    0,0,0,0};


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
    World world(300,300, Color(0.5,0.5,0.5), igt ? World::GroundTexture(gt.width(), gt.height(), bits) : World::GroundTexture());

    double x[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    if(mode == 0)
    {
      std::copy ( x0, x0+16, x);
      dim = 12;
    }
    else
    {
      std::copy ( x1, x1+16, x);
      dim = 16;
    }
    cout << "Mode = " << mode << endl;
    ShepherdingGUI viewer(&world, mode, noOfSheep,noOfShepherd,noOfObjects,Csheep,
      Cshepherd,Ksheep, K1, K2, KWall, Goalx, Goaly, x, dim);

      viewer.show();
      return app.exec();
  }
  //Analysis
  else if(Analysis)
  {
    double x[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    if(mode == 0)
    {
      std::copy ( x0, x0+16, x);
      dim = 12;
    }
    else
    {
      std::copy ( x1, x1+16, x);
      dim = 16;
    }

    int No_Of_Success = 0;
    omp_set_num_threads(No_Of_Threads);
    omp_set_dynamic(false);
    #pragma omp parallel for reduction(+:No_Of_Success)
    for(int i=0;i<No_Of_Trials;i++)
    {
      cout << "Trial " << i << endl;
      World world(300,300, Color(0.5,0.5,0.5));
      Shepherding simulation(&world,mode,noOfSheep,noOfShepherd,noOfObjects,Csheep,
        Cshepherd,Ksheep,K1, K2, KWall, Goalx, Goaly, x, dim);

        if(logData)
        {
          simulation.printHeaders();
        }

        for (unsigned j=0; j < NoOfSteps; j++)
        {
          world.step(0.1);
          simulation.doPerTick();
          if(logData)
          {
            simulation.printPositions();
          }
        }
    }
    // cout << No_Of_Success << endl;
    // cout << "Success rate = " << float(No_Of_Success)/float(No_Of_Trials) << endl;
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
      out << "Generation, Iteration, Trial, Vl_1, Vr_1, Vl_2, Vr_2, Vl_3, Vr_3, Vl_4, Vr_4,Vl_5, Vr_5,Vl_6, Vr_6,Vl_7, Vr_7,Vl_8, Vr_8, Fitness Value" << endl;
      if(mode == 2)
      {
        dim = 16; // problem dimensions.
      }
      else
      {
        dim = 12; // problem dimensions.
      }
      std::vector<double> x0(dim,0);
      double sigma = 0.72;

      CMAParameters<> cmaparams(x0,sigma,lambda);
      //cmaparams._algo = BIPOP_CMAES;
      cmaparams.set_mt_feval(true); //Paralell eval
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
