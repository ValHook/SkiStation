/*
 *  irrlichtLauncher.cpp
 *  Spaera
 *
 *  Created by Valentin on 27/12/2013.
 *  Copyright (c) 2013 Valentin Mercier. All rights reserved.
 */
#include "irrlichtLauncher.h"
#include "Graph Functions/libsaferesource.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

/* Creates a full screen and full resolution Open GL based Irrlicht device according to the user's screen size
 * A string parameter for the software title is passed */
IrrlichtDevice *createFullScreenDevice(const wchar_t *title, dimension2d<u32> minimumSize, IEventReceiver *event)
{
        /* Get the screen resollution using a null device (Irrlicht trick) */
        IrrlichtDevice *nulldevice = createDevice(EDT_NULL);
        if (!nulldevice)
                die("Cannot create Irrlicht device");
        dimension2d<u32> deskres = nulldevice->getVideoModeList()->getDesktopResolution();

        /* Check screen size */
        if (deskres.Width < minimumSize.Width || deskres.Height < minimumSize.Height)
                die("Screen too small. Minimum required %dx%d", minimumSize.Width, minimumSize.Height);

        /* Delete the nulldevice */
        nulldevice->closeDevice();

        /* Now the dimensions can be used to create the real device */
        IrrlichtDevice *device = createDevice(EDT_OPENGL, deskres, 32, true, true, true, event);
        if (!device)
                die("Cannot create Irrlicht device");
        device->setWindowCaption(title);
        return device;
}
