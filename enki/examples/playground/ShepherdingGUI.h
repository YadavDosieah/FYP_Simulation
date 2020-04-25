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
#include <fstream>

#define SPEED_MAX 12.8

using namespace Enki;
using namespace std;

class ShepherdingGUI: public ViewerWidget
{
	protected:
		QVector<EPuck*> shepherds;
		QVector<EPuck*> flock;
		QVector<PhysicalObject*> objects;
		int mode;
		int noOfSheep;
		int noOfShepherd;
		int noOfObjects;
		int Csheep;
		int Cshepherd;
		float Ksheep;
		float K1;
		float K2;
		float KWall;
		int Goalx;
		int Goaly;
		int dim;
		float GoalRadius;
		const double *y;
		double x[16];
		std::ofstream outputFile;
    // const double (&x)[12];

	public:
	ShepherdingGUI(World *world, int mode, int noOfSheep, int noOfShepherd, int noOfObjects,
								int Csheep, int Cshepherd, float Ksheep, float K1,	float K2,
								float KWall,	int Goalx, int Goaly,float GoalRadius, const double (&y)[16], int dim, QWidget *parent = 0):
								ViewerWidget(world, parent), mode(mode), noOfSheep(noOfSheep),
								noOfShepherd(noOfShepherd), noOfObjects(noOfObjects), Csheep(Csheep), Cshepherd(Cshepherd),
								Ksheep(Ksheep), K1(K1), K2(K2), KWall(KWall), Goalx(Goalx),	Goaly(Goaly),
								GoalRadius(GoalRadius), y(y), dim(dim)
	{
		for(int i=0; i<dim; i++)
		{
			x[i] = (1-exp(-y[i]))/(1+exp(-y[i]));
			cout << x[i] << endl;
		}

		srand(time(0));
		outputFile.open("Output.csv");
		for(int i = 0; i < noOfSheep; i++)
		{
			addRobots(world, &flock, false);
			outputFile << "SheepX,SheepY,";
		}

		for(int i = 0; i < noOfObjects; i++)
		{
			addCylinders(world, &objects);
			outputFile << "CylinderX,CylinderY,";
		}

		for(int i = 0; i < noOfShepherd; i++)
		{
			addRobots(world, &shepherds, true);
			outputFile << "ShepherdX,ShepherdY,";
		}


		outputFile << "\n";

		PhysicalObject* Goal = new PhysicalObject;
		Goal->pos = Point(Goalx,Goaly);
		Goal->setCylindric(GoalRadius, 5, 1000000);
		Goal->dryFrictionCoefficient = 100;
		Goal->setColor(Color(0, 0, 1));
		Goal->collisionElasticity = 0;
		world->addObject(Goal);

	}

