#include <viewer/Viewer.h>
#include <enki/PhysicalEngine.h>
#include <enki/robots/e-puck/EPuck.h>
#include <iostream>
#include "stdio.h"
#include <cstdlib>
#include "time.h"
#include "math.h"

#define SPEED_MAX 12.8

using namespace Enki;
using namespace std;

class Shepherding
{
	protected:
		QVector<EPuck*> shepherds;
		QVector<EPuck*> flock;
		int noOfSheep;
		int noOfShepherd;
		int Csheep;
		int Cshepherd;
		float Ksheep;
		float K1;
		float K2;
		float KWall;
		int Goalx;
		int Goaly;
    const double *y;
		double x[12];
		double fitness_val = 0;
		int TimeStep = 0;
		int multiplier = 1;

	public:
	Shepherding(World *world, int noOfSheep, int noOfShepherd,	int Csheep,
								int Cshepherd, float Ksheep, float K1,	float K2, float KWall,
								int Goalx, int Goaly, const double *y): noOfSheep(noOfSheep),
								noOfShepherd(noOfShepherd), Csheep(Csheep), Cshepherd(Cshepherd),
								Ksheep(Ksheep), K1(K1), K2(K2),KWall(KWall), Goalx(Goalx),	Goaly(Goaly), y(y)
	{
		for(int i=0; i<12; i++)
		{
			x[i] = (1-exp(y[i]))/(1+exp(y[i]));
			//cout << x[i] << endl;
		}
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
		Goal->setCylindric(25, 5, 1000000);
		Goal->dryFrictionCoefficient = 100;
		Goal->setColor(Color(0, 0, 1));
		Goal->collisionElasticity = 0;
		world->addObject(Goal);
	}

	virtual void doPerTick()
	{
		TimeStep++;
		double centroid_x = 0;
		double centroid_y = 0;
		for(int i = 0; i < noOfSheep; i++)
		{
			centroid_x = centroid_x + flock[i]->pos.x;
			centroid_y = centroid_y + flock[i]->pos.y;
		}
		centroid_x = centroid_x/noOfSheep;
		centroid_y = centroid_y/noOfSheep;

		for(int i = 0; i < noOfSheep; i++)
		{
			double Distance_x1 = centroid_x - flock[i]->pos.x;
			double Distance_y1 = centroid_y - flock[i]->pos.y;
			double Distance_x2 = Goalx - centroid_x;
			double Distance_y2 = Goaly - centroid_y;
			Distance_x2 = Distance_x2 < 50? Distance_x2 : 0;
			Distance_y2 = Distance_y2 < 50? Distance_y2 : 0;

			if(TimeStep > 500)
			{
				multiplier = 3;
			}
			if(TimeStep > 1000)
			{
				multiplier = 5;
			}
			double Distances_sq = (pow(Distance_x1,2) + pow(Distance_y1,2))*(pow(Distance_x2,2) + pow(Distance_y2,2));
			fitness_val = fitness_val + (Distances_sq/(4*noOfSheep*3.7*3.7))*multiplier;
		}
		for(int i = 0; i < noOfShepherd; i++)
		{
			valarray<Color> image = shepherds[i]->camera.image;
			valarray<Color> image2 = shepherds[i]->camera2.image;

			bool red = false;
			bool green = false;
			bool blue = false;
			for (size_t i = 0; i < image.size(); i++)
			{
				//std::cout << image[i] << std::endl;
					red 	= red 	|| (image[i].components[0] == 1 ? 1 : 0);
					green = green || (image[i].components[1] == 1 ? 1 : 0);
					blue 	= blue 	|| (image2[i].components[2] == 1 ? 1 : 0);
			}
			// std::cout << "Colour Observed - (r:" << red << ",g:" << green << ",b:" <<
			// blue << ")\n";
			if((red||blue||green) == false) //No objects seen			STATE 0
			{
        // cout << "State 0\n";
        shepherds[i]->leftSpeed 	= x[0] * SPEED_MAX;
				shepherds[i]->rightSpeed	= x[1] * SPEED_MAX;
			}
			else if((red&&blue) == true) //Sheep + goal						STATE 4
			{
        // cout << "State 4\n";
        shepherds[i]->leftSpeed 	= x[8] * SPEED_MAX;
				shepherds[i]->rightSpeed	= x[9] * SPEED_MAX;
			}
			else if((green&&blue) == true) //Shepherd + goal			STATE 5
			{
        // cout << "State 5\n";
        shepherds[i]->leftSpeed 	= x[10] * SPEED_MAX;
				shepherds[i]->rightSpeed	= x[11] * SPEED_MAX;
			}
			else if(blue) //Only goal														STATE 3
			{
        // cout << "State 3\n";
        shepherds[i]->leftSpeed 	= x[6] * SPEED_MAX;
				shepherds[i]->rightSpeed	= x[7] * SPEED_MAX;
			}
			else if(red)// sheep																STATE 1
			{
        // cout << "State 1\n";
        shepherds[i]->leftSpeed 	= x[2] * SPEED_MAX;
				shepherds[i]->rightSpeed	= x[3] * SPEED_MAX;
			}
			else if(green)//shepherd														STATE 2
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
						Force_x = Force_x + (Cshepherd/Distance_sq)*(Distance_x/sqrt(Distance_sq));
						Force_y = Force_y + (Cshepherd/Distance_sq)*(Distance_y/sqrt(Distance_sq));
					}
			}
			double Angle = flock[i]->angle;
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

