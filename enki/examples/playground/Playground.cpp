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

int noOfSheep, noOfShepherd, noOfObjects, Csheep, Cshepherd, mode;
float Ksheep, K1, K2, KWall, GoalRadius, GoalDistance, Sheep_MaxSpeed_Multiplier;
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
    Shepherding simulation(&world,mode,noOfSheep,noOfShepherd,noOfObjects,Csheep,
      Cshepherd,Ksheep,K1, K2, KWall, Goalx, Goaly,GoalRadius, GoalDistance,
      Sheep_MaxSpeed_Multiplier, x, N);

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
  Sheep_MaxSpeed_Multiplier = configfile.lookup("Sheep_MaxSpeed_Multiplier");
  //cout << KWall;
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

  #if (Gui || Analysis) && !defined(Post_Eval)
  // double x0[16] = { 4.47020,  2.25992,
  //                   -2.03707, 1.71585,
  //                   1.29139, 1.060159,
  //                   5.32278,  3.63879,
  //                   -1.25734, 2.58292,
  //                   -1.24890, 0.390242,
  //                   0,0,0,0}; //Anil's

  // double x0[16] = { 5.1175,  1.49066,
  //                   -1.02706,  1.72113,
  //                   1.42577,  0.19968,
  //                   0.658893,  4.20533,
  //                   -1.28859, 0.402441,
  //                   0.968842,   1.1955,
  //                   0,0,0,0};//Evo 1


 // double x0[16] = {  1.46536,     5.029,
 //                    3.86595, -0.423476,
 //                    -1.42011,  0.591789,
 //                    4.85686, -0.433813,
 //                    3.30068,   2.27523,
 //                    -2.14908,   2.71443,
 //                    0,0,0,0};//Evo 2


  double x0[12] = { 8.54883,  1.98728,
                    0.613737,  6.37663,
                    0.329281, -1.10761,
                    1.05533,  2.69364,
                    0.846464,    3.117,
                    -10.1579, 0.649279};//Evo 4

// double x0[16] = {   1.47637276371257,   7.79745996832812,
//                     6.42601580665163,  -1.32876317508355,
//                     -0.174182310770176, -0.890900773539272,
//                     3.26664619587021,   -2.7323945160532,
//                     6.51486214529354,  -8.05755277995334,
//                     -7.8455360832917,  -4.62744928131071,
//                     0,0,0,0};//Rerun Evo 10

// double x0[16] = {   6.65482,   1.53632,
//                     -0.551062,   4.04329,
//                     0.165422,  -3.84506,
//                     -2.32391,   3.29178,
//                     0.839847,   1.99268,
//                     3.33858,  -2.80341,
//                     0,0,0,0};//Rerun Evo 28


// double x0[16] = {   6.31079348196655,   1.61196899331601,
//                     -0.537008442153642,   5.43337478434273,
//                     2.50445523929584, -0.645910301917617,
//                     -0.251274076963363,   5.70300766090279,
//                     0.162006352417641,   1.20122190545975,
//                     -1.35046215506816,  -1.17626530866892,
//                     0,0,0,0};//Rerun Evo 5


  // double x1[16] = { 1.62085,   4.87631,
  //                   7.00202,   3.59515,
  //                   0.253233,  0.375119,
  //                   -2.51459,  0.194949,
  //                   6.61425,   3.94885,
  //                   -1.80797, 0.0526062,
  //                   0,0,0,0};//Arch. Evo 1
  //
  // double x1[16] = { 1.4226,    5.11139,
  //                   8.71069,    4.27825,
  //                   0.00327356,  -0.447402,
  //                   1.44861,    4.96937,
  //                   1.38475,    8.23185,
  //                   0.0966084,   -5.95994,
  //                   0,0,0,0};//Evo 3 - Not good


  // double x1[16] = { 9.72565,  1.25369,
  //                   3.40784,  9.64805,
  //                   -0.39977, -0.36701,
  //                   3.65284, 0.918264,
  //                   4.14598,  1.10587,
  //                   5.41277, -3.71182,
  //                   0,0,0,0};//Evo 1 - Not good

// double x1[16] = { 12.8278,  1.37234,
//                   1.55537,  8.38157,
//                   8.60962, -5.74115,
//                   7.87746,  1.10101,
//                   8.97432,  4.55264,
//                   -6.65248,   2.5631,
//                   0,0,0,0};//Evo 4 --Not Good
// double x1[16] = { 0.875843,  3.98296,
//                   6.89279,  2.36443,
//                   2.72799, 0.245733,
//                   -1.25524,  2.21442,
//                   4.15631,  2.58278,
//                   -1.18257,  6.30556,
//                   0,0,0,0}; //Evo 4 v2 - ok?

