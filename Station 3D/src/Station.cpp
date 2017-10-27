/*
 *  Station.cpp
 *  Station3D
 *
 *  Created by Valentin on 16/02/2014.
 *  Copyright (c) 2014 Valentin Mercier. All rights reserved.
 */

#include "Station.h"
#include "PauseMenu.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

Station::Station(IrrlichtDevice *device, domain_t *domain)
{
        /* Station domain graph*/
        m_domain = domain;

        /* Main Irrlicht objects */
        m_device = device;
        m_smgr = device->getSceneManager();
        m_driver = device->getVideoDriver();
        m_guienv = device->getGUIEnvironment();
        m_timer = device->getTimer();

        /* Textures */
        land = m_driver->getTexture("land.jpg");
        land_bw = m_driver->getTexture("land_bw.jpg");
        tree = m_driver->getTexture("tree.png");
        tree_bw = m_driver->getTexture("tree_bw.png");
        sky = m_driver->getTexture("skydome.jpg");
        sky_bw = m_driver->getTexture("skydome_bw.jpg");


        /* Mountain */
        IMesh* mountainmesh = m_smgr->getMesh("Mountain2.3ds");
	mountain = m_smgr->addMeshSceneNode(mountainmesh);
	mountain->setMaterialFlag(EMF_LIGHTING, false);
	mountain->setMaterialTexture(0, land);
	mountain->setMaterialFlag(EMF_FOG_ENABLE, true);
        mountain->setScale(vector3df(1,1.2,1));
        wireframe = false;

        /* Trees */
	IMesh* treemesh = m_smgr->getMesh("Trees.3ds");
	trees = m_smgr->addMeshSceneNode(treemesh);
	trees->setMaterialFlag(EMF_LIGHTING, false);
	trees->setMaterialTexture(0, tree);
	trees->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
	trees->setMaterialFlag(EMF_FOG_ENABLE, true);
        trees->setScale(vector3df(1,1.2,1));



        /* Fog */
        m_driver->setFog(SColor(255,0x80,0xa6,0xFF), EFT_FOG_LINEAR, 12, 17, .003f, true, false);

        /* Skyboxes */
        skydome_bw = m_smgr->addSkyDomeSceneNode(sky_bw);
        skydome = m_smgr->addSkyDomeSceneNode(sky);

        /* Sun */
        sun = m_smgr->addLightSceneNode(0, vector3df(0, 25, 110), SColorf(1.0f, 1.0f, 1.0f), 10000.0f);

        /* Rotate world to face the sun */
        mountain->setRotation(vector3df(0,90,0));
        trees->setRotation(vector3df(0,90,0));

        /* Camera */
	camera = m_smgr->addCameraSceneNodeFPS(0,0,0);

        /* Camera Animators */
        /* Movement and Orientation */
        list<ISceneNodeAnimator *> animators = camera->getAnimators();
        fps_animator = (ISceneNodeAnimatorCameraFPS *) *(animators.begin());
        fps_animator->grab();

        /* Mountain Collision */
        ITriangleSelector *selector = m_smgr->createTriangleSelector(mountainmesh, mountain);
        mountain->setTriangleSelector(selector);
        collision_animator = m_smgr->createCollisionResponseAnimator(selector, mountain, vector3df(1, 0.6, 1), vector3df(0,0,0), vector3df(0, 0.4, 0));
        selector->drop();
        collision_animator->grab();

        /* Intro Straight Movement */
        intro_animator = m_smgr->createFlyStraightAnimator(vector3df(-18.485241, 2.514725, -34.014355), vector3df(8.302281, 1.800478, -5.261674), 15000, true);
        camera->addAnimator(intro_animator);
        intro_animator->drop();




        /* Helicopter */
        IMesh *copteramesh = m_smgr->getMesh("copter_a.3ds");
        coptera = m_smgr->addMeshSceneNode(copteramesh, camera);
        coptera->setMaterialTexture(0, m_driver->getTexture("copter-a.png"));
        coptera->setPosition(vector3df(0,-0.2,0.8));
        coptera->setRotation(vector3df(0,185,0));
        coptera->setScale(vector3df(0.05, 0.05, 0.05));

        IMesh *copterbmesh = m_smgr->getMesh("copter_b.3ds");
        copterb = m_smgr->addMeshSceneNode(copterbmesh, coptera);
        copterb->setMaterialTexture(0, m_driver->getTexture("copter-b.png"));

        IMesh *rotormesh = m_smgr->getMesh("rotor.3ds");
        rotora = m_smgr->addMeshSceneNode(rotormesh, copterb);
        rotora->setPosition(vector3df(2, 1.7, -0.5));

        rotorb = m_smgr->addMeshSceneNode(rotormesh, copterb);
        rotorb->setPosition(vector3df(-2, 1.7, -0.5));

        rotoranimatora = m_smgr->createRotationAnimator(vector3df(0,10,0));
        rotoranimatorb = m_smgr->createRotationAnimator(vector3df(0,10,0));
        rotora->addAnimator(rotoranimatora);
        rotorb->addAnimator(rotoranimatorb);
        rotoranimatora->grab();
        rotoranimatorb->grab();





        /* Blurry effect */
        ppRenderer = new CRendererPostProc(m_smgr, dimension2d<unsigned int>(1024, 512), true, true,  SColor(255,0,0,0));
        ppBlur = new CEffectPostProc(ppRenderer, dimension2d<unsigned int>(1024, 512), PP_BLUR, 0.01f);
        ppBlur->setQuality(PPQ_BEST);

        /* Opening fader */
        openingfader = m_guienv->addInOutFader();
        openingfader->setColor(SColor(255,255,255,255));
        openingfader->fadeIn(6500);

        /* Path movement related variables */
        paused = false;
        ended = false;
        moving = false;
        path_index = 0;
        path_element_animator = NULL;
        text_node = NULL;

        /* 2D Font */
        IGUISkin* skin = m_guienv->getSkin();
	font = m_guienv->getFont("fontlucida.png");
	skin->setFont(font);
}







