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
		QVector<PhysicalObject*> objects;
		vector<int> noOfShepherd;
		int noOfObjects;
		int NoOfGroups;
		int Goalx;
		int Goaly;
		int Xbound;
		int Ybound;
		int dim;
		float GoalRadius;
		float GoalDistance;
    const double *y;
		double x[24];
		double fitness_val = 0;
		int TimeStep = 0;
		std::ofstream outputFile;
		bool TargetReached = false;

	public:
	Shepherding(World *world,vector<int> noOfShepherd, int noOfObjects,
							int Goalx, int Goaly, float GoalRadius, float GoalDistance,
							const double *y, int dim):
							noOfShepherd(noOfShepherd), noOfObjects(noOfObjects),
							Goalx(Goalx), Goaly(Goaly), GoalRadius(GoalRadius),
							GoalDistance(pow(GoalDistance,2)),y(y), dim(dim)
	{

		Xbound = int(world->w);
		Ybound = int(world->h);

		NoOfGroups = noOfShepherd.size();
		x[NoOfGroups*12] = {0};

		// cout << sizeof(y)/sizeof(*y) << endl;
		for(int i=0; i<dim; i++)
		{
			x[i] = (1-exp(-y[i]))/(1+exp(-y[i]));
			//cout << x[i] << endl;
		}
		srand(time(0));

		// cout << NoOfGroups;
		for(int i = 0; i < NoOfGroups; i++)
		{
			// cout << noOfShepherd[i] << endl;
			for(int j=0; j< noOfShepherd[i]; j++)
			{
				addRobots(world, &shepherds);
			}
		}

		for(int i = 0; i < noOfObjects; i++)
		{
			addCylinders(world, &objects);
		}

		PhysicalObject* Goal = new PhysicalObject;
		Goal->pos = Point(Goalx,Goaly);
		Goal->setCylindric(GoalRadius, 5, 1000000);
		Goal->dryFrictionCoefficient = 100;
		Goal->setColor(Color(0, 0, 1));
		Goal->collisionElasticity = 0;
		world->addObject(Goal);
	}

	int doPerTick()
	{
		TimeStep++;

		/*************************************************************************/
		/********************* Shepherd Speed Calculation *************************/
		/*************************************************************************/
		CalculateFitness();
		int noOfVel = 12;
		int CumulativeNumberOfShepherds = 0;
		for (int j=0; j< NoOfGroups; j++)
		{
			for(int i = 0; i < noOfShepherd[i]; i++)
			{
				valarray<Color> image = shepherds[i+CumulativeNumberOfShepherds]->camera.image;
				valarray<Color> image2 = shepherds[i+CumulativeNumberOfShepherds]->camera2.image;
				bool shepherdDetected = false;
				bool goalDetected = false;
				bool objectDetected = false;

				shepherdDetected = image[0].components[1] == 1 ? 1 : 0;
				goalDetected 	= image2[0].components[2] == 1 ? 1 : 0;
				objectDetected = image[0].components[0] == 0.9 ? 1 : 0;

				if((objectDetected||shepherdDetected||goalDetected) == false) //No objects seen			STATE 0
				{
					// cout << "State 0\n";
					shepherds[i+CumulativeNumberOfShepherds]->leftSpeed 	= x[0+(j*noOfVel)] * SPEED_MAX;
					shepherds[i+CumulativeNumberOfShepherds]->rightSpeed	= x[1+(j*noOfVel)] * SPEED_MAX;
				}
				else if((objectDetected&&goalDetected) == true) //Sheep + goal						STATE 4
				{
					// cout << "State 4\n";
					shepherds[i+CumulativeNumberOfShepherds]->leftSpeed 	= x[8+(j*noOfVel)] * SPEED_MAX;
					shepherds[i+CumulativeNumberOfShepherds]->rightSpeed	= x[9+(j*noOfVel)] * SPEED_MAX;
				}
				else if((shepherdDetected&&goalDetected) == true) //Shepherd + goal			STATE 5
				{
					// cout << "State 5\n";
					shepherds[i+CumulativeNumberOfShepherds]->leftSpeed 	= x[10+(j*noOfVel)] * SPEED_MAX;
					shepherds[i+CumulativeNumberOfShepherds]->rightSpeed	= x[11+(j*noOfVel)] * SPEED_MAX;
				}
				else if(goalDetected) //Only goal														STATE 3
				{
					// cout << "State 3\n";
					shepherds[i+CumulativeNumberOfShepherds]->leftSpeed 	= x[6+(j*noOfVel)] * SPEED_MAX;
					shepherds[i+CumulativeNumberOfShepherds]->rightSpeed	= x[7+(j*noOfVel)] * SPEED_MAX;
				}
				else if(objectDetected)// sheep																STATE 1
				{
					// cout << "State 1\n";
					shepherds[i+CumulativeNumberOfShepherds]->leftSpeed 	= x[2+(j*noOfVel)] * SPEED_MAX;
					shepherds[i+CumulativeNumberOfShepherds]->rightSpeed	= x[3+(j*noOfVel)] * SPEED_MAX;
				}
				else if(shepherdDetected)//shepherd														STATE 2
				{
					// cout << "State 2\n";
					shepherds[i+CumulativeNumberOfShepherds]->leftSpeed 	= x[4+(j*noOfVel)] * SPEED_MAX;
					shepherds[i+CumulativeNumberOfShepherds]->rightSpeed	= x[5+(j*noOfVel)] * SPEED_MAX;
				}
				else
				{
					std::cout << "Error\n";
				}
			}

			CumulativeNumberOfShepherds += noOfShepherd[j];
		}
		return isnan(fitness_val);
	}

	void CalculateFitness()
	{
		for(int i = 0; i < noOfObjects; i++)
		{
			double Distance_x = objects[i]->pos.x - Goalx;
			double Distance_y = objects[i]->pos.y - Goaly;

			double Distances_sq = pow(Distance_x,2) + pow(Distance_y,2);
			fitness_val = fitness_val + (Distances_sq/(4*noOfObjects*3.7*3.7))*TimeStep*0.1;
		}
	}

	float getSuccessRate()
	{
		double Distance_sq = 0;
		float counter = 0;
		for(int i = 0; i < noOfObjects; i++)
		{
			Distance_sq = pow(Goalx - objects[i]->pos.x,2) + pow(Goaly - objects[i]->pos.y,2);
			if(Distance_sq < GoalDistance)
			{
				counter++;
			}
		}

		return counter/(noOfObjects);
	}

	void printHeaders()
	{
		outputFile.open("Output.csv");

		for(int i = 0; i < noOfObjects; i++)
		{
			outputFile << "CylinderX,CylinderY,";
		}

		for(int i = 0; i < NoOfGroups; i++)
		{
			for(int j=0; j< noOfShepherd[i]; j++)
			{
				outputFile << "ShepherdX,ShepherdY,Angle,";
			}
		}

		outputFile << "\n";
	}

	void printPositions()
	{

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

	bool GoalReached()
	{
		return TargetReached;
	}

	double getTotalFitness()
	{
		return fitness_val;
	}

	~Shepherding()
	{
		if(outputFile.is_open())
		{
			outputFile.close();
		}
	}

	void addCylinders(World *world, QVector<PhysicalObject*> *V)
	{
		PhysicalObject* Obj = new PhysicalObject;
		Obj->pos = Point(rand()%Xbound, rand()%(Goaly-50));;
		Obj->setCylindric(3.7, 4.7, 152);
		Obj->dryFrictionCoefficient = 2.5;
		Obj->setColor(Color(0.9, 0.9, 0));
		Obj->collisionElasticity = 0;
		V->push_back(Obj);
		world->addObject(Obj);
	}

	void addRobots(World *world, QVector<EPuck*> *V)
	{
			EPuck *epuck = new EPuck(0x2);
			epuck->camera.init(0.01,world);
			epuck->setColor(Color(0, 1, 0)); // Green for shepherd
			epuck->pos = Point(RandomFloat(3.7,Xbound-3.7),RandomFloat(3.7,Goaly-50));
			epuck->angle = fmod(rand(),(2*M_PI)) - M_PI;
			V->push_back(epuck);
			world->addObject(epuck);
	}

	float RandomFloat(float a, float b)//Generate float between a and b
	{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
	}
};