double x1[12] = { 11.7619,   1.19117,
                  2.52318,   11.9229,
                  6.28643,   1.26785,
                  -0.696947,   2.73256,
                  1.08471,  -3.25945,
                  -10.3829,   1.37449};//Evo 4 v3

// double x1[16] = { 6.39921,    1.53387,
//                   3.27998,     5.6971,
//                   -1.06653,  -0.310626,
//                   4.58632,   0.140127,
//                   3.02105,    1.60247,
//                   -0.0424818,   -2.70668,
//                   0,0,0,0};//Rerun Evo 27

// double x1[16] = { 1.25098,  16.8782,
//                   10.0743,  2.59443,
//                   3.58004, -16.0966,
//                   -1.1735,  2.81624,
//                   7.14517,  15.5674,
//                   1.58428,  5.95882,
//                   0,0,0,0};//Rerun Evo 27

// double x1[16] = { 1.44331,   5.81528,
//                   5.43122,   2.43779,
//                   -2.0279, -0.908411,
//                   -3.37443,  0.471246,
//                   5.86835,   2.23802,
//                   -4.44833,   2.08824,
//                   0,0,0,0};//Rerun Evo 22

  // double x2[16] = { 5.5242,    1.4171,
  //                   3.35711,   11.8144,
  //                   -0.839286,   -2.24036,
  //                   -0.928745,   7.15171,
  //                   5.05223,   -0.335071,
  //                   6.69508,   5.08438,
  //                   0.523579,   -5.96249,
  //                   -0.539203,   4.00641};

  // double x2[16] = { 1.44757,  13.1821,
  //                   11.4065,  3.97144,
  //                   -1.83888, -4.51648,
  //                   12.726, -7.80834,
  //                   3.47325,  6.39449,
  //                   4.57125, -11.2938,
  //                   -3.38478, -3.26062,
  //                   4.31573, -3.78404};

  double x2[16] = { 1.49992,    13.3238,
                    12.7281,    2.90316,
                    -0.0970288,    6.89836,
                    14.1324,   -8.02044,
                    9.75644,   -1.57262,
                    1.55119,    10.6471,
                    9.54676,   -2.38561,
                    4.40578,   -1.76815};//Evo 4

// double x2[16] = { 14.4944,  1.42862,
//                   3.76818,  5.15606,
//                   -9.76129, -7.04812,
//                   -14.9875,  9.21298,
//                   6.22934,  10.5726,
//                   4.56196,  0.84798,
//                   13.0375,  15.3517,
//                   -12.0532, 0.961111};//Rerun Evo 4
// double x2[16] = { 1.66835,  6.21536,
//                   11.4458,  3.98066,
//                   0.668355,  1.13669,
//                   8.52617, -4.01594,
//                   3.63397,  4.78214,
//                   5.40997, -8.98858,
//                   1.46185, 0.985173,
//                   -1.40889,  5.81565};//Rerun Evo 24