	virtual void timerEvent(QTimerEvent * event)
	{
			ViewerWidget::timerEvent(event);
			/*************************************************************************/
			/********************* Shepherd Speed Calculation *************************/
			/*************************************************************************/
			if(mode == 0) //Shepherding
			{
				for(int i = 0; i < noOfShepherd; i++)
				{
					valarray<Color> image = shepherds[i]->camera.image;
					valarray<Color> image2 = shepherds[i]->camera2.image;

					bool sheepDetected = false;
					bool shepherdDetected = false;
					bool goalDetected = false;

					sheepDetected 	= image[0].components[0] == 1 ? 1 : 0;
					shepherdDetected = image[0].components[1] == 1 ? 1 : 0;
					goalDetected 	= image2[0].components[2] == 1 ? 1 : 0;

					if((sheepDetected||shepherdDetected||goalDetected) == false) //No objects seen			STATE 0
					{
						// cout << "State 0\n";
						shepherds[i]->leftSpeed 	= x[0] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[1] * SPEED_MAX;
					}
					else if((sheepDetected&&goalDetected) == true) //Sheep + goal						STATE 4
					{
						// cout << "State 4\n";
						shepherds[i]->leftSpeed 	= x[8] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[9] * SPEED_MAX;
					}
					else if((shepherdDetected&&goalDetected) == true) //Shepherd + goal			STATE 5
					{
						// cout << "State 5\n";
						shepherds[i]->leftSpeed 	= x[10] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[11] * SPEED_MAX;
					}
					else if(goalDetected) //Only goal														STATE 3
					{
						// cout << "State 3\n";
						shepherds[i]->leftSpeed 	= x[6] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[7] * SPEED_MAX;
					}
					else if(sheepDetected)// sheep																STATE 1
					{
						// cout << "State 1\n";
						shepherds[i]->leftSpeed 	= x[2] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[3] * SPEED_MAX;
					}
					else if(shepherdDetected)//shepherd														STATE 2
					{
						// cout << "State 2\n";
						shepherds[i]->leftSpeed 	= x[4] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[5] * SPEED_MAX;
					}
					else
					{
						std::cout << "Error\n";
					}
				}
			}

			else if(mode == 1)//Object Clustering
			{
				for(int i = 0; i < noOfShepherd; i++)
				{
					valarray<Color> image = shepherds[i]->camera.image;
					valarray<Color> image2 = shepherds[i]->camera2.image;
					bool shepherdDetected = false;
					bool goalDetected = false;
					bool objectDetected = false;

					shepherdDetected = image[0].components[1] == 1 ? 1 : 0;
					goalDetected 	= image2[0].components[2] == 1 ? 1 : 0;
					objectDetected = image[0].components[0] == 0.9 ? 1 : 0;

					if((objectDetected||shepherdDetected||goalDetected) == false) //No objects seen			STATE 0
					{
						// cout << "State 0\n";
						shepherds[i]->leftSpeed 	= x[0] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[1] * SPEED_MAX;
					}
					else if((objectDetected&&goalDetected) == true) //Sheep + goal						STATE 4
					{
						// cout << "State 4\n";
						shepherds[i]->leftSpeed 	= x[8] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[9] * SPEED_MAX;
					}
					else if((shepherdDetected&&goalDetected) == true) //Shepherd + goal			STATE 5
					{
						// cout << "State 5\n";
						shepherds[i]->leftSpeed 	= x[10] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[11] * SPEED_MAX;
					}
					else if(goalDetected) //Only goal														STATE 3
					{
						// cout << "State 3\n";
						shepherds[i]->leftSpeed 	= x[6] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[7] * SPEED_MAX;
					}
					else if(objectDetected)// sheep																STATE 1
					{
						// cout << "State 1\n";
						shepherds[i]->leftSpeed 	= x[2] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[3] * SPEED_MAX;
					}
					else if(shepherdDetected)//shepherd														STATE 2
					{
						// cout << "State 2\n";
						shepherds[i]->leftSpeed 	= x[4] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[5] * SPEED_MAX;
					}
					else
					{
						std::cout << "Error\n";
					}
				}
			}

			else if(mode == 2)//Both
			{
				for(int i = 0; i < noOfShepherd; i++)
				{
					valarray<Color> image = shepherds[i]->camera.image;
					valarray<Color> image2 = shepherds[i]->camera2.image;

					bool sheepDetected = false;
					bool shepherdDetected = false;
					bool goalDetected = false;
					bool objectDetected = false;

					sheepDetected 	= image[0].components[0] == 1 ? 1 : 0;
					shepherdDetected = image[0].components[1] == 1 ? 1 : 0;
					goalDetected 	= image2[0].components[2] == 1 ? 1 : 0;
					objectDetected = image[0].components[0] == 0.9 ? 1 : 0;

					if((objectDetected||shepherdDetected||goalDetected||sheepDetected) == false) //No objects seen			STATE 0
					{
						// cout << "State 0\n";
						shepherds[i]->leftSpeed 	= x[0] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[1] * SPEED_MAX;
					}
					else if((objectDetected&&goalDetected) == true) //Object + goal						STATE 5
					{
						// cout << "State 4\n";
						shepherds[i]->leftSpeed 	= x[10] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[11] * SPEED_MAX;
					}
					else if((shepherdDetected&&goalDetected) == true) //Shepherd + goal			STATE 6
					{
						// cout << "State 5\n";
						shepherds[i]->leftSpeed 	= x[12] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[13] * SPEED_MAX;
					}
					else if((sheepDetected&&goalDetected) == true) //Sheep + goal			STATE 7
					{
						// cout << "State 5\n";
						shepherds[i]->leftSpeed 	= x[14] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[15] * SPEED_MAX;
					}
					else if(goalDetected) //Only goal														STATE 4
					{
						// cout << "State 3\n";
						shepherds[i]->leftSpeed 	= x[8] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[9] * SPEED_MAX;
					}
					else if(objectDetected)// object																STATE 1
					{
						// cout << "State 1\n";
						shepherds[i]->leftSpeed 	= x[2] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[3] * SPEED_MAX;
					}
					else if(shepherdDetected)//shepherd														STATE 2
					{
						// cout << "State 2\n";
						shepherds[i]->leftSpeed 	= x[4] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[5] * SPEED_MAX;
					}
					else if(sheepDetected)//sheep														STATE 3
					{
						// cout << "State 2\n";
						shepherds[i]->leftSpeed 	= x[6] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[7] * SPEED_MAX;
					}
					else
					{
						std::cout << "Error\n";
					}
				}
			}


		/*************************************************************************/
		/*********************** Sheep Speed Calculation *************************/
		/*************************************************************************/
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
					if(Distance_sq < 225)
					{
						Force_x = Force_x + (Csheep/Distance_sq)*(Distance_x/sqrt(Distance_sq));
						Force_y = Force_y + (Csheep/Distance_sq)*(Distance_y/sqrt(Distance_sq));
					}
					if(Distance_sq > 625 && Distance_sq < 1225)
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
					if(Distance_sq < 2500)
					{
						ShepherdDetected = true;
						Force_x = Force_x + (Cshepherd/Distance_sq)*(Distance_x/sqrt(Distance_sq));
						Force_y = Force_y + (Cshepherd/Distance_sq)*(Distance_y/sqrt(Distance_sq));
					}
			}
			if (ShepherdDetected != true)
			{
				Force_x = Force_x + Force_Sheep_x;
				Force_y = Force_y + Force_Sheep_y;
			}

