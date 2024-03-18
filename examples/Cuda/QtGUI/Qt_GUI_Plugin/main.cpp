#include <QtApp.h>
using namespace dyno;

#include "RigidBody/initializeRigidBody.h"
#include "ParticleSystem/initializeParticleSystem.h"
#include "Peridynamics/initializePeridynamics.h"
#include "SemiAnalyticalScheme/initializeSemiAnalyticalScheme.h"
#include "Volume/initializeVolume.h"
#include "Multiphysics/initializeMultiphysics.h"

#include "initializeModeling.h"
#include "initializeIO.h"

/**
 * @brief This example demonstrate how to load plugin libraries in a static way
 */

int main()
{
	Modeling::initStaticPlugin();
	RigidBody::initStaticPlugin();
	PaticleSystem::initStaticPlugin();
	Peridynamics::initStaticPlugin();
	SemiAnalyticalScheme::initStaticPlugin();
	Volume::initStaticPlugin();
	Multiphysics::initStaticPlugin();
	dynoIO::initStaticPlugin();

	QtApp app;
	app.initialize(1366, 800);
	app.mainLoop();

	return 0;
}