			Force_x = cos(-Angle) * Force_x - sin(-Angle) * Force_y;
   		Force_y = cos(-Angle) * Force_y + sin(-Angle) * Force_x;
			flock[i]->leftSpeed = K1*Force_x + K2*Force_y;
			flock[i]->rightSpeed = K1*Force_x - K2*Force_y;

			flock[i]->leftSpeed = flock[i]->leftSpeed > SPEED_MAX/2 ? SPEED_MAX/2 : flock[i]->leftSpeed;
			flock[i]->rightSpeed = flock[i]->rightSpeed > SPEED_MAX/2 ? SPEED_MAX/2 : flock[i]->rightSpeed;
			flock[i]->leftSpeed = flock[i]->leftSpeed < -SPEED_MAX/2 ? -SPEED_MAX/2 : flock[i]->leftSpeed;
			flock[i]->rightSpeed = flock[i]->rightSpeed < -SPEED_MAX/2 ? -SPEED_MAX/2 : flock[i]->rightSpeed;

			// if(abs(flock[i]->leftSpeed) > abs(flock[i]->rightSpeed))
			// {
			// 	if(abs(flock[i]->leftSpeed) >  SPEED_MAX/2)
			// 	{
			// 		float ratio = abs(flock[i]->leftSpeed)/(SPEED_MAX/2);
			// 		flock[i]->leftSpeed = flock[i]->leftSpeed/ratio;
			// 		flock[i]->rightSpeed = flock[i]->rightSpeed/ratio;
			// 	}
			// }
			// else if(abs(flock[i]->rightSpeed) > abs(flock[i]->leftSpeed))
			// {
			// 	if(abs(flock[i]->rightSpeed) >  SPEED_MAX/2)
			// 	{
			// 		float ratio = abs(flock[i]->rightSpeed)/(SPEED_MAX/2);
			// 		flock[i]->leftSpeed = flock[i]->leftSpeed/ratio;
			// 		flock[i]->rightSpeed = flock[i]->rightSpeed/ratio;
			// 	}
			// }

			if(Force_x == 0 && Force_y == 0)
			{
				flock[i]->leftSpeed = (rand()%int(SPEED_MAX)) - SPEED_MAX/2;
				flock[i]->rightSpeed = (rand()%int(SPEED_MAX)) - SPEED_MAX/2;
			}
		}
	}

	double getFitness()
	{
		return fitness_val;
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
			epuck->pos = Point(rand()%300, rand()%200);
			epuck->leftSpeed = 0;
			epuck->rightSpeed = 0;
			V->push_back(epuck);
			world->addObject(epuck);
		}
		else
		{
			EPuck *epuck = new EPuck;
			epuck->setColor(Color(1, 0, 0)); // Red for Sheep
			epuck->pos = Point(rand()%300, rand()%200);
			epuck->leftSpeed = 0;
			epuck->rightSpeed = 0;
			V->push_back(epuck);
			world->addObject(epuck);
		}
	}
};
