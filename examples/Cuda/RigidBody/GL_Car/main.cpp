#include <QtApp.h>

#include <SceneGraph.h>

#include <RigidBody/Vechicle.h>

#include <GLRenderEngine.h>
#include <GLPointVisualModule.h>
#include <GLSurfaceVisualModule.h>
#include <GLWireframeVisualModule.h>

#include <Mapping/DiscreteElementsToTriangleSet.h>
#include <Mapping/ContactsToEdgeSet.h>
#include <Mapping/ContactsToPointSet.h>
#include <Mapping/AnchorPointToPointSet.h>

#include "Collision/NeighborElementQuery.h"

#include <Module/GLPhotorealisticInstanceRender.h>

#include "GltfLoader.h"


using namespace std;
using namespace dyno;

std::shared_ptr<SceneGraph> creatCar()
{
	std::shared_ptr<SceneGraph> scn = std::make_shared<SceneGraph>();

	auto jeep = scn->addNode(std::make_shared<Vechicle<DataType3f>>());

	auto prRender = std::make_shared<GLPhotorealisticInstanceRender>();
	jeep->inTextureMesh()->connect(prRender->inTextureMesh());
	jeep->stateInstanceTransform()->connect(prRender->inTransform());
	jeep->graphicsPipeline()->pushModule(prRender);

	BoxInfo box, box2;
	//box.center = Vec3f(0, 0.450, 0.148);
	//box.center = Vec3f(0, 1.171, 0.148);
	box.center = Vec3f(0, 1.171, -0.011);
	box.halfLength = Vec3f(1.011, 0.2, 1.8);
	box2.center = Vec3f(0, 1.044, -2.254);
	box2.halfLength = Vec3f(0.450, 0.450, 0.250);
	CapsuleInfo capsule1, capsule2, capsule3, capsule4;

	capsule1.center = Vec3f(0.812, 0.450, 1.722);
	capsule1.rot = Quat1f(M_PI / 2, Vec3f(0, 0, 1));
	capsule1.halfLength = 0.1495;
	capsule1.radius = 0.450;
	capsule2.center = Vec3f(-0.812, 0.450, 1.722);
	capsule2.rot = Quat1f(M_PI / 2, Vec3f(0, 0, 1));
	capsule2.halfLength = 0.1495;
	capsule2.radius = 0.450;
	capsule3.center = Vec3f(-0.812, 0.450, -1.426);
	capsule3.rot = Quat1f(M_PI / 2, Vec3f(0, 0, 1));
	capsule3.halfLength = 0.1495;
	capsule3.radius = 0.450;
	capsule4.center = Vec3f(0.812, 0.450, -1.426);
	capsule4.rot = Quat1f(M_PI / 2, Vec3f(0, 0, 1));
	capsule4.halfLength = 0.1495;
	capsule4.radius = 0.450;

	RigidBodyInfo rigidbody;

	Vec3f offset = Vec3f(0.0f, -0.721f, 0.0f);
	rigidbody.offset = offset;
	jeep->addBox(box, rigidbody, 1000);

	rigidbody.offset = Vec3f(0.0f);
	jeep->addBox(box2, rigidbody);
	Real wheel_velocity = 50;

	jeep->addCapsule(capsule1, rigidbody, 100);
	jeep->addCapsule(capsule2, rigidbody, 100);
	jeep->addCapsule(capsule3, rigidbody, 100);
	jeep->addCapsule(capsule4, rigidbody, 100);

	HingeJoint<Real> joint1(2, 0);
	joint1.setAnchorPoint(capsule1.center, capsule1.center, box.center + offset, capsule1.rot, box.rot);
	joint1.setMoter(wheel_velocity, true);
	joint1.setAxis(Vec3f(1, 0, 0), capsule1.rot, box.rot);
	jeep->addHingeJoint(joint1);
	HingeJoint<Real> joint2(3, 0);
	joint2.setAnchorPoint(capsule2.center, capsule2.center, box.center + offset, capsule2.rot, box.rot);
	joint2.setMoter(wheel_velocity, true);
	joint2.setAxis(Vec3f(1, 0, 0), capsule2.rot, box.rot);
	jeep->addHingeJoint(joint2);
	HingeJoint<Real> joint3(4, 0);
	joint3.setAnchorPoint(capsule3.center, capsule3.center, box.center + offset, capsule3.rot, box.rot);
	joint3.setMoter(wheel_velocity, true);
	joint3.setAxis(Vec3f(1, 0, 0), capsule3.rot, box.rot);
	jeep->addHingeJoint(joint3);
	HingeJoint<Real> joint4(5, 0);
	joint4.setAnchorPoint(capsule4.center, capsule4.center, box.center + offset, capsule4.rot, box.rot);
	joint4.setMoter(wheel_velocity, true);
	joint4.setAxis(Vec3f(1, 0, 0), capsule4.rot, box.rot);
	jeep->addHingeJoint(joint4);


	FixedJoint<Real> joint5(0, 1);
	joint5.setAnchorPoint((box.center + box2.center) / 2, box.center + offset, box2.center, box.rot, box2.rot);
	jeep->addFixedJoint(joint5);

	jeep->bind(0, Pair<uint, uint>(5, 0));
	jeep->bind(1, Pair<uint, uint>(4, 0));
	jeep->bind(2, Pair<uint, uint>(0, 0));
	jeep->bind(3, Pair<uint, uint>(1, 0));
	jeep->bind(4, Pair<uint, uint>(2, 0));
	jeep->bind(5, Pair<uint, uint>(3, 0));

	auto gltf = scn->addNode(std::make_shared<GltfLoader<DataType3f>>());
	gltf->setVisible(false);
	gltf->varFileName()->setValue(getAssetPath() + "Jeep/JeepGltf/jeep.gltf");

	gltf->stateTextureMesh()->connect(jeep->inTextureMesh());

	auto mapper = std::make_shared<DiscreteElementsToTriangleSet<DataType3f>>();
	jeep->stateTopology()->connect(mapper->inDiscreteElements());
	jeep->graphicsPipeline()->pushModule(mapper);

	auto sRender = std::make_shared<GLSurfaceVisualModule>();
	sRender->setColor(Color(0.3f, 0.5f, 0.9f));
	sRender->setAlpha(0.8f);
	sRender->setRoughness(0.7f);
	sRender->setMetallic(3.0f);
	mapper->outTriangleSet()->connect(sRender->inTriangleSet());
	jeep->graphicsPipeline()->pushModule(sRender);

	//TODO: to enable using internal modules inside a node
	//Visualize contact normals
	auto elementQuery = std::make_shared<NeighborElementQuery<DataType3f>>();
	jeep->stateTopology()->connect(elementQuery->inDiscreteElements());
	jeep->stateCollisionMask()->connect(elementQuery->inCollisionMask());
	jeep->graphicsPipeline()->pushModule(elementQuery);

	auto contactMapper = std::make_shared<ContactsToEdgeSet<DataType3f>>();
	elementQuery->outContacts()->connect(contactMapper->inContacts());
	contactMapper->varScale()->setValue(0.00002);
	jeep->graphicsPipeline()->pushModule(contactMapper);

	auto wireRender = std::make_shared<GLWireframeVisualModule>();
	wireRender->setColor(Color(0, 0, 1));
	contactMapper->outEdgeSet()->connect(wireRender->inEdgeSet());
	jeep->graphicsPipeline()->pushModule(wireRender);

	//Visualize contact points
	auto contactPointMapper = std::make_shared<ContactsToPointSet<DataType3f>>();
	elementQuery->outContacts()->connect(contactPointMapper->inContacts());
	jeep->graphicsPipeline()->pushModule(contactPointMapper);

	auto pointRender = std::make_shared<GLPointVisualModule>();
	pointRender->setColor(Color(1, 0, 0));
	pointRender->varPointSize()->setValue(0.00003f);
	contactPointMapper->outPointSet()->connect(pointRender->inPointSet());
	jeep->graphicsPipeline()->pushModule(pointRender);

	//Visualize Anchor point for joint
	auto anchorPointMapper = std::make_shared<AnchorPointToPointSet<DataType3f>>();
	jeep->stateCenter()->connect(anchorPointMapper->inCenter());
	jeep->stateRotationMatrix()->connect(anchorPointMapper->inRotationMatrix());
	jeep->stateBallAndSocketJoints()->connect(anchorPointMapper->inBallAndSocketJoints());
	jeep->stateSliderJoints()->connect(anchorPointMapper->inSliderJoints());
	//rigid->stateHingeJoints()->connect(anchorPointMapper->inHingeJoints());
	//rigid->stateFixedJoints()->connect(anchorPointMapper->inFixedJoints());
	jeep->graphicsPipeline()->pushModule(anchorPointMapper);

	auto pointRender2 = std::make_shared<GLPointVisualModule>();
	pointRender2->setColor(Color(1, 0, 0));
	pointRender2->varPointSize()->setValue(0.03f);
	anchorPointMapper->outPointSet()->connect(pointRender2->inPointSet());
	jeep->graphicsPipeline()->pushModule(pointRender2);
	return scn;
}

int main()
{
	QtApp app;
	app.setSceneGraph(creatCar());
	app.initialize(1280, 768);
	app.mainLoop();

	return 0;
}


