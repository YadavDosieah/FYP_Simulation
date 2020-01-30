/*
    Enki - a fast 2D robot simulator
    Copyright (C) 1999-2016 Stephane Magnenat <stephane at magnenat dot net>
    Copyright (C) 2004-2005 Markus Waibel <markus dot waibel at epfl dot ch>
    Copyright (c) 2004-2005 Antoine Beyeler <abeyeler at ab-ware dot com>
    Copyright (C) 2005-2006 Laboratory of Intelligent Systems, EPFL, Lausanne
    Copyright (C) 2006-2008 Laboratory of Robotics Systems, EPFL, Lausanne
    See AUTHORS for details

    This program is free software; the authors of any publication
    arising from research using this software are asked to add the
    following reference:
    Enki - a fast 2D robot simulator
    http://home.gna.org/enki
    Stephane Magnenat <stephane at magnenat dot net>,
    Markus Waibel <markus dot waibel at epfl dot ch>
    Laboratory of Intelligent Systems, EPFL, Lausanne.

    You can redistribute this program and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

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

#define noOfSheep 10
#define Csheep 100
#define Csheepdog 5000
#define Ksheep 0.5
#define K1 2
#define K2 1.3

#ifdef USE_SDL
#include </usr/include/SDL2/SDL.h>
#endif

/*!	\file Studio.cpp
	\brief Test of the Qt-based viewer widget
*/

using namespace Enki;
using namespace std;

class EnkiPlayground : public ViewerWidget
{
protected:
	#ifdef USE_SDL
	QVector<SDL_Joystick *> joysticks;
	#endif
	QVector<EPuck*> epucks;
	QVector<EPuck*> sheep;
	QMap<PhysicalObject*, int> bullets;

public:
	EnkiPlayground(World *world, QWidget *parent = 0) :
		ViewerWidget(world, parent)
	{
		#ifdef PROBLEM_LONE_EPUCK
		addDefaultsRobots(world);
		#endif // PROBLEM_LONE_EPUCK

		#ifdef USE_SDL

		srand(time(0));
		for(int i = 0; i < noOfSheep; i++)
		{
			addSheepRobots(world, &sheep);
		}

		if((SDL_Init(SDL_INIT_JOYSTICK)==-1))
		{
			cerr << "Error : Could not initialize SDL: " << SDL_GetError() << endl;
			addDefaultsRobots(world);
			return;
		}

		int joystickCount = SDL_NumJoysticks();
		std::cout << "Number=" << joystickCount << '\n';
		for (int i = 0; i < joystickCount; ++i)
		{
			SDL_Joystick* joystick = SDL_JoystickOpen(i);
			if (!joystick)
			{
				cerr << "Error: Can't open joystick " << i << endl;
				continue;
			}
			if (SDL_JoystickNumAxes(joystick) < 2)
			{
				cerr << "Error: not enough axis on joystick" << i<< endl;
				SDL_JoystickClose(joystick);
				continue;
			}
			joysticks.push_back(joystick);

			EPuck *epuck = new EPuck;
			//epuck->pos = Point(UniformRand(20, 100)(), UniformRand(20, 100)());
			epuck->pos = Point(20, 20);
			epucks.push_back(epuck);
			world->addObject(epuck);
		}
		cout << "Added " << joystickCount << " controlled e-pucks." << endl;
		#else // USE_SDL
		addDefaultsRobots(world);
		#endif // USE_SDL
	}

	void addDefaultsRobots(World *world)
	{
		EPuck *epuck = new EPuck;
		epuck->pos = Point(60, 50);
		//epuck->leftSpeed = 5;
		//epuck->rightSpeed = 5;
		world->addObject(epuck);

		epuck = new EPuck;
		epuck->pos = Point(40, 50);
		epuck->leftSpeed = 5;
		epuck->rightSpeed = 5;
		epuck->setColor(Color(1, 0, 0));
		//world->addObject(epuck);
	}

	void addSheepRobots(World *world, QVector<EPuck*> *sheep)
	{
		EPuck *epuck = new EPuck;
		epuck->pos = Point(rand()%300, rand()%300);
		epuck->leftSpeed = 5;
		epuck->rightSpeed = 5;
		epuck->setColor(Color(1, 0, 0));
		sheep->push_back(epuck);
		world->addObject(epuck);
	}

	~EnkiPlayground()
	{
		#ifdef USE_SDL
		for (int i = 0; i < joysticks.size(); ++i)
			SDL_JoystickClose(joysticks[i]);
		SDL_Quit();
		#endif
	}

