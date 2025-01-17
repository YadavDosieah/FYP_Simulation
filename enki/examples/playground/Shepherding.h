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
		int Xbound;
		int Ybound;
		int dim;
		float GoalRadius;
		float GoalDistance;
		float Sheep_MaxSpeed_Multiplier;
    const double *y;
		double x[32];
		double fitness_val = 0;
		int TimeStep = 0;
		std::ofstream outputFile;
		bool TargetReached = false;

	public:
	Shepherding(World *world, int mode, int noOfSheep, int noOfShepherd, int noOfObjects,
							int Csheep, int Cshepherd, float Ksheep, float K1,	float K2,
							float KWall, int Goalx, int Goaly, float GoalRadius, float GoalDistance,
							float Sheep_MaxSpeed_Multiplier, const double *y, int dim):
							mode(mode), noOfSheep(noOfSheep), noOfShepherd(noOfShepherd),
							noOfObjects(noOfObjects), Csheep(Csheep), Cshepherd(Cshepherd),
							Ksheep(Ksheep), K1(K1), K2(K2),KWall(KWall), Goalx(Goalx), Goaly(Goaly),
							GoalRadius(GoalRadius), GoalDistance(pow(GoalDistance,2)),
							Sheep_MaxSpeed_Multiplier(Sheep_MaxSpeed_Multiplier),	y(y), dim(dim)
	{
		Xbound = int(world->w);
		Ybound = int(world->h);

		//cout << sizeof(y)/sizeof(*y) << endl;
		for(int i=0; i<dim; i++)
		{
			x[i] = (1-exp(-y[i]))/(1+exp(-y[i]));
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
		if(mode == 0) //Shepherding
		{
			CalculateFitness();

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
				sheepDetected = objectDetected || sheepDetected;

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
			CalculateFitness();

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
				objectDetected = objectDetected || sheepDetected;

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
			CalculateFitness();
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
		else if(mode == 3)
		{
			CalculateFitness();
			for(int i = 0; i < noOfShepherd; i++)
			{
				valarray<Color> image = shepherds[i]->camera.image;
				valarray<Color> image2 = shepherds[i]->camera2.image;

				bool sheepDetected = false;
				//bool shepherdDetected = false;
				bool goalDetected = false;
				bool objectDetected = false;

				sheepDetected 	= image[0].components[0] == 1 ? 1 : 0;
				//shepherdDetected = image[0].components[1] == 1 ? 1 : 0;
				goalDetected 	= image2[0].components[2] == 1 ? 1 : 0;
				objectDetected = image[0].components[0] == 0.9 ? 1 : 0;

				objectDetected = objectDetected || sheepDetected;

				if((objectDetected||goalDetected) == false) //No objects seen			STATE 0
				{
					// cout << "State 0\n";
					shepherds[i]->leftSpeed 	= x[0] * SPEED_MAX;
					shepherds[i]->rightSpeed	= x[1] * SPEED_MAX;
				}
				else if((objectDetected&&goalDetected) == true) //Object + goal						STATE 5
				{
					// cout << "State 4\n";
					shepherds[i]->leftSpeed 	= x[6] * SPEED_MAX;
					shepherds[i]->rightSpeed	= x[7] * SPEED_MAX;
				}
				else if(goalDetected) //Only goal														STATE 4
				{
					// cout << "State 3\n";
					shepherds[i]->leftSpeed 	= x[4] * SPEED_MAX;
					shepherds[i]->rightSpeed	= x[5] * SPEED_MAX;
				}
				else if(objectDetected)// object																STATE 1
				{
					// cout << "State 1\n";
					shepherds[i]->leftSpeed 	= x[2] * SPEED_MAX;
					shepherds[i]->rightSpeed	= x[3] * SPEED_MAX;
				}
				else
				{
					std::cout << "Error\n";
				}
			}
		}

		else if(mode == 6)//Heterogenous shepherds
		{
			CalculateFitness();
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
				if(i < noOfShepherd/2)
				{
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
				else
				{
					if((objectDetected||shepherdDetected||goalDetected||sheepDetected) == false) //No objects seen			STATE 0
					{
						// cout << "State 0\n";
						shepherds[i]->leftSpeed 	= x[16] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[17] * SPEED_MAX;
					}
					else if((objectDetected&&goalDetected) == true) //Object + goal						STATE 5
					{
						// cout << "State 4\n";
						shepherds[i]->leftSpeed 	= x[26] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[27] * SPEED_MAX;
					}
					else if((shepherdDetected&&goalDetected) == true) //Shepherd + goal			STATE 6
					{
						// cout << "State 5\n";
						shepherds[i]->leftSpeed 	= x[28] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[29] * SPEED_MAX;
					}
					else if((sheepDetected&&goalDetected) == true) //Sheep + goal			STATE 7
					{
						// cout << "State 5\n";
						shepherds[i]->leftSpeed 	= x[30] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[31] * SPEED_MAX;
					}
					else if(goalDetected) //Only goal														STATE 4
					{
						// cout << "State 3\n";
						shepherds[i]->leftSpeed 	= x[24] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[25] * SPEED_MAX;
					}
					else if(objectDetected)// object																STATE 1
					{
						// cout << "State 1\n";
						shepherds[i]->leftSpeed 	= x[18] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[19] * SPEED_MAX;
					}
					else if(shepherdDetected)//shepherd														STATE 2
					{
						// cout << "State 2\n";
						shepherds[i]->leftSpeed 	= x[20] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[21] * SPEED_MAX;
					}
					else if(sheepDetected)//sheep														STATE 3
					{
						// cout << "State 2\n";
						shepherds[i]->leftSpeed 	= x[22] * SPEED_MAX;
						shepherds[i]->rightSpeed	= x[23] * SPEED_MAX;
					}
					else
					{
						std::cout << "Error\n";
					}
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
					double Distance = sqrt(Distance_sq);
					double X_Comp = Distance_x/Distance;
					double Y_Comp = Distance_y/Distance;
					if(Distance < 10)
					{
						Force_x = Force_x + (Csheep/Distance_sq)*X_Comp;
						Force_y = Force_y + (Csheep/Distance_sq)*Y_Comp;
					}
					else if(Distance > 25 && Distance < 35)
					{
						Force_x = Force_x - Ksheep*Distance/35*X_Comp;
						Force_y = Force_y - Ksheep*Distance/35*Y_Comp;
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
					double Distance = sqrt(Distance_sq);
					if(Distance < 50)
					{
						ShepherdDetected = true;
						Force_x = Force_x + (Cshepherd/Distance_sq)*(Distance_x/Distance);
						Force_y = Force_y + (Cshepherd/Distance_sq)*(Distance_y/Distance);
					}
			}
			if(mode == 1 || mode == 3)
			{
				Force_x = Force_x + Force_Sheep_x;
				Force_y = Force_y + Force_Sheep_y;
			}
			else
			{
				if (ShepherdDetected != true)
				{
					Force_x = Force_x + Force_Sheep_x;
					Force_y = Force_y + Force_Sheep_y;
				}
			}


			int margin = 25;
			if(flock[i]->pos.x < margin)
			{
				Force_x = Force_x + KWall/pow((flock[i]->pos.x),2);
			}
			else if(flock[i]->pos.x > (Xbound-margin))
			{
				Force_x = Force_x - KWall/pow((Xbound-flock[i]->pos.x),2);
			}

			if(flock[i]->pos.y < margin)
			{
				Force_y = Force_y + KWall/pow((flock[i]->pos.y),2);
			}
			else if(flock[i]->pos.y > (Ybound-margin))
			{
				Force_y = Force_y - KWall/pow((Ybound-flock[i]->pos.y),2);
			}

			double Angle = -flock[i]->angle;
			float cs = cos(Angle);
			float sn = sin(Angle);
			float Robot_Force_x = cs*Force_x - sn*Force_y;
   		float Robot_Force_y = sn*Force_x + cs*Force_y;

			flock[i]->leftSpeed = K1*Robot_Force_x + K2*Robot_Force_y;
			flock[i]->rightSpeed = K1*Robot_Force_x - K2*Robot_Force_y;

			flock[i]->leftSpeed = flock[i]->leftSpeed > SPEED_MAX*Sheep_MaxSpeed_Multiplier ? SPEED_MAX*Sheep_MaxSpeed_Multiplier  : flock[i]->leftSpeed;
			flock[i]->rightSpeed = flock[i]->rightSpeed > SPEED_MAX*Sheep_MaxSpeed_Multiplier  ? SPEED_MAX*Sheep_MaxSpeed_Multiplier  : flock[i]->rightSpeed;
			flock[i]->leftSpeed = flock[i]->leftSpeed < -SPEED_MAX*Sheep_MaxSpeed_Multiplier  ? -SPEED_MAX*Sheep_MaxSpeed_Multiplier  : flock[i]->leftSpeed;
			flock[i]->rightSpeed = flock[i]->rightSpeed < -SPEED_MAX*Sheep_MaxSpeed_Multiplier  ? -SPEED_MAX*Sheep_MaxSpeed_Multiplier  : flock[i]->rightSpeed;

			if(Force_x == 0 && Force_y == 0)
			{
				flock[i]->leftSpeed = RandomFloat(-SPEED_MAX*Sheep_MaxSpeed_Multiplier,SPEED_MAX*Sheep_MaxSpeed_Multiplier);
				flock[i]->rightSpeed = RandomFloat(-SPEED_MAX*Sheep_MaxSpeed_Multiplier,SPEED_MAX*Sheep_MaxSpeed_Multiplier);
			}
		}
		return isnan(fitness_val);
	}

	void CalculateFitness()
	{
		double centroid_x = 0;
		double centroid_y = 0;
		for(int i = 0; i < noOfObjects; i++)
		{
			centroid_x = centroid_x + objects[i]->pos.x;
			centroid_y = centroid_y + objects[i]->pos.y;
		}
		centroid_x = centroid_x/noOfObjects;
		centroid_y = centroid_y/noOfObjects;
		double Distance_x2 = Goalx - centroid_x;
		double Distance_y2 = Goaly - centroid_y;
		double Distance_sq2 = (pow(Distance_x2,2) + pow(Distance_y2,2));
		// bool ObjectsReached = Distance_sq2 < 2025? true : false;
		// ObjectsReached = noOfObjects > 0 ? ObjectsReached : true;

		for(int i = 0; i < noOfObjects; i++)
		{
			double Distance_x1 = centroid_x - objects[i]->pos.x;
			double Distance_y1 = centroid_y - objects[i]->pos.y;
			double Distance_x2 = Goalx - centroid_x;
			double Distance_y2 = Goaly - centroid_y;

			double Distances_sq = (pow(Distance_x1,2) + pow(Distance_y1,2))*Distance_sq2;
			fitness_val = fitness_val + (Distances_sq/(4*noOfObjects*3.7*3.7))*TimeStep*0.1;
		}

		centroid_x = 0;
		centroid_y = 0;
		for(int i = 0; i < noOfSheep; i++)
		{
			centroid_x = centroid_x + flock[i]->pos.x;
			centroid_y = centroid_y + flock[i]->pos.y;
		}
		centroid_x = centroid_x/noOfSheep;
		centroid_y = centroid_y/noOfSheep;
		Distance_x2 = Goalx - centroid_x;
		Distance_y2 = Goaly - centroid_y;
		Distance_sq2 = (pow(Distance_x2,2) + pow(Distance_y2,2));
		// bool SheepReached = (Distance_sq2 < 2025 ? true : false);
		// SheepReached = noOfSheep > 0 ? SheepReached : true;
		// TargetReached = SheepReached && ObjectsReached;

		for(int i = 0; i < noOfSheep; i++)
		{
			double Distance_x1 = centroid_x - flock[i]->pos.x;
			double Distance_y1 = centroid_y - flock[i]->pos.y;

			double Distances_sq = (pow(Distance_x1,2) + pow(Distance_y1,2))*Distance_sq2;
			fitness_val = fitness_val + (Distances_sq/(4*noOfSheep*3.7*3.7))*TimeStep*0.1;
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

		for(int i = 0; i < noOfSheep; i++)
		{
			Distance_sq = pow(Goalx - flock[i]->pos.x,2) + pow(Goaly - flock[i]->pos.y,2);
			if(Distance_sq < GoalDistance)
			{
				counter++;
			}
		}

		return counter/(noOfObjects+noOfSheep);
	}

	void printHeaders()
	{
		outputFile.open("Output.csv");
		for(int i = 0; i < noOfSheep; i++)
		{
			outputFile << "SheepX,SheepY,Angle,";
		}

		for(int i = 0; i < noOfObjects; i++)
		{
			outputFile << "CylinderX,CylinderY,";
		}

		for(int i = 0; i < noOfShepherd; i++)
		{
			outputFile << "ShepherdX,ShepherdY,Angle,";
		}

		outputFile << "\n";
	}

	void printPositions()
	{
		for(int i=0; i < int(flock.size()); i++)
		{
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

	void addRobots(World *world, QVector<EPuck*> *V, bool shepherd)
	{
		if(shepherd)
		{
			EPuck *epuck = new EPuck(0x2);
			epuck->camera.init(0.01,world);
			epuck->setColor(Color(0, 1, 0)); // Green for shepherd
			epuck->pos = Point(RandomFloat(3.7,Xbound-3.7),RandomFloat(3.7,Goaly-50));
			epuck->angle = fmod(rand(),(2*M_PI)) - M_PI;
			V->push_back(epuck);
			world->addObject(epuck);
		}
		else
		{
			EPuck *epuck = new EPuck(0);
			epuck->setColor(Color(1, 0, 0)); // Red for Sheep
			epuck->pos = Point(RandomFloat(3.7,Xbound-3.7),RandomFloat(3.7,Goaly-50));
			epuck->angle = fmod(rand(),(2*M_PI)) - M_PI;
			V->push_back(epuck);
			world->addObject(epuck);
		}
	}

	float RandomFloat(float a, float b)//Generate float between a and b
	{
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
	}
};
