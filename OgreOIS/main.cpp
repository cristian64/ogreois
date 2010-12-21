#include "Ogre.h"
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"

#include <iostream>
using namespace std;

#define OIS1362
#ifdef OIS1362
#include <OIS/OIS.h>

// Manejador del ratón.
class MyMouseListener : public OIS::MouseListener
{
public:
	OIS::Mouse *mouse;
	Ogre::RenderWindow *renderWindow;
	Ogre::SceneManager *sceneManager;
	Ogre::SceneNode *seleccionado;

	int oldX, oldY;
	Ogre::Real distancia;

	bool control;

	bool mouseMoved(const OIS::MouseEvent &e)
	{
		oldX = e.state.X.abs;
		oldY = e.state.Y.abs;

		if (seleccionado != NULL && e.state.buttonDown(OIS::MB_Left))
		{
			// Normalizar los valores de posición.
			Ogre::Real realX = (float) e.state.X.abs / renderWindow->getViewport(0)->getActualWidth();
			Ogre::Real realY = (float) e.state.Y.abs / renderWindow->getViewport(0)->getActualHeight();

			// Se lanza el rayo que busca los elementos en ese píxel.
			Ogre::Ray ray;
			renderWindow->getViewport(0)->getCamera()->getCameraToViewportRay(Ogre::Real(realX), Ogre::Real(realY), &ray);
			Ogre::RaySceneQuery *query = sceneManager->createRayQuery(ray);
			Ogre::RaySceneQueryResult results = query->execute();

			// Trasladamos el objeto seleccionado en un punto del rayo y a la misma distancia.
			seleccionado->setPosition(ray.getPoint(distancia));
		}
		else if (seleccionado != NULL && e.state.buttonDown(OIS::MB_Right))
		{
			// Se rota el objeto seleccionado.
			seleccionado->pitch(Ogre::Degree(e.state.Y.rel));
			seleccionado->roll(Ogre::Degree(e.state.X.rel));
		}
		else if (seleccionado != NULL && e.state.Z.rel != 0)
		{
			// Se escala el objeto seleccionado.
			float escalado = (e.state.Z.rel > 0) ? 1.05f : 0.95f;
			seleccionado->scale(escalado, escalado, escalado);
		}
		else if (seleccionado == NULL && e.state.buttonDown(OIS::MB_Right))
		{
			// Normalizar los valores de posición.
			Ogre::Real realX = (float) e.state.X.abs / renderWindow->getViewport(0)->getActualWidth();
			Ogre::Real realY = (float) e.state.Y.abs / renderWindow->getViewport(0)->getActualHeight();

			// Se lanza el rayo que busca los elementos en ese píxel.
			Ogre::Ray ray;
			renderWindow->getViewport(0)->getCamera()->getCameraToViewportRay(Ogre::Real(realX), Ogre::Real(realY), &ray);
			Ogre::RaySceneQuery *query = sceneManager->createRayQuery(ray);
			Ogre::RaySceneQueryResult results = query->execute();

			// Reorientamos la cámara.
			renderWindow->getViewport(0)->getCamera()->lookAt(ray.getOrigin());

			// Colocamos el ratón en el centro de la pantalla.
			OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(mouse->getMouseState());
			mutableMouseState.X.abs = renderWindow->getWidth()/2;
			mutableMouseState.Y.abs = renderWindow->getHeight()/2;
		}

		return true;
	}

	bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
	{
		cout << "Ratón: " << "(" << e.state.X.abs << ", " << e.state.Y.abs << ")" << endl;
		return true;
	}

	bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
	{
		if (id == OIS::MB_Left)
		{
			seleccionado = NULL;

			// Normalizar los valores de posición.
			Ogre::Real realX = (float) e.state.X.abs / renderWindow->getViewport(0)->getActualWidth();
			Ogre::Real realY = (float) e.state.Y.abs / renderWindow->getViewport(0)->getActualHeight();

			// Se lanza el rayo que busca los elementos en ese píxel.
			Ogre::Ray ray;
			renderWindow->getViewport(0)->getCamera()->getCameraToViewportRay(Ogre::Real(realX), Ogre::Real(realY), &ray);
			Ogre::RaySceneQuery *query = sceneManager->createRayQuery(ray);
			Ogre::RaySceneQueryResult results = query->execute();

			cout << "Elementos: " << results.size() << endl;

			// Se recorre la lista de objectos
			Ogre::RaySceneQueryResult::iterator it = results.begin();
			Ogre::Real minDistance = Ogre::Real(99999999);
			for (; it != results.end(); it++)
			{
				Ogre::RaySceneQueryResultEntry *entry = &(*it);
				if (minDistance > entry->distance)
				{
					if (entry->movable->getParentSceneNode() != sceneManager->getSceneNode("scene3"))
						seleccionado = entry->movable->getParentSceneNode();
					minDistance = entry->distance;
				}
			}

			// Distancia a la que estaba el objeto.
			if (seleccionado)
			{
				Ogre::Vector3 segmento = sceneManager->getCamera("camara1")->getPosition() - seleccionado->getPosition();
				distancia = segmento.length();
			}
		}
		return true;
	}
};