	virtual void timerEvent(QTimerEvent * event)
	{
		static int fireCounter = 0;
		#ifdef USE_SDL
		SDL_JoystickUpdate();
		doDumpFrames = false;
		for (int i = 0; i < joysticks.size(); ++i)
		{
			EPuck* epuck = epucks[i];

			if (world->hasGroundTexture())
				cout << "Robot " << i << " is on ground of colour " << world->getGroundColor(epuck->pos) << endl;

			#define SPEED_MAX 13.
			//cout << "S " << epuck->infraredSensor2.getRayDist(0) << " " << epuck->infraredSensor2.getRayDist(1) << " " << epuck->infraredSensor2.getRayDist(2) << endl;
			#if 0
			epuck->leftSpeed = - SDL_JoystickGetAxis(joysticks[i], 1) / (32767. / SPEED_MAX);
			epuck->rightSpeed = - SDL_JoystickGetAxis(joysticks[i], 4) / (32767. / SPEED_MAX);
			#else
			double x = SDL_JoystickGetAxis(joysticks[i], 0) / (32767. / SPEED_MAX);
			double y = -SDL_JoystickGetAxis(joysticks[i], 1) / (32767. / SPEED_MAX);
			epuck->leftSpeed = y + x;
			epuck->rightSpeed = y - x;
			#endif

			if ((SDL_JoystickGetButton(joysticks[i], 6) || SDL_JoystickGetButton(joysticks[i], 7)) &&
				(++fireCounter % 2) == 0)
			{
				PhysicalObject* o = new PhysicalObject;
				Vector delta(cos(epuck->angle), sin(epuck->angle));
				o->pos = epuck->pos + delta * 6;
				o->speed = epuck->speed + delta * 10;
				o->setCylindric(0.5, 0.5, 10);
				o->dryFrictionCoefficient = 0.01;
				o->setColor(Color(0.4, 0, 0));
				o->collisionElasticity = 1;
				bullets[o] = 300;
				world->addObject(o);
			}
			doDumpFrames |= SDL_JoystickGetButton(joysticks[i], 0);
		}
		#endif
		QMap<PhysicalObject*, int>::iterator i = bullets.begin();
		while (i != bullets.end())
		{
			QMap<PhysicalObject*, int>::iterator oi = i;
			++i;
			if (oi.value())
			{
				oi.value()--;
			}
			else
			{
				PhysicalObject* o = oi.key();
				world->removeObject(o);
				bullets.erase(oi);
				delete o;
			}
		}
		ViewerWidget::timerEvent(event);
		for(int i=0; i < int(sheep.size()); i++)
		{
			double Force_x = 0;
			double Force_y = 0;
			for(int j=0; j < int(sheep.size()); j++)
			{
				if(j != i)
				{
					double Distance_x = sheep[i]->pos.x - sheep[j]->pos.x;
					double Distance_y = sheep[i]->pos.y - sheep[j]->pos.y;
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
			for(int j=0; j < int(epucks.size()); j++)
			{
					double Distance_x = sheep[i]->pos.x - epucks[j]->pos.x;
					double Distance_y = sheep[i]->pos.y - epucks[j]->pos.y;
					double Distance_sq = pow(Distance_x,2) + pow(Distance_y,2);
					if(Distance_sq < 1500)
					{
						ShepherdDetected = true;
						Force_x = Force_x + (Csheepdog/Distance_sq)*(Distance_x/sqrt(Distance_sq));
						Force_y = Force_y + (Csheepdog/Distance_sq)*(Distance_y/sqrt(Distance_sq));
					}
			}
			double Angle = sheep[i]->angle;
			if (ShepherdDetected != true)
			{
				Force_x = Force_x + Force_Sheep_x;
				Force_y = Force_y + Force_Sheep_y;
			}
			Force_x = cos(-Angle) * Force_x - sin(-Angle) * Force_y;
   		Force_y = cos(-Angle) * Force_y + sin(-Angle) * Force_x;
			sheep[i]->leftSpeed = K1*Force_x + K2*Force_y;
			sheep[i]->rightSpeed = K1*Force_x - K2*Force_y;
			if(abs(sheep[i]->leftSpeed) > abs(sheep[i]->rightSpeed))
			{
				if(abs(sheep[i]->leftSpeed) >  SPEED_MAX/2)
				{
					float ratio = abs(sheep[i]->leftSpeed)/(SPEED_MAX/2);
					sheep[i]->leftSpeed = sheep[i]->leftSpeed/ratio;
					sheep[i]->rightSpeed = sheep[i]->rightSpeed/ratio;
				}
			}
			else if(abs(sheep[i]->rightSpeed) > abs(sheep[i]->leftSpeed))
			{
				if(abs(sheep[i]->rightSpeed) >  SPEED_MAX/2)
				{
					float ratio = abs(sheep[i]->rightSpeed)/(SPEED_MAX/2);
					sheep[i]->leftSpeed = sheep[i]->leftSpeed/ratio;
					sheep[i]->rightSpeed = sheep[i]->rightSpeed/ratio;
				}
			}

			if(Force_x == 0 && Force_y == 0)
			{
				sheep[i]->leftSpeed = (rand()%int(SPEED_MAX)) - SPEED_MAX/2;
				sheep[i]->rightSpeed = (rand()%int(SPEED_MAX)) - SPEED_MAX/2;
			}
		}
	}

	virtual void sceneCompletedHook()
	{

	}
};

// http://qtnode.net/wiki?title=Qt_with_cmake
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
	World world(300, 300, Color(0.9, 0.9, 0.9), igt ? World::GroundTexture(gt.width(), gt.height(), bits) : World::GroundTexture());
	EnkiPlayground viewer(&world);

	viewer.show();

	return app.exec();
}