			int margin = 15;
			if(flock[i]->pos.x < margin)
			{
				Force_x = Force_x - KWall*(0-flock[i]->pos.x);
			}
			else if(flock[i]->pos.x > (300-margin))
			{
				Force_x = Force_x - KWall*(300-flock[i]->pos.x);
			}

			if(flock[i]->pos.y < margin)
			{
				Force_y = Force_y - KWall*(0-flock[i]->pos.y);
			}
			else if(flock[i]->pos.y > (300-margin))
			{
				Force_y = Force_y - KWall*(300-flock[i]->pos.y);
			}

			double Angle = -flock[i]->angle;
			Force_x = cos(-Angle) * Force_x - sin(-Angle) * Force_y;
   		Force_y = sin(-Angle) * Force_x + cos(-Angle) * Force_y;

			flock[i]->leftSpeed = K1*Force_x + K2*Force_y;
			flock[i]->rightSpeed = K1*Force_x - K2*Force_y;
			flock[i]->leftSpeed = flock[i]->leftSpeed > SPEED_MAX/2 ? SPEED_MAX/2 : flock[i]->leftSpeed;
			flock[i]->rightSpeed = flock[i]->rightSpeed > SPEED_MAX/2 ? SPEED_MAX/2 : flock[i]->rightSpeed;
			flock[i]->leftSpeed = flock[i]->leftSpeed < -SPEED_MAX/2 ? -SPEED_MAX/2 : flock[i]->leftSpeed;
			flock[i]->rightSpeed = flock[i]->rightSpeed < -SPEED_MAX/2 ? -SPEED_MAX/2 : flock[i]->rightSpeed;

			if(Force_x == 0 && Force_y == 0)
			{
				flock[i]->leftSpeed = (fmod(rand(),SPEED_MAX)) - SPEED_MAX/2;
				flock[i]->rightSpeed = (fmod(rand(),SPEED_MAX)) - SPEED_MAX/2;
			}
			outputFile << flock[i]->pos.x << "," << flock[i]->pos.y << "," << flock[i]->angle << ",";
		}

		for(int i=0; i < int(objects.size()); i++)
		{
			outputFile << objects[i]->pos.x << "," << objects[i]->pos.y << ",";
		}

		for(int i=0; i < int(shepherds.size()); i++)
		{
			outputFile << shepherds[i]->pos.x << "," << shepherds[i]->pos.y << "," << shepherds[i]->angle << ",";
		}

		outputFile << "\n";
	}

	~ShepherdingGUI()
	{
		outputFile.close();
	}

	void addCylinders(World *world, QVector<PhysicalObject*> *V)
	{
		PhysicalObject* Obj = new PhysicalObject;
		Obj->pos = Point(rand()%300, rand()%200);;
		Obj->setCylindric(3.7, 4.7, 152);
		Obj->dryFrictionCoefficient = 2.5;
		Obj->setColor(Color(0.9, 0.9, 0));
		Obj->collisionElasticity = 0;
		V->push_back(Obj);
		world->addObject(Obj);
	}

	void addRobots(World *world, QVector<EPuck*> *V, bool shepherd)
	{
		if(shepherd)
		{
			EPuck *epuck = new EPuck(0x2);
			epuck->camera.init(0.01,world);
			epuck->setColor(Color(0, 1, 0)); // Green for shepherd
			epuck->pos = Point(rand()%300, rand()%200);
			epuck->angle = fmod(rand(),(2*M_PI)) - M_PI;
			V->push_back(epuck);
			world->addObject(epuck);
		}
		else
		{
			EPuck *epuck = new EPuck;
			epuck->setColor(Color(1, 0, 0)); // Red for Sheep
			epuck->pos = Point(rand()%300, rand()%200);
			epuck->angle = fmod(rand(),(2*M_PI)) - M_PI;
			V->push_back(epuck);
			world->addObject(epuck);
		}
	}
};
