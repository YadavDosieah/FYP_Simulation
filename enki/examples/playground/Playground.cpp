#include <viewer/Viewer.h>
#include <enki/PhysicalEngine.h>
#include <enki/robots/e-puck/EPuck.h>
#include <QApplication>
#include <QtGui>
#include <iostream>
#include "stdio.h"
#include <cstdlib>
#include "time.h"
#include "math.h"
// #ifdef USE_SDL
// #include </usr/include/SDL2/SDL.h>
// #endif

#define SPEED_MAX 13.
#define noOfSheep 10
#define noOfShepherd 1
#define Csheep 100
#define Csheepdog 5000
#define Ksheep 0.5
#define K1 2
#define K2 1.3
#define Goalx 150
#define Goaly 250

using namespace Enki;
using namespace std;

class Shepherding: public ViewerWidget
{
	protected:
		QVector<EPuck*> shepherds;
		QVector<EPuck*> flock;

	public:
	Shepherding(World *world, QWidget *parent = 0) :
	ViewerWidget(world, parent)
	{
		srand(time(0));
		for(int i = 0; i < noOfSheep; i++)
		{
			addRobots(world, &flock, false);
		}

		for(int i = 0; i < noOfShepherd; i++)
		{
			addRobots(world, &shepherds, true);
		}

		PhysicalObject* Goal = new PhysicalObject;
		Goal->pos = Point(Goalx,Goaly);
		Goal->setCylindric(25, 4, 1000000);
		Goal->dryFrictionCoefficient = 100;
		Goal->setColor(Color(0, 0, 1));
		Goal->collisionElasticity = 0;
		world->addObject(Goal);
		//
		// for (unsigned i=0; i<1000; i++)
		// {
		// 			// step of 50 ms
		// 			world->step(0.01);
		// 			this->DoPerTick();
		// 			cout << i << endl;
		//  }
	}

	virtual void timerEvent(QTimerEvent * event)
	{
		ViewerWidget::timerEvent(event);
		for(int i = 0; i < noOfShepherd; i++)
		{
			valarray<Color> image = shepherds[i]->camera.image;
			std::cout << "New Image" << std::endl;
			for (size_t i = 0; i < image.size(); i++)
			{
				std::cout << image[i] << std::endl;
			}
		}

		// Point P = shepherds[0]->camera.getAbsolutePosition();
		// cout << P << "\t" << shepherds[0]->pos.x << "," << shepherds[0]->pos.y << endl;
		for(int i=0; i < int(flock.size()); i++)
		{
			double Force_x = 0;
			double Force_y = 0;
			for(int j=0; j < int(flock.size()); j++)
			{
				if(j != i)
				{
					double Distance_x = flock[i]->pos.x - flock[j]->pos.x;
					double Distance_y = flock[i]->pos.y - flock[j]->pos.y;
					double Distance_sq = pow(Distance_x,2) + pow(Distance_y,2);
					if(Distance_sq < 100)
					{
						Force_x = Force_x + (Csheep/Distance_sq)*(Distance_x/sqrt(Distance_sq));
						Force_y = Force_y + (Csheep/Distance_sq)*(Distance_y/sqrt(Distance_sq));
					}
					if(Distance_sq > 500 && Distance_sq < 1000)
					{
						Force_x = Force_x - Ksheep*Distance_sq*(Distance_x/sqrt(Distance_sq));
						Force_y = Force_y - Ksheep*Distance_sq*(Distance_y/sqrt(Distance_sq));
					}
				}
			}
			double Force_Sheep_x = Force_x;
			double Force_Sheep_y = Force_y;
			bool ShepherdDetected = false;
			Force_x = 0;
			Force_y = 0;
			for(int j=0; j < int(shepherds.size()); j++)
			{
					double Distance_x = flock[i]->pos.x - shepherds[j]->pos.x;
					double Distance_y = flock[i]->pos.y - shepherds[j]->pos.y;
					double Distance_sq = pow(Distance_x,2) + pow(Distance_y,2);
					if(Distance_sq < 1500)
					{
						ShepherdDetected = true;
						Force_x = Force_x + (Csheepdog/Distance_sq)*(Distance_x/sqrt(Distance_sq));
						Force_y = Force_y + (Csheepdog/Distance_sq)*(Distance_y/sqrt(Distance_sq));
					}
			}
			double Angle = flock[i]->angle;
			if (ShepherdDetected != true)
			{
				Force_x = Force_x + Force_Sheep_x;
				Force_y = Force_y + Force_Sheep_y;
			}
			Force_x = cos(-Angle) * Force_x - sin(-Angle) * Force_y;
   		Force_y = cos(-Angle) * Force_y + sin(-Angle) * Force_x;
			flock[i]->leftSpeed = K1*Force_x + K2*Force_y;
			flock[i]->rightSpeed = K1*Force_x - K2*Force_y;
			if(abs(flock[i]->leftSpeed) > abs(flock[i]->rightSpeed))
			{
				if(abs(flock[i]->leftSpeed) >  SPEED_MAX/2)
				{
					float ratio = abs(flock[i]->leftSpeed)/(SPEED_MAX/2);
					flock[i]->leftSpeed = flock[i]->leftSpeed/ratio;
					flock[i]->rightSpeed = flock[i]->rightSpeed/ratio;
				}
			}
			else if(abs(flock[i]->rightSpeed) > abs(flock[i]->leftSpeed))
			{
				if(abs(flock[i]->rightSpeed) >  SPEED_MAX/2)
				{
					float ratio = abs(flock[i]->rightSpeed)/(SPEED_MAX/2);
					flock[i]->leftSpeed = flock[i]->leftSpeed/ratio;
					flock[i]->rightSpeed = flock[i]->rightSpeed/ratio;
				}
			}

			if(Force_x == 0 && Force_y == 0)
			{
				flock[i]->leftSpeed = (rand()%int(SPEED_MAX)) - SPEED_MAX/2;
				flock[i]->rightSpeed = (rand()%int(SPEED_MAX)) - SPEED_MAX/2;
			}
		}
	}

	~Shepherding()
	{
	}

	void addRobots(World *world, QVector<EPuck*> *V, bool shepherd)
	{
		if(shepherd)
		{
			EPuck *epuck = new EPuck(0x2);
			epuck->camera.init(0.01,world);
			epuck->setColor(Color(0, 1, 0)); // Green for shepherd
			epuck->pos = Point(rand()%300, rand()%300);
			epuck->leftSpeed = 0;
			epuck->rightSpeed = 0;
			V->push_back(epuck);
			world->addObject(epuck);
		}
		else
		{
			EPuck *epuck = new EPuck;
			epuck->setColor(Color(1, 0, 0)); // Red for Sheep
			epuck->pos = Point(rand()%300, rand()%300);
			epuck->leftSpeed = 0;
			epuck->rightSpeed = 0;
			V->push_back(epuck);
			world->addObject(epuck);
		}
	}
};

int main(int argc, char *argv[])
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
	//World world;
	Shepherding viewer(&world);

	viewer.show();
	// Run for some times

	return app.exec();

	//Create World



}