#endif

void initBlendMaps(Ogre::Terrain* terrain)
{
    Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    Ogre::Real minHeight0 = 70;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 70;
    Ogre::Real fadeDist1 = 15;
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
	   for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
	   {
		  Ogre::Real tx, ty;

		  blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
		  Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
		  Ogre::Real val = (height - minHeight0) / fadeDist0;
		  val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);

		  val = (height - minHeight1) / fadeDist1;
		  val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
		  *pBlend1++ = val;
	   }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
}

int main()
{
	#ifdef CEGUI1362
	// Se inicializa CEGUI.
	CEGUI::Renderer* myRenderer = new CEGUI::Renderer();
	new CEGUI::System(myRenderer);
	#endif

	// Se inicializa Ogre.
	Ogre::Root *root = new Ogre::Root("plugins.cfg", "ogre.cfg", "ogre.log");
	root->restoreConfig();
	Ogre::RenderWindow *renderWindow = root->initialise(true, "OGRE + OIS");

	// Se cargan los recursos de Ogre.
	Ogre::ResourceGroupManager::getSingletonPtr()->addResourceLocation("models/", "FileSystem");
	Ogre::ResourceGroupManager::getSingletonPtr()->addResourceLocation("textures/", "FileSystem");
	Ogre::ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();

	// Creación de una escena con sombras y luz ambiente.
	Ogre::SceneManager *sceneManager = root->createSceneManager(Ogre::ST_INTERIOR);
	sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	sceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));
	sceneManager->setSkyDome(true, "CloudySky", 5, 8, 500);

	// Introducción de la cámara pricipal de la escena.
	Ogre::Camera *camera = sceneManager->createCamera("camara1");
	camera->setPosition(Ogre::Vector3(0,100,500));
	camera->lookAt(Ogre::Vector3(0,0,0));
	camera->setNearClipDistance(5);

	// Crea un viewport en toda la ventana.
	Ogre::Viewport* viewport = renderWindow->addViewport(camera);
	viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
	camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

	// Niebla.
	Ogre::ColourValue fadeColour(0.5, 0.5, 0.5);
	sceneManager->setFog(Ogre::FOG_LINEAR, fadeColour, 0.0, 10, 1200);
	viewport->setBackgroundColour(fadeColour);

	// Otro viewport para el cursor.
	Ogre::Camera *camera2 = sceneManager->createCamera("camara2");
	camera2->setPosition(Ogre::Vector3(0,-1000,0));
	camera2->lookAt(Ogre::Vector3(0,-1010,0));
	camera2->setNearClipDistance(5);
	Ogre::Viewport* viewport2 = renderWindow->addViewport(camera2, 1, 0, 0, 0.01f, 0.01f);
	viewport2->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Introducción de una luz focal, para evaluar las sombras.
	Ogre::Light* light = sceneManager->createLight("light1");
	light->setType(Ogre::Light::LT_POINT);
	light->setPosition(Ogre::Vector3(100,150,300));
	light->setDiffuseColour(0.4, 0.4, 0.4);
	light->setSpecularColour(0.2, 0.2, 0.2);

	// Elementos de la escena.
	Ogre::Entity *ogre = sceneManager->createEntity("ogre1", "ogrehead.mesh");
	Ogre::SceneNode *sceneNode1 = sceneManager->getRootSceneNode()->createChildSceneNode("scene1");
	sceneNode1->attachObject(ogre);
	sceneNode1->setPosition(10, 40, 10);
	Ogre::Entity *sphere = sceneManager->createEntity("sphere1", "Esfera.mesh");
	Ogre::SceneNode *sceneNode2 = sceneManager->getRootSceneNode()->createChildSceneNode("scene2");
	sceneNode2->attachObject(sphere);
	sceneNode2->scale(30, 30, 30);
	sceneNode2->setPosition(10, 110, 20);
	Ogre::Entity *floor = sceneManager->createEntity("floor1", "cube.1m.mesh");
	Ogre::SceneNode *sceneNode3 = sceneManager->getRootSceneNode()->createChildSceneNode("scene3");
	sceneNode3->attachObject(floor);
	sceneNode3->setScale(10, 0.1, 10);
	sceneNode3->scale(100, 100, 100);
	Ogre::Entity *penguin = sceneManager->createEntity("penguin1", "penguin.mesh");
	penguin->getAnimationState("amuse")->setEnabled(true);
	penguin->getAnimationState("amuse")->setLoop(true);
	Ogre::SceneNode *sceneNode4 = sceneManager->getRootSceneNode()->createChildSceneNode("scene4");
	sceneNode4->attachObject(penguin);
	sceneNode4->setPosition(60, 40, -10);
	Ogre::Entity *robot = sceneManager->createEntity("robot1", "robot.mesh");
	Ogre::SceneNode *sceneNode5 = sceneManager->getRootSceneNode()->createChildSceneNode("scene5");
	sceneNode5->attachObject(robot);
	robot->getAnimationState("Walk")->setEnabled(true);
	robot->getAnimationState("Walk")->setLoop(true);
	sceneNode5->setPosition(-160, 0, -10);
	Ogre::Entity *ninja = sceneManager->createEntity("ninja1", "ninja.mesh");
	Ogre::SceneNode *sceneNode6 = sceneManager->getRootSceneNode()->createChildSceneNode("scene6");
	sceneNode6->attachObject(ninja);
	ninja->getAnimationState("Attack2")->setEnabled(true);
	ninja->getAnimationState("Attack2")->setLoop(true);
	sceneNode6->scale(0.3f, 0.3f, 0.3f);
	sceneNode6->setPosition(-50, 3, 230);

	#ifndef TERRAIN1362
	Ogre::TerrainGlobalOptions *terrainGlobals = new Ogre::TerrainGlobalOptions();
	Ogre::TerrainGroup *terrainGroup = new Ogre::TerrainGroup(sceneManager, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
	//terrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
	terrainGroup->setOrigin(Ogre::Vector3::ZERO);

	// Configure global
	terrainGlobals->setMaxPixelError(8);
	// testing composite map
	terrainGlobals->setCompositeMapDistance(3000);

	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	terrainGlobals->setLightMapDirection(light->getDerivedDirection());
	terrainGlobals->setCompositeMapAmbient(sceneManager->getAmbientLight());
	terrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

	// Configure default import settings for if we use imported image
	Ogre::Terrain::ImportData& defaultimp = terrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = 513;
	defaultimp.worldSize = 12000.0f;
	defaultimp.inputScale = 600;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");

	bool terrainsImported = false;
	for (long x = 0; x <= 0; ++x)
	{
		for (long y = 0; y <= 0; ++y)
		{
			Ogre::String filename = terrainGroup->generateFilename(x, y);
			if (Ogre::ResourceGroupManager::getSingleton().resourceExists(terrainGroup->getResourceGroup(), filename))
			{
				terrainGroup->defineTerrain(x, y);
			}
			else
			{
				Ogre::Image img;
				bool flipX = x % 2 != 0;
				bool flipY = y % 2 != 0;
				img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				if (flipX)
					img.flipAroundY();
				if (flipY)
					img.flipAroundX();
				terrainGroup->defineTerrain(x, y, &img);
				terrainsImported = true;
			}
		}
	}

	// sync load since we want everything in place when we start
	terrainGroup->loadAllTerrains(true);

	if (terrainsImported)
	{
	    Ogre::TerrainGroup::TerrainIterator ti = terrainGroup->getTerrainIterator();
	    while(ti.hasMoreElements())
	    {
		   Ogre::Terrain* t = ti.getNext()->instance;
		   initBlendMaps(t);
	    }
	}

	terrainGroup->freeTemporaryResources();
	#endif

	#ifdef OIS1362
	// Inicializar OIS indicándole la ventana que generará eventos.
	size_t hWnd = 0;
	renderWindow->getCustomAttribute("WINDOW", &hWnd);
	OIS::InputManager *inputManager = OIS::InputManager::createInputSystem(hWnd);

	// Creación de un teclado.
	OIS::Keyboard *keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));

	// Creación de un ratón.
	OIS::Mouse *mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
	const OIS::MouseState &mouseState = mouse->getMouseState();
	mouseState.width = renderWindow->getWidth();
	mouseState.height = renderWindow->getHeight();

	// Manejador del ratón.
	MyMouseListener myMouseListener;
	myMouseListener.renderWindow = renderWindow;
	myMouseListener.sceneManager = sceneManager;
	myMouseListener.seleccionado = NULL;
	myMouseListener.control = false;
	myMouseListener.mouse = mouse;
	mouse->setEventCallback(&myMouseListener);
	#endif

	float velocidad = 10.0f;
	while (true)
	{
		#ifdef OIS1362
		// Captura del teclado.
		keyboard->capture();
		if (keyboard->isKeyDown(OIS::KC_ESCAPE))
		{
			break;
		}
		if (keyboard->isKeyDown(OIS::KC_LCONTROL))
		{
			myMouseListener.control = true;
		}
		else
		{
			myMouseListener.control = false;
		}
		if (keyboard->isKeyDown(OIS::KC_F1))
		{
			if (sceneManager->getShadowTechnique() == Ogre::SHADOWTYPE_STENCIL_MODULATIVE)
			{
				sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
			}
			else
			{
				sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
			}
		}

		if (keyboard->isKeyDown(OIS::KC_W))
		{
			Ogre::Vector3 posicion = camera->getPosition();
			posicion += camera->getDirection();
			if (myMouseListener.control)
			{
				Ogre::Vector3 direccion = posicion - camera->getPosition();
				camera->setPosition(camera->getPosition() + direccion * velocidad);
			}
			else
			{
				camera->setPosition(posicion);
			}
		}
		if (keyboard->isKeyDown(OIS::KC_S))
		{
			Ogre::Vector3 posicion = camera->getPosition();
			posicion -= camera->getDirection();
			if (myMouseListener.control)
			{
				Ogre::Vector3 direccion = posicion - camera->getPosition();
				camera->setPosition(camera->getPosition() + direccion * velocidad);
			}
			else
			{
				camera->setPosition(posicion);
			}
		}
		if (keyboard->isKeyDown(OIS::KC_A))
		{
			Ogre::Vector3 posicion = camera->getPosition();
			posicion -= camera->getRight();
			if (myMouseListener.control)
			{
				Ogre::Vector3 direccion = posicion - camera->getPosition();
				camera->setPosition(camera->getPosition() + direccion * velocidad);
			}
			else
			{
				camera->setPosition(posicion);
			}
		}
		if (keyboard->isKeyDown(OIS::KC_D))
		{
			Ogre::Vector3 posicion = camera->getPosition();
			posicion += camera->getRight();
			if (myMouseListener.control)
			{
				Ogre::Vector3 direccion = posicion - camera->getPosition();
				camera->setPosition(camera->getPosition() + direccion * velocidad);
			}
			else
			{
				camera->setPosition(posicion);
			}
		}

		// Captura del ratón y dibujado del cursor.
		mouse->capture();
		viewport2->setDimensions((float) myMouseListener.oldX/renderWindow->getWidth(), (float) myMouseListener.oldY/renderWindow->getHeight(), 0.006f, 0.01f);

		// Dibuja el bounding box del objeto seleccionado.
		if (myMouseListener.seleccionado != NULL)
			myMouseListener.seleccionado->showBoundingBox(true);
		#endif

		penguin->getAnimationState("amuse")->addTime(0.05f);
		robot->getAnimationState("Walk")->addTime(0.04f);
		ninja->getAnimationState("Attack2")->addTime(0.01f);

		root->renderOneFrame();
		renderWindow->update(true);

		#ifdef OIS1362
		// Oculta el bounding box del objeto seleccionado.
		if (myMouseListener.seleccionado != NULL)
			myMouseListener.seleccionado->showBoundingBox(false);
		#endif
	}

	#ifdef OIS1362
	// Se libera OIS.
	inputManager->destroyInputObject(keyboard);
	inputManager->destroyInputObject(mouse);
	OIS::InputManager::destroyInputSystem(inputManager);
	keyboard = NULL;
	mouse = NULL;
	inputManager = NULL;
	#endif

	// Se libera Ogre.
	root->shutdown();
	delete root;
}