IrrlichtDevice *Station::getDevice()
{
        return m_device;
}

node_vector *Station::getNodes()
{
        return get_domain_nodes(m_domain);
}

edge_vector *Station::getEdges()
{
        return get_domain_edges(m_domain);
}

void Station::getNodeLocation(char location[255], int id)
{
        get_node_location(location, m_domain, id);
}


void Station::update_route_times(int route_times[NUMBER_OF_EDGE_TYPES -1])
{
        struct SEdgeTypesWithStringsAndTimes *edge_type = get_domain_edge_types(m_domain);
        for (s32 i = 0; i < NUMBER_OF_EDGE_TYPES - 1; i++)
                edge_type[i].route_time = route_times[i];
        update_domain_route_times(m_domain);
}

int *Station::getRouteTimes()
{
        struct SEdgeTypesWithStringsAndTimes *edge_type = get_domain_edge_types(m_domain);
        int *route_times = new int[NUMBER_OF_EDGE_TYPES - 1];
        for (s32 i = 0; i < NUMBER_OF_EDGE_TYPES - 1; i++)
                route_times[i] = edge_type[i].route_time;
        return route_times;
}

bool Station::isPaused()
{
        return paused;
}

bool Station::isEnded()
{
        return ended;
}

bool Station::shouldHelicopterMove()
{
        return moving;
}











void Station::togglePauseMenu()
{
        paused = !paused;
        if (paused) {
                clearPath();
                sun->setVisible(false);
                camera->removeAnimators();
                intro_animator = NULL;
                rotora->removeAnimators();
                rotorb->removeAnimators();
                mountain->setMaterialTexture(0, land_bw);
                trees->setMaterialTexture(0, tree_bw);
                skydome->setVisible(false);
                m_driver->setFog(SColor(255,255,255,255), EFT_FOG_LINEAR, 12, 17, .003f, true, false);
                m_device->getCursorControl()->setVisible(true);
        } else {
                mountain->setMaterialTexture(0, land);
                trees->setMaterialTexture(0, tree);
                skydome->setVisible(true);
                m_driver->setFog(SColor(255,0x80,0xa6,0xFF), EFT_FOG_LINEAR, 12, 17, .003f, true, false);
                m_device->getCursorControl()->setVisible(false);
                coptera->setRotation(vector3df(0,180,0));
                rotora->addAnimator(rotoranimatora);
                rotorb->addAnimator(rotoranimatorb);
                fps_animator->setMoveSpeed(0.002f);
                fps_animator->setRotateSpeed(100.0f);
                camera->addAnimator(fps_animator);
                camera->addAnimator(collision_animator);
                sun->setVisible(true);
        }

}

void Station::toggleWireframe()
{
        wireframe = !wireframe;
        mountain->setMaterialFlag(EMF_WIREFRAME, wireframe);
}

void Station::end()
{
        ended = true;
}


/* Calculate the path for moving from source to destination and tells the controller that
 * the movemement animation can start */
