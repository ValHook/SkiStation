/*
 *  Splashscreen.cpp
 *  Spaera
 *
 *  Created by Valentin on 27/12/2013.
 *  Copyright (c) 2013 Valentin Mercier. All rights reserved.
 */

#include "Splashscreen.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;



/* Constructor */
Splashscreen::Splashscreen(IrrlichtDevice *device, SColor backgroundColor, f32 waitBeforeStartTime, f32 slideTime, bool crossfade)
{
        m_device = device;
        m_driver = device->getVideoDriver();
        m_guienv = device->getGUIEnvironment();
        m_timer = device->getTimer();
        
        m_backgroundColor = backgroundColor;
        m_waitBeforeStartTime = waitBeforeStartTime;
        m_slideTime = slideTime;
        m_crossfade = crossfade;
}


/* Adds a new image to the list
 * If the image cannot be found then it is not added */
void Splashscreen::addSlide(const path &slidePath)
{
        /* Attempt to load the slide */
        ITexture *newSlide = m_driver->getTexture(slidePath);
        
        /* If it worked then we add it to the vector */
        if (newSlide)
                m_slides.push_back(newSlide);
}


/* Draws the splashscreens */
void Splashscreen::draw()
{
        /* Get the screen dimensions
         * This will be used later for centering the images */
        dimension2d<u32> deskres = m_device->getVideoModeList()->getDesktopResolution();
        
        /* Get the number of images to draw */
        u32 n = m_slides.size();
        
        /* Get the current time at the beginning of the display */
        u32 beginTime = m_timer->getTime() + m_waitBeforeStartTime;
        u32 deadline = beginTime + (n+0.5) * m_slideTime;
        
        /* Temporary unsigned integers for determinig what image to show */
        s32 old_i = -1, i = 0;
        
        /* Crossfader objectd, will be used if corssfade option is enabled */
        IGUIInOutFader* crossfader = NULL, *endfader = NULL;


        
        /* Main Loop */
        while (m_device->run() && m_timer->getTime() < deadline) {
                /* Clear buffer with the background color */
                m_driver->beginScene(true, true, m_backgroundColor);
                
                /* Find the right image to draw */
                i = (m_timer->getTime() - beginTime) / m_slideTime;

                /* When the last image has shown up we show an ending fader if the option is enabled */
                if (i == (s32)m_slides.size()) {
                        i--;
                        if (!endfader && m_crossfade) {
                                endfader = m_guienv->addInOutFader();
                                endfader->setColor(m_backgroundColor);
                                endfader->fadeOut(m_slideTime / 2);
                        }
                }
                
                /* If the slideshow has begun */
                if (m_timer->getTime() >= beginTime) {
                        /* Get the image dimensions */
                        dimension2d<u32> dimensions = m_slides[i]->getSize();
                
                        /* Determine the image position so that it is centered */
                        vector2d<s32> slidePosition;
                        slidePosition.X = (deskres.Width - dimensions.Width) / 2;
                        slidePosition.Y = (deskres.Height - dimensions.Height) / 2;
                
                        /* Draw the image */
                        m_driver->draw2DImage(m_slides[i], slidePosition);
                
                        /* If the crossfade option is enabled and the image changed
                         * We create a new fader */
                        if (m_crossfade && old_i != i) {
                                crossfader = m_guienv->addInOutFader();
                                crossfader->setColor(m_backgroundColor);
                                crossfader->fadeIn(m_slideTime / 2);
                        }
                        /* If the fader is finished we remove it */
                        if (m_crossfade && crossfader && crossfader->isReady()) {
                                crossfader->remove();
                                crossfader = NULL;
                        }
                }
                
                old_i = i;
                
                
                /* Swap buffer to screen */
                m_guienv->drawAll();
                m_driver->endScene();
        }
        
        /* This loop will stop when all images would have been drawn */
        endfader->remove();
}


/* Destructor */
Splashscreen::~Splashscreen()
{
        /* Removes the textures allocated */
        for (u32 i = 0; i < m_slides.size(); i++)
                m_driver->removeTexture(m_slides[i]);
        m_slides.clear();
}