// double x2[16] = { 7.93988,   1.28629,
//                   3.10843,   5.19092,
//                   0.370215, -0.721604,
//                   -1.37954,   4.47404,
//                   -1.2666,   4.23055,
//                   1.47551, 0.0939253,
//                   -1.64169,  -1.72524,
//                   -2.03186,   1.59506};//Rerun Evo 27

  // double x2[16] = { 7.6066,    1.6498,
  //                   3.43017,    7.7092,
  //                   0.648434, -0.947013,
  //                   -6.96371,    8.4718,
  //                   4.78765,   4.24262,
  //                   -5.0149,  0.801766,
  //                   -1.14734,  -5.17562,
  //                   -3.0852,   1.73464};//Rerun Evo 2



  // double x3[16] = { 1.5521250596654,  7.38527612759293,
  //                   12.002738271923,  1.64270681779971,
  //                   17.3689922933527, -5.69151784793263,
  //                   9.15976141325661,  4.29874652437399,
  //                   0,0,0,0,0,0,0,0}; //Final

  // double x3[16] = { 14.5441, 0.307065,
  //                   -0.326968,   7.00522,
  //                   4.39762,   5.21863,
  //                   2.18167,   6.50076,
  //                   0,0,0,0,0,0,0,0}; //Re-run Evo 20

  // double x3[16] = { 5.12201,  0.318505,
  //                   -0.375725,   3.64858,
  //                   4.31782,    4.0153,
  //                   3.01553,   4.57586,
  //                   0,0,0,0,0,0,0,0}; //Re-run Evo 8

  // double x3[16] = { 0.412552,   5.84604,
  //                   5.02832, -0.454589,
  //                   5.5003,  -4.48167,
  //                   -2.24078,   3.19676,
  //                     0,0,0,0,0,0,0,0}; //Re-run Evo 18

  double x3[8] = { 11.5616,  1.57654,
                    1.46906,  6.19461,
                    -13.4256,  2.47699,
                    -2.07122,  5.58177};//Re-run Evo 22


 // double x4[16] = {  1.61232669709324,  5.11330101906351,
 //                    1.06344429100577, -2.94592926223512,
 //                    3.06706892590647, 0.328710367373353,
 //                    1.38356423294492,  3.20571932523051,
 //                    0,0,0,0,0,0,0,0}; //Simplified trained in S.A - Evo 24

 double x4[8] = { 6.32578186589613,  1.60511591621045,
                  -2.8594112055925,  1.57035966006463,
                  0.987657980752346,  3.22181985917952,
                  1.81915276300813,  5.42701623231154}; //Simplified trained in S.A - Evo 15


  // double x5[16] = { 10.565,  1.46622,
  //                   2.95645,  6.88097,
  //                   0.944368, -1.68692,
  //                   -1.54675, -5.37055,
  //                   0,0,0,0,0,0,0,0}; //Simplified trained in S.B - Evo 30

  // double x5[16] = { 1.64345,   14.2974,
  //                   17.9004,   3.23616,
  //                   -13.3463, -0.244719,
  //                   4.98966,   3.69208,
  //                   0,0,0,0,0,0,0,0}; //Simplified trained in S.B - Evo 29

  double x5[8] = { 1.56189,  8.60265,
                    7.86989,  2.54455,
                    -3.78878,  6.94438,
                    1.45707,  2.57578}; //Simplified trained in S.B - Evo 21

  double x6[32] = { 7.19082,  1.50769,
                    3.59987,  6.41254,
                    3.66301,   1.0439,
                    -6.32858,  4.52046,
                    -1.32855,  2.97883,
                    1.98907,  0.67343,
                    -2.76998, -1.96865,
                    2.43246,  1.30969,
                    0.502497,  4.97725,
                    4.69878, 0.304761,
                    5.00095, -2.42564,
                    4.18128, -4.19175,
                    2.95004,  1.20124,
                    3.7609, -1.44454,
                    2.19637, -4.25928,
                    4.79388,  3.28435};//Evo 25

  // double x6[32] = { 6.69059,    1.12129,
  //                   2.9219,    5.21433,
  //                   1.77482,   0.536469,
  //                   -4.64549,    4.97861,
  //                   1.29094,    2.27983,
  //                   0.984329,   0.166457,
  //                   0.898485,    1.72334,
  //                   -0.913469,    2.15659,
  //                   0.708841,    2.37201,
  //                   3.22795,  0.0214118,
  //                   1.10231,   -4.67734,
  //                   0.625593,   -1.34397,
  //                   3.25957, -0.0127892,
  //                   -0.40751,   -1.24928,
  //                   1.52867,  -0.205116,
  //                   0.0788397,  -0.928116}; //Evo 19

  double x7[32] = {8.54883,  1.98728,
                    0.613737,  6.37663,
                    0.613737,  6.37663,
                    0.329281, -1.10761,
                    1.05533,  2.69364,
                    0.846464,    3.117,
                    0.846464,    3.117,
                    -10.1579, 0.649279,
                    11.7619,   1.19117,
                    2.52318,   11.9229,
                    2.52318,   11.9229,
                    6.28643,   1.26785,
                    -0.696947,   2.73256,
                    1.08471,  -3.25945,
                    1.08471,  -3.25945,
                    -10.3829,   1.37449};
  // double x7[32] = {x2,x2};
  // double x7[32];
  // std::copy ( x2, x2+16, x7);
  // std::copy ( x2, x2+16, x7+16);


  #endif

  //Run simulation with GUI
  #if(GUI)
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
    else if(mode == 3)
    {
      std::copy ( x3, x3+16, x);
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
  //Analysis
  #elif(Analysis)
    #ifdef Analyis_Log
      std::ofstream Analysis_Logfile;
      Analysis_Logfile.open("Analysis.csv",std::ios_base::app);
    #endif

    #ifdef Noise_Analysis
      std::ofstream Noise_Logfile;
      Noise_Logfile.open("Noise.csv",std::ios_base::app);
    #endif

    #ifdef Post_Eval
    double x[32] = {configfile.lookup("x0"),configfile.lookup("x1"),
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
                    configfile.lookup("x22"),configfile.lookup("x23"),
                    configfile.lookup("x24"),configfile.lookup("x25"),
                    configfile.lookup("x26"),configfile.lookup("x27"),
                    configfile.lookup("x28"),configfile.lookup("x29"),
                    configfile.lookup("x30"),configfile.lookup("x31")};
    // for(int i=0;i<32;i++)
    // {
    //   cout << x[i] << ",";
    // }
    // cout << endl << endl;
    int dim;
    switch (mode) {
      case 0:
        dim = 12;
        break;
      case 1:
        dim = 12;
        break;
      case 2:
        dim = 16;
        break;
      case 3:
        dim = 8;
        break;
      case 6:
        dim = 32;
      default:
        dim = 16;
    }
    #else
      double x[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
      int dim;
      if(mode == 0)
      {
        std::copy ( x0, x0+12, x);
        dim = 12;
      }
      else if(mode == 1)
      {
        std::copy ( x1, x1+12, x);
        dim = 12;
      }
      else if(mode == 2)
      {
        std::copy ( x2, x2+16, x);
        dim = 16;
      }
      else if(mode == 3)
      {
        std::copy ( x3, x3+8, x);
        dim = 8;
      }
      else if(mode == 4)
      {
        std::copy ( x4, x4+8, x);
        dim = 8;
        mode = 3;
      }
      else if(mode == 5)
      {
        std::copy ( x5, x5+8, x);
        dim = 8;
        mode = 3;
      }
      else if(mode == 6)
      {
        std::copy ( x6, x6+32, x);
        dim = 32;
      }
      else if(mode == 7)
      {
        std::copy ( x7, x7+32, x);
        mode = 6;
        dim = 32;
      }
      else
      {
        dim = 32;
      }
      // for(int i=0;i<32;i++)
      // {
      //   cout << x[i] << ",";
      // }
      // cout << endl << endl;
    #endif
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
        Shepherding simulation(&world,mode,noOfSheep,noOfShepherd,noOfObjects,Csheep,
          Cshepherd,Ksheep,K1, K2, KWall, Goalx, Goaly,GoalRadius, GoalDistance,
          Sheep_MaxSpeed_Multiplier, x, dim);
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
          Analysis_Logfile << mode << "," << noOfShepherd << "," << noOfSheep
          << "," << noOfObjects <<  ",";

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
      Parameters_Outfile.open("Results/mode" + std::to_string(mode) + "/Parameters_File.csv",std::ofstream::app);
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
      else if(mode == 3)
      {
        dim = 8;
        out << "Generation, Iteration, Trial, Vl_1, Vr_1, Vl_2, Vr_2, Vl_3, Vr_3, Vl_4, Vr_4, Fitness Value" << endl;
      }
      else if(mode == 6)//Heterogenous shepherds
      {
        dim = 32; // problem dimensions.
        out << "Generation, Iteration, Trial, Vl_1.1, Vr_1.1, Vl_1.2, Vr_1.2, Vl_1.3, Vr_1.3, Vl_1.4, Vr_1.4,Vl_1.5, Vr_1.5,Vl_1.6, Vr_1.6,Vl_1.7, Vr_1.7,Vl_1.8, Vr_1.8, Vl_2.1, Vr_2.1, Vl_2.2, Vr_2.2, Vl_2.3, Vr_2.3, Vl_2.4, Vr_2.4,Vl_2.5, Vr_2.5,Vl_2.6, Vr_2.6,Vl_2.7, Vr_2.7,Vl_2.8, Vr_2.8, Fitness Value" << endl;
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
      Parameters_Outfile << "Evolution "<< evolution << ",best solution: " << cmasols << "\n";
      Parameters_Outfile << "optimization took " << cmasols.elapsed_time() / 1000.0 << " seconds\n";
      out.close();
      //return cmasols.run_status();
      Parameters_Outfile.close();
    }
  #endif
}