void Station::startHelicopterMovement(s32 source, s32 destination, s32 mode)
{
        /* Find corresponding path filter */
        enum EPathType filter;
        switch(mode){
        case 0: filter = FASTEST;
                break;
        case 1: filter = EASIEST;
                break;
        case 2: filter = NICEST;
                break;
        }

        /* Calculate path */
        edge_vector *ev;
        ev = get_path(filter, m_domain, source, destination);

        /* Get domain nodes, for further use */
        node_vector *nv = getNodes();
        node_t *start_node, *end_node;
        char node_location[255];
        int textlen, shorttextlen;

        /* If we still have an old animation in progress we stop it */
        clearPath();

        /* We then prepare the path working element by element */
        struct SPathElement *path_element;
        dimension2d<u32> deskres;
        deskres = m_device->getVideoModeList()->getDesktopResolution();

        /* Path opener */
        path_element = new struct SPathElement;
        path_element->edge_id = -1;
        path_element->node_id = ev->items[0]->parent;
        path_element->node_name = getNodes()->items[ev->items[0]->parent]->name;
        path_element->start_point = camera->getPosition();
        start_node = nv->items[ev->items[0]->parent];
        path_element->is_end_node_real_point = start_node->is_real_point;
        path_element->end_point = vector3df(start_node->pos_x, start_node->pos_y, start_node->pos_z);
        path_element->old_notice_frame = rect<s32>(0, 100, deskres.Width/4, 200);
        path_element->new_notice_frame = rect<s32>(0, 100, deskres.Width, 200);
        getNodeLocation(node_location, source);
        shorttextlen = strlen(node_location);
        textlen = strlen("Aller jusqu'") + shorttextlen;
        path_element->old_notice_text = rect<s32>(deskres.Width/8 - 6*shorttextlen, 140, deskres.Width/8 + 6*shorttextlen, 60);
        path_element->new_notice_text = rect<s32>(deskres.Width/2 - 3*textlen, 140, deskres.Width/2 + 3*textlen, 60);
        path_element->notice_color = SColor(255, 0x20, 0x20, 0x20);
        path.push_back(*path_element);

        for (u32 i = 0; i < ev->length; i++) {

                /* Allocate new element */
                path_element = new struct SPathElement;

                /* Find element extremas */
                start_node = nv->items[ev->items[i]->parent];
                end_node = nv->items[ev->items[i]->child];
                path_element->node_id = ev->items[i]->child;
                path_element->edge_name = ev->items[i]->name;
                path_element->node_name = end_node->name;
                path_element->is_end_node_real_point = end_node->is_real_point;
                path_element->start_point = vector3df(start_node->pos_x, start_node->pos_y, start_node->pos_z);
                path_element->end_point = vector3df(end_node->pos_x, end_node->pos_y, end_node->pos_z);

                /* Build Notice GUI */
                path_element->old_notice_frame = rect<s32>(0, 100, deskres.Width/4, 200);
                //path_element->new_notice_frame = rect<s32>(deskres.Width/4, 100, deskres.Width - 1, 200);
                path_element->new_notice_frame = rect<s32>(0, 100, deskres.Width, 200);
                getNodeLocation(node_location, ev->items[i]->child);
                shorttextlen = strlen(node_location);
                textlen = strlen(ev->items[i]->name) + strlen("Prendre ");
                path_element->old_notice_text = rect<s32>(deskres.Width/8 - 6*shorttextlen, 140, deskres.Width/8 + 6*shorttextlen, 160);
                //path_element->new_notice_text = rect<s32>(deskres.Width*5/8 - 6*textlen, 140, deskres.Width*5/8 + 6*textlen, 160);
                path_element->new_notice_text = rect<s32>(deskres.Width/2 - 3*textlen, 140, deskres.Width/2 + 3*textlen, 160);
                switch(ev->items[i]->type) {
                        case V: path_element->notice_color = SColor(255, 0, 255, 0);
                                break;
                        case B: path_element->notice_color = SColor(255, 0, 0, 255);
                                break;
                        case R: path_element->notice_color = SColor(255, 255, 0, 0);
                                break;
                        case N: path_element->notice_color = SColor(255,0,0,0);
                                break;
                        case KL: path_element->notice_color = SColor(255, 0x44, 0x44, 0x44);
                                break;
                        case SURF: path_element->notice_color = SColor(255,0x80,0xa6,0xFF);
                                break;
                        default: path_element->notice_color = SColor(255,0x69, 0x44, 0x89);
                }
                path.push_back(*path_element);
        }
        path_index = 0;
        moving = true;
        camera->removeAnimators();
        remove_edge_vector(ev);
}

