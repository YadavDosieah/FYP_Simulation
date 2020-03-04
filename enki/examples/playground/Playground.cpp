#include "ShepherdingGUI.h"
#include "Shepherding.h"
#include "cmaes.h"
#include <libconfig.h++>
#include <mutex>
#include <fstream>

// #ifdef USE_SDL
// #include </usr/include/SDL2/SDL.h>
// #endif

using namespace libconfig;
using namespace libcmaes;

pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
std::ofstream out;

int noOfSheep, noOfShepherd, Csheep, Cshepherd;
float Ksheep, K1, K2, KWall;
int Goalx, Goaly;
int global_iter = 0;
int generation = 1;
int No_Of_Trials = 50;


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
  for(int i=0;i<No_Of_Trials;i++)
  {
    trial++;
    World world(300,300, Color(0.5,0.5,0.5));
    Shepherding simulation(&world,noOfSheep,noOfShepherd,Csheep,Cshepherd,Ksheep,
      K1, K2, KWall, Goalx, Goaly, x);
      // cout << "Optimisation\n";

      for (unsigned i=0; i < 15000; i++)
      {
        world.step(0.1);
        simulation.doPerTick();
      }
      pthread_mutex_lock(&mtx);
      out << generation << "," << iter << "," << trial << "," << simulation.getFitness() << endl;
      pthread_mutex_unlock(&mtx);
      fitness = fitness + simulation.getFitness();
  }
  return fitness/No_Of_Trials;
};


// FitFunc ff = [&](const double *x, const int N)
// {
//   std::lock_guard<std::mutex> lck(fmtx);
//   double fval = (*ControllerFitness)(x);
//   return fval;
// };

int main(int argc, char *argv[])
{
	Config configfile;
	configfile.readFile("Parameters.cfg");
	noOfSheep = configfile.lookup("noOfSheep");
	noOfShepherd = configfile.lookup("noOfShepherd");
	Csheep = configfile.lookup("Csheep");
	Cshepherd = configfile.lookup("Cshepherd");
	Ksheep = configfile.lookup("Ksheep");
	K1 = configfile.lookup("K1");
	K2 = configfile.lookup("K2");
  KWall = configfile.lookup("KWall");
  cout << KWall;
	Goalx = configfile.lookup("Goalx");
	Goaly = configfile.lookup("Goaly");
  bool GUI = configfile.lookup("GUI");
  bool Optimise = configfile.lookup("Optimise");

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
    const double x[12] = {-0.464966806865609,   4.87464109745684,
                          -3.22016622077874,  -10.5962527041542,
                          -1.20562993521035,   12.0946227366324,
                          0.308720591678681,  7.80713621843201,
                          -3.65810223767648,  -2.13303614650118,
                          5.59638221361075,  -1.24182746118171};
    // const double x[12] =  {0,0,0,0,0,0,0,0,0,0,0,0} ;
    ShepherdingGUI viewer(&world,noOfSheep,noOfShepherd,Csheep,Cshepherd,Ksheep,
      K1, K2, KWall, Goalx, Goaly, x);

      viewer.show();
      // Run for some times

      return app.exec();
  }
  else if(Optimise)
  {
    out.open("Optimisation_Logfile.csv");
  	out << "Generation, Iteration, Trial, Fitness Value" << endl;
    int dim = 12; // problem dimensions.
    std::vector<double> x0(dim,0);
    double sigma = 0.72;
    int lambda = 20; // offsprings at each generation.
    CMAParameters<> cmaparams(x0,sigma,lambda);
    //cmaparams._algo = BIPOP_CMAES;
    cmaparams.set_mt_feval(true); //Paralell eval
    cmaparams.set_quiet(false);
    cmaparams.set_fplot("youroutput.dat");
    cmaparams.set_max_iter(80);
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
    out.close();
    return cmasols.run_status();
  }
}
