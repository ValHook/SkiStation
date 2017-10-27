/*
 *  irrlichtLauncher.h
 *  Spaera
 *
 *  Created by Valentin on 27/12/2013.
 *  Copyright (c) 2013 Valentin Mercier. All rights reserved.
 */

#ifndef __Spaera__irrlichtLauncher__
#define __Spaera__irrlichtLauncher__

#include <irrlicht.h>

irr::IrrlichtDevice *createFullScreenDevice(const wchar_t *title = L"\0",
                                            irr::core::dimension2d<irr::u32> minimumSize = irr::core::dimension2d<irr::u32>(0,0),
                                            irr::IEventReceiver *event = NULL);

#endif /* defined(__Spaera__irrlichtLauncher__) */