void Station::moveHelicopter()
{
        char *edge_name;
        char node_name[255];
        wchar_t wnode_name[255];
        stringw notice_text;

        /* Movement */
        if (!path_element_animator || path_element_animator->hasFinished()) {
                if (path_element_animator) {
                        camera->removeAnimator(path_element_animator);
                        path_element_animator->drop();
                        path_element_animator = NULL;
                        path_index++;
                        /* Movement end detection */
                        if (path_index == path.size()) {
                                clearPath();
                                camera->addAnimator(fps_animator);
                                camera->addAnimator(collision_animator);
                                return ;
                        }
                }
                u32 time;
                time = (path[path_index].end_point - path[path_index].start_point).getLength() * 500;
                path_element_animator = m_smgr->createFlyStraightAnimator(path[path_index].start_point,
                                                                          path[path_index].end_point,
                                                                          time);
                camera->addAnimator(path_element_animator);

                /* Draw text at arrival point */
                getNodeLocation(node_name, path[path_index].node_id);
                mbstowcs (wnode_name, node_name, strlen(node_name) + 1);
                if (text_node) {
                        text_node->remove();
                        text_node = NULL;
                }
                text_node = m_smgr->addBillboardTextSceneNode(font, wnode_name);
                text_node->setPosition(path[path_index].end_point + (path[path_index].end_point - path[path_index].start_point).normalize()*1);
                text_node->setSize(dimension2d<f32>(0.3,0.2));
        }

        /* Notice Frame*/
        m_driver->draw2DRectangle(path[path_index].notice_color, path[path_index].new_notice_frame);

        /* Notice Text */
        if (path_index == 0) {
                getNodeLocation(node_name, path[path_index].node_id);
                node_name[0] = tolower(node_name[0]);
                notice_text = "Aller jusqu'";
                if (path[path_index].is_end_node_real_point)
                        notice_text += "a";
                notice_text += " ";
                notice_text += node_name;
        } else {
                edge_name = path[path_index].edge_name;
                notice_text = "Prendre ";
                notice_text += edge_name;
        }
        font->draw(notice_text, path[path_index].new_notice_text, SColor(255, 255, 255, 255));


        /* Camera re-orientation */
        vector3df end_point = path[path_index].end_point + (path[path_index].end_point - path[path_index].start_point).normalize()*10;
        f32 rotateSpeed = 0.2f;
        camera->updateAbsolutePosition();

        if (camera->getTarget().X < end_point.X - rotateSpeed)
                camera->setTarget(camera->getTarget() + vector3df(rotateSpeed,0,0));
        else if (camera->getTarget().X > end_point.X + rotateSpeed)
                camera->setTarget(camera->getTarget() - vector3df(rotateSpeed,0,0));

        if (camera->getTarget().Y < end_point.Y - rotateSpeed)
                camera->setTarget(camera->getTarget() + vector3df(0,rotateSpeed,0));
        else if (camera->getTarget().Y > end_point.Y + rotateSpeed)
                camera->setTarget(camera->getTarget() - vector3df(0,rotateSpeed,0));

        if (camera->getTarget().Z < end_point.Z - rotateSpeed)
                camera->setTarget(camera->getTarget() + vector3df(0,0,rotateSpeed));
        else if (camera->getTarget().Z > end_point.Z + rotateSpeed)
                camera->setTarget(camera->getTarget() - vector3df(0,0,rotateSpeed));
}


void Station::clearPath()
{
        path_index = 0;
        moving = false;
        if (text_node) {
                text_node->remove();
                text_node = NULL;
        }
        if (path.empty())
                return;
        path.clear();
        if (path_element_animator) {
                path_element_animator->drop();
                path_element_animator = NULL;
        }
        camera->removeAnimators();
}




/* STATION VIEW */
void Station::draw()
{
        /* Create fresh new video buffer */
        m_driver->beginScene();

        /* Draw the whole 3D scene to the buffer */
        if (!paused) {
                /* Usual view */
                if (intro_animator)
                        camera->setTarget(vector3df(5, 10, 30));
                m_smgr->drawAll();
        } else {
                /* Pause Menu with Blurry effect*/
                ppBlur->render(NULL);
                PauseMenu *pauseMenu = (PauseMenu *)m_device->getEventReceiver();
                pauseMenu->draw();

        }

        /* Draws the opening fader and kills it when finished and loads the pause menu controller instead */
        if (openingfader && openingfader->isReady()) {
                openingfader->remove();
                openingfader = NULL;

                /* Load Pause Menu toggler */
                PauseMenu *pauseMenu = new PauseMenu(this);
                m_device->setEventReceiver(pauseMenu);
        } else if (openingfader) {
                m_guienv->drawAll();
        }

        /* Handle helicopter movement along the requested path */
        if (shouldHelicopterMove())
                moveHelicopter();

        /* Swap buffer to screen */
        m_driver->endScene();

}

IGUIFont *Station::getFont()
{
        return font;
}

void Station::logCameraPosition()
{
        vector3df position = camera->getAbsolutePosition();
        std::cout << position.X << " " << position.Y << " " << position.Z << std::endl;
}

Station::~Station()
{
        m_smgr->clear();
        m_guienv->clear();
        delete (PauseMenu *)m_device->getEventReceiver();
        delete ppBlur;
        delete ppRenderer;
}
