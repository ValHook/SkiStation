/*
 *  Splashscreen.h
 *  Spaera
 *
 *  Created by Valentin on 27/12/2013.
 *  Copyright (c) 2013 Valentin Mercier. All rights reserved.
 */

#ifndef __Spaera__Splashscreen__
#define __Spaera__Splashscreen__

#include <irrlicht.h>


/* Little Splashscreen Class to display some images before your game
 * It does not handle transparency though
 */
class Splashscreen
{
private:
        /* Root Irrlicht device */
        irr::IrrlichtDevice *m_device;
        
        /* Key obkects */
        irr::video::IVideoDriver *m_driver;
        irr::gui::IGUIEnvironment *m_guienv;
        irr::ITimer *m_timer;
        
        /* Background color */
        irr::video::SColor m_backgroundColor;
        
        /* Vector of images to show */
        irr::core::array<irr::video::ITexture *> m_slides;
        
        /* Time during each image is being shown */
        irr::f32 m_waitBeforeStartTime, m_slideTime;
        
        /* Parameter for the image transisition
         * False will make images pop one by one
         * True will add a crossfade between every transition */
        bool m_crossfade;
        
        
public:
        /* Constructor 
         * Defaults parameters are 2 seconds for the slideTime
         * and no crossfade */
        Splashscreen(irr::IrrlichtDevice *device, irr::video::SColor backgroundColor =
                     irr::video::SColor(255,0,0,0), irr::f32 waitBeforeStartTime = 0, irr::f32 slideTime = 2000, bool crossfade = false);
        
        /* Adds a new image to the list
         * If the image cannot be found then it is not added */
        void addSlide(const irr::io::path &slidePath);
        
        /* Draws the splashscreens */
        void draw();
        
        /* Destructor */
        ~Splashscreen();
};

#endif /* defined(__Spaera__Splashscreen__) */
