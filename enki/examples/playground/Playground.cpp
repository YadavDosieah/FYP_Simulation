#include "ShepherdingGUI.h"
#include "Shepherding.h"
#include "cmaes.h"
#include <libconfig.h++>
// #ifdef USE_SDL
// #include </usr/include/SDL2/SDL.h>
// #endif

using namespace libconfig;
using namespace libcmaes;

int noOfSheep, noOfShepherd, Csheep, Cshepherd;
float Ksheep, K1, K2;
int Goalx, Goaly;

FitFunc ControllerFitness = [](const double *x, const int N)
{
  World world(300,300, Color(0.5,0.5,0.5));
  Shepherding simulation(&world,noOfSheep,noOfShepherd,Csheep,Cshepherd,Ksheep,
    K1, K2, Goalx, Goaly, x);
    // cout << "Optimisation\n";
    for (unsigned i=0; i < 15000; i++)
    {
      world.step(0.1);
      simulation.doPerTick();
    }
    return simulation.getFitness();
  };

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
    const double x[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    ShepherdingGUI viewer(&world,noOfSheep,noOfShepherd,Csheep,Cshepherd,Ksheep,
      K1, K2, Goalx, Goaly, x);

      viewer.show();
      // Run for some times

      return app.exec();
  }
  else if(Optimise)
  {
    int dim = 12; // problem dimensions.
    std::vector<double> x0(dim,0);
    double sigma = 0.72;
    //int lambda = 100; // offsprings at each generation.
    CMAParameters<> cmaparams(x0,sigma);
    //cmaparams._algo = BIPOP_CMAES;
    cmaparams.set_fplot("youroutput.dat");
    CMASolutions cmasols = cmaes<>(ControllerFitness,cmaparams);
    std::cout << "best solution: " << cmasols << std::endl;
    std::cout << "optimization took " << cmasols.elapsed_time() / 1000.0 << " seconds\n";
    return cmasols.run_status();
  }
}
