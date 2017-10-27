/*
 *  station.h
 *  Station3D
 *
 *  Created by Valentin on 16/02/2014.
 *  Copyright (c) 2014 Valentin Mercier. All rights reserved.
 */

#ifndef __Station3D__station__
#define __Station3D__station__

#include <irrlicht.h>
#include "Shaders Effects/CRendererPostProc.h"
#include "Shaders Effects/CEffectPostProc.h"
#include "Shaders Effects/CTransitionPostProc.h"
#include "Shaders Effects/CSplitPostProc.h"
#include "Shaders Effects/CLensFlarePostProc.h"
#include "Shaders Effects/CWaterPostProc.h"


#include "Graph Functions/domain.h"

/* Quick struct that will hold data for the movement along the path requested */
struct SPathElement
{
        int is_end_node_real_point, edge_id, node_id;
        irr::core::vector3df start_point, end_point;
        irr::video::SColor notice_color;
        irr::core::rect<irr::s32> new_notice_frame, old_notice_frame, new_notice_text, old_notice_text;
        char *edge_name, *node_name;
};


/* Station controlling class */
class Station
{
private:
        domain_t *m_domain;
        irr::IrrlichtDevice *m_device;
        irr::scene::ISceneManager *m_smgr;
        irr::video::IVideoDriver *m_driver;
        irr::gui::IGUIEnvironment *m_guienv;
        irr::ITimer *m_timer;
        IPostProc* ppRenderer;
        CEffectPostProc* ppBlur;
        
        irr::video::ITexture *land, *land_bw, *tree, *tree_bw, *sky, *sky_bw;
        irr::scene::ICameraSceneNode *camera;
        irr::scene::IMeshSceneNode *mountain, *trees, *coptera, *copterb, *rotora, *rotorb;
        irr::scene::ISceneNodeAnimator *intro_animator, *collision_animator, *rotoranimatora, *rotoranimatorb;
        irr::scene::ISceneNodeAnimatorCameraFPS *fps_animator;
        
        irr::scene::ILightSceneNode *sun;
        irr::scene::ISceneNode *skydome, *skydome_bw;
        
        irr::gui::IGUIInOutFader* openingfader;
        bool wireframe, paused, ended;
        
        irr::core::array<struct SPathElement> path;
        irr::u32 path_index;
        bool moving;
        irr::scene::ISceneNodeAnimator *path_element_animator;
        irr::scene::IBillboardTextSceneNode *text_node;
        
        irr::gui::IGUIFont *font;
public:
        Station(irr::IrrlichtDevice *device, domain_t *domain);
        irr::IrrlichtDevice *getDevice();
        node_vector *getNodes();
        edge_vector *getEdges();
        void getNodeLocation(char location[255], int id);
        int *getRouteTimes();
        void update_route_times(int route_times[NUMBER_OF_EDGE_TYPES -1]);
        bool isPaused();
        bool isEnded();
        void togglePauseMenu();
        void toggleWireframe();
        void end();
        void startHelicopterMovement(irr::s32 source, irr::s32 destination, irr::s32 mode);
        bool shouldHelicopterMove();
        void moveHelicopter();
        void clearPath();
        irr::gui::IGUIFont *getFont();
        void draw();
        void logCameraPosition();
        ~Station();
};

#endif
