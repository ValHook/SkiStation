/*
 *  PauseMenu.h
 *  Station3D
 *
 *  Created by Valentin on 16/02/2014.
 *  Copyright (c) 2014 Valentin Mercier. All rights reserved.
 */

#ifndef __Station3D__PauseMenu__
#define __Station3D__PauseMenu__

#include <iostream>
#include <irrlicht.h>
#include "Station.h"

/* Some enums */
enum EGUIButtonType
{
        SELECT_PATH_BUTTON,
        SELECT_PATH_CONFIRM_BUTTON,
        CUSTOMIZE_BUTTON,
        CUSTOMIZE_CONFIRM_BUTTON,
        CREDITS_BUTTON,
        QUIT_BUTTON
};

enum EPauseMenuView
{
        DEFAULT,
        CREDITS,
        CUSTOMIZE,
        SELECT_PATH
};

/* Pause Menu class */
class PauseMenu : public irr::IEventReceiver
{
private:
        Station *m_station;
        irr::video::IVideoDriver *m_driver;
        irr::gui::IGUIEnvironment *m_guienv;
        irr::core::dimension2d<irr::u32> m_deskres;
        
        enum EPauseMenuView view;
        irr::core::array<irr::video::ITexture *> buttonTextures;
        irr::gui::IGUIEditBox *editBox[NUMBER_OF_EDGE_TYPES - 1];
        irr::gui::IGUIWindow* errorwindow;
        irr::gui::IGUIListBox* listfrom, *listto, *listfilter;
        
        irr::gui::IGUIFont *font;
        irr::s32 extension;
public:
        PauseMenu(Station *station);
        void preparePauseMenu();
        void toggleView(enum EPauseMenuView new_view);
        void draw();
        bool OnEvent(const irr::SEvent& event);
        ~PauseMenu();
};

#endif /* defined(__Station3D__PauseMenu__) */
