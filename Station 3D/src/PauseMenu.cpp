/*
 *  PauseMenu.cpp
 *  Station3D
 *
 *  Created by Valentin on 16/02/2014.
 *  Copyright (c) 2014 Valentin Mercier. All rights reserved.
 */

#include "PauseMenu.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

PauseMenu::PauseMenu(Station *station)
{
        m_station = station;
        IrrlichtDevice *device = m_station->getDevice();
        m_driver = device->getVideoDriver();
        m_guienv = device->getGUIEnvironment();
        m_deskres = device->getVideoModeList()->getDesktopResolution();

        buttonTextures.push_back(m_driver->getTexture("button-quit.png"));
        buttonTextures.push_back(m_driver->getTexture("button-credits.png"));
        buttonTextures.push_back(m_driver->getTexture("button-customize.png"));
        buttonTextures.push_back(m_driver->getTexture("button-path.png"));
        buttonTextures.push_back(m_driver->getTexture("button-customize-confirm.png"));

	font = m_station->getFont();

        view = DEFAULT;
        extension = 0;
        errorwindow = NULL;
        listfrom = NULL;
        listto = NULL;
        listfilter = NULL;
        preparePauseMenu();
}

bool PauseMenu::OnEvent(const SEvent& event)
{
        /* Keyboard detection */
        if (event.EventType == EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown) {
                switch (event.KeyInput.Key) {
                case KEY_ESCAPE: m_station->togglePauseMenu();
                                return true;
                case KEY_KEY_W:
                case KEY_KEY_Z: m_station->toggleWireframe();
                                return true;
                case KEY_KEY_L: m_station->logCameraPosition();
                                return true;
                default: ;
                }
        }

        /* Button detection */
        if (event.EventType == EET_GUI_EVENT && event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
                s32 id = event.GUIEvent.Caller->getID();
                switch (id) {
                case QUIT_BUTTON:
                {
                        m_station->end();
                        return true;
                }
                case CREDITS_BUTTON:
                {
                        toggleView(CREDITS);
                        return true;
                }
                case CUSTOMIZE_BUTTON:
                {
                        toggleView(CUSTOMIZE);
                        return true;
                }
                case CUSTOMIZE_CONFIRM_BUTTON: /* Checks wether specified times are correct or not */
                {
                        int times[NUMBER_OF_EDGE_TYPES - 1];
                        bool has_real_values = true;
                        for (s32 i = 0; i < NUMBER_OF_EDGE_TYPES - 1; i++) {
                                times[i] = 0;
                                swscanf(editBox[i]->getText(),L"%d", &times[i]);
                                if (times[i] <= 0)
                                        has_real_values = false;
                        }
                        /* If not valid, we show an error window */
                        if (has_real_values == false  && !errorwindow) {
                                errorwindow = m_guienv->addWindow(
                                                                rect<s32>(0.5 * m_deskres.Width - 30 - 201, 700, 0.5 * m_deskres.Width + 30 + 201, 800),
                                                                false, // modal?
                                                                L"Les temps saisis sont invalides");

                                m_guienv->addStaticText(L"Veuillez préciser uniquement des valeurs correctes",
                                                rect<s32>(35,35,550,60),
                                                false, // border?
                                                false, // wordwrap?
                                                errorwindow);
                        } else {
                        /* Otherwise we update the route times */
                                m_station->update_route_times(times);
                                toggleView(CUSTOMIZE);
                        }
                        return true;
                }
                case SELECT_PATH_BUTTON:
                {
                        toggleView(SELECT_PATH);
                        return true;
                }
                case SELECT_PATH_CONFIRM_BUTTON: /* Checks path the asked is correct or not */
                {
                        if (errorwindow) {
                                errorwindow->remove();
                                errorwindow = NULL;
                        }
                        if (listfrom->getSelected() == -1 || listto->getSelected() == -1 || listfilter->getSelected() == -1) {
                                /* Case when some elements are not selected */
                                errorwindow = m_guienv->addWindow(
                                                                  rect<s32>(0.5 * m_deskres.Width + 30, 470, 0.5 * m_deskres.Width + 490, 570),
                                                                  false, // modal?
                                                                  L"L'itinéraire demandé est incomplet");

                                m_guienv->addStaticText(L"Veuillez vérifier préciser chaque paramètre",
                                                        rect<s32>(35,35,400,60),
                                                        false, // border?
                                                        false, // wordwrap?
                                                        errorwindow);
                                return true;
                        } else if (!wcslen(listfrom->getListItem(listfrom->getSelected())) || !wcslen(listto->getListItem(listto->getSelected()))) {
                                /* Case when some elements are not selected */
                                errorwindow = m_guienv->addWindow(
                                                                  rect<s32>(0.5 * m_deskres.Width + 30, 470, 0.5 * m_deskres.Width + 490, 570),
                                                                  false, // modal?
                                                                  L"L'itinéraire demandé est incomplet");

                                m_guienv->addStaticText(L"Veuillez vérifier préciser chaque paramètre",
                                                        rect<s32>(35,35,400,60),
                                                        false, // border?
                                                        false, // wordwrap?
                                                        errorwindow);
                                return true;
                        } else if (listfrom->getSelected() == listto->getSelected()) {
                                /* Case when source and destination points are the same */
                                errorwindow = m_guienv->addWindow(
                                                                  rect<s32>(0.5 * m_deskres.Width + 30, 470, 0.5 * m_deskres.Width + 490, 570),
                                                                  false, // modal?
                                                                  L"L'itinéraire demandé est incorrect");

                                m_guienv->addStaticText(L"Les points de départ et d'arrivée sont identiques",
                                                        rect<s32>(35,35,400,60),
                                                        false, // border?
                                                        false, // wordwrap?
                                                        errorwindow);
                                return true;
                        } else {
                                /* Otherwise we start showing the path */
                                /* WARNING THIS VERY ORDER MUST BE KEPT,  or serious collision bugs and crasehs will occur */
                                m_station->togglePauseMenu();
                                m_station->startHelicopterMovement(listfrom->getSelected(), listto->getSelected(), listfilter->getSelected());
                                toggleView(SELECT_PATH);
                                return true;
                        }
                }
                default: ;
                }
        }

        /* Error window closed detection */
        if (event.EventType == EET_GUI_EVENT && event.GUIEvent.EventType == EGET_ELEMENT_CLOSED) {
                errorwindow->remove();
                errorwindow = NULL;
                return true;
        }
        return false;
}














void PauseMenu::preparePauseMenu()
{
        m_guienv->addButton(rect<s32>(0.5 * m_deskres.Width + 290,150,0.5 * m_deskres.Width + 290 + 201,201), 0, QUIT_BUTTON)->setImage(buttonTextures[0]);    /* Quit Button */
        m_guienv->addButton(rect<s32>(0.5 * m_deskres.Width + 30,150,0.5 * m_deskres.Width + 30 + 201,201), 0, CREDITS_BUTTON)->setImage(buttonTextures[1]);   /* Credits Button */
        m_guienv->addButton(rect<s32>(0.5 * m_deskres.Width - 30 - 201,150,0.5 * m_deskres.Width - 30,201), 0, CUSTOMIZE_BUTTON)->setImage(buttonTextures[2]); /* Customize Button */
        m_guienv->addButton(rect<s32>(0.5 * m_deskres.Width - 290 - 201,150,0.5 * m_deskres.Width - 290,201), 0, SELECT_PATH_BUTTON)->setImage(buttonTextures[3]);/* Path Button */
}

void PauseMenu::toggleView(enum EPauseMenuView new_view)
{
        /* Clear old view */
        if (errorwindow) {
                errorwindow->remove();
                errorwindow = NULL;
        }
        if (listfrom) {
                listfrom->remove();
                listfrom = NULL;
        }
        if (listto) {
                listto->remove();
                listto = NULL;
        }
        if (listfilter) {
                listfilter->remove();
                listfilter = NULL;
        }
        m_guienv->clear();
        preparePauseMenu();

        /* Set up new view */
        /* We define here only the new GUI Elements, text and background rectangle are drawn from the draw method */
        view = (view != new_view) ? new_view : DEFAULT;
        switch (view) {
        case DEFAULT: extension = 0;
                        break;
        case CREDITS: extension = 50;
                        break;
        case CUSTOMIZE:
        {
                        extension = 400;
                        int *route_times = m_station->getRouteTimes();
                        wchar_t time[NUMBER_OF_EDGE_TYPES - 1][11];
                        for (s32 i = 0; i < NUMBER_OF_EDGE_TYPES - 1; i++)
                                swprintf(time[i], 10, L"%d", route_times[i]);
                        delete route_times;
                        editBox[0] = m_guienv->addEditBox(time[0], rect<s32>(0.5 * m_deskres.Width - 231, 310, 0.5 * m_deskres.Width - 30, 330));
                        editBox[1] = m_guienv->addEditBox(time[1], rect<s32>(0.5 * m_deskres.Width - 231, 360, 0.5 * m_deskres.Width - 30, 380));
                        editBox[2] = m_guienv->addEditBox(time[2], rect<s32>(0.5 * m_deskres.Width - 231, 410, 0.5 * m_deskres.Width - 30, 430));
                        editBox[3] = m_guienv->addEditBox(time[3], rect<s32>(0.5 * m_deskres.Width - 231, 460, 0.5 * m_deskres.Width - 30, 480));
                        editBox[4] = m_guienv->addEditBox(time[4], rect<s32>(0.5 * m_deskres.Width - 231, 510, 0.5 * m_deskres.Width - 30, 530));
                        editBox[5] = m_guienv->addEditBox(time[5], rect<s32>(0.5 * m_deskres.Width - 231, 560, 0.5 * m_deskres.Width - 30, 580));

                        editBox[6] = m_guienv->addEditBox(time[6], rect<s32>(0.5 * m_deskres.Width + 30, 310, 0.5 * m_deskres.Width + 231, 330));
                        editBox[7] = m_guienv->addEditBox(time[7], rect<s32>(0.5 * m_deskres.Width + 30, 360, 0.5 * m_deskres.Width + 231, 380));
                        editBox[8] = m_guienv->addEditBox(time[8], rect<s32>(0.5 * m_deskres.Width + 30, 410, 0.5 * m_deskres.Width + 231, 430));
                        editBox[9] = m_guienv->addEditBox(time[9], rect<s32>(0.5 * m_deskres.Width + 30, 460, 0.5 * m_deskres.Width + 231, 480));
                        editBox[10] = m_guienv->addEditBox(time[10], rect<s32>(0.5 * m_deskres.Width + 30, 510, 0.5 * m_deskres.Width + 231, 530));
                        m_guienv->addButton(rect<s32>(0.5 * m_deskres.Width + 30, 560, 0.5 * m_deskres.Width + 231, 581), 0, CUSTOMIZE_CONFIRM_BUTTON)
                        ->setImage(buttonTextures[4]);
                        break;
        }
        case SELECT_PATH:
        {
                        extension = 550;
                        listfrom = m_guienv->addListBox(rect<s32>(0.5 * m_deskres.Width - 290 - 201,350,0.5 * m_deskres.Width - 290,701));
                        listto = m_guienv->addListBox(rect<s32>(0.5 * m_deskres.Width - 30 - 201,350,0.5 * m_deskres.Width - 30,701));
                        node_vector *nv = m_station->getNodes();

                        wchar_t name[255];
                        char node_location[255];
                        for (s32 i = 0; i < nv->length; i++) {

                                node_location[0] = '\0';
                                m_station->getNodeLocation(node_location, i);
                                mbstowcs (name, node_location, strlen(node_location) + 1);

                                listfrom->insertItem(i, name, -1);
                                listfrom->setItemOverrideColor(i, SColor(255,255,255,255));
                                listto->insertItem(i, name, -1);
                                listto->setItemOverrideColor(i, SColor(255,255,255,255));
                        }

                        listfilter = m_guienv->addListBox(rect<s32>(0.5 * m_deskres.Width + 30,350,0.5 * m_deskres.Width + 30 + 201,430));
                        listfilter->insertItem(0, L"Plus Rapide", -1);
                        listfilter->setItemOverrideColor(0, SColor(255,255,255,255));
                        listfilter->insertItem(1, L"Plus Facile", -1);
                        listfilter->setItemOverrideColor(1, SColor(255,255,255,255));
                        listfilter->insertItem(2, L"Plus Joli", -1);
                        listfilter->setItemOverrideColor(2, SColor(255,255,255,255));

                        m_guienv->addButton(rect<s32>(0.5 * m_deskres.Width + 290, 350,0.5 * m_deskres.Width + 290 + 201,371), 0, SELECT_PATH_CONFIRM_BUTTON)
                        ->setImage(buttonTextures[4]);

                        break;
        }
        default: extension = 0;
        }
}










void PauseMenu::draw()
{
        /* Draw Background Rectangle (Black) */
        m_driver->draw2DRectangle(SColor(255,0,0,0), rect<s32>(0, 100, m_deskres.Width, 250 + extension));

        /* Draw texts */
        switch (view) {
        case CREDITS: font->draw(L"Antoine Loret & Valentin Mercier | ECE Paris | 2014",
                           rect<s32>(0.5 * m_deskres.Width - 150,250, 0.5 * m_deskres.Width + 150,280),
                           SColor(255,255,255,255));
                        break;
        case CUSTOMIZE: font->draw(L"Personnalisez votre rythme de ski, précisez en secondes le temps pris pour 100 metres de dénivelé !",
                                   rect<s32>(0.5 * m_deskres.Width - 300,250, 0.5 * m_deskres.Width + 300,280),
                                   SColor(255,255,255,255));
                        font->draw(L"Pistes Vertes",
                                   rect<s32>(0.5 * m_deskres.Width - 378, 310, 0.5 * m_deskres.Width - 290, 330),
                                   SColor(255,255,255,255));
                        font->draw(L"Pistes Bleues",
                                   rect<s32>(0.5 * m_deskres.Width - 378, 360, 0.5 * m_deskres.Width - 290, 380),
                                   SColor(255,255,255,255));
                        font->draw(L"Pistes Rouges",
                                   rect<s32>(0.5 * m_deskres.Width - 383, 410, 0.5 * m_deskres.Width - 290, 430),
                                   SColor(255,255,255,255));
                        font->draw(L"Pistes Noires",
                                   rect<s32>(0.5 * m_deskres.Width - 379, 460, 0.5 * m_deskres.Width - 290, 480),
                                   SColor(255,255,255,255));
                        font->draw(L"Piste de Kilomètre lancé",
                                   rect<s32>(0.5 * m_deskres.Width - 447, 510, 0.5 * m_deskres.Width - 290, 530),
                                   SColor(255,255,255,255));
                        font->draw(L"Snowparks",
                                   rect<s32>(0.5 * m_deskres.Width - 365, 560, 0.5 * m_deskres.Width - 290, 580),
                                   SColor(255,255,255,255));

                        font->draw(L"Téléphériques",
                                   rect<s32>(0.5 * m_deskres.Width + 290, 310, 0.5 * m_deskres.Width + 490, 330),
                                   SColor(255,255,255,255));
                        font->draw(L"Télécabines",
                                   rect<s32>(0.5 * m_deskres.Width + 290, 360, 0.5 * m_deskres.Width + 490, 380),
                                   SColor(255,255,255,255));
                        font->draw(L"Télésièges drebayables",
                                   rect<s32>(0.5 * m_deskres.Width + 290, 410, 0.5 * m_deskres.Width + 490, 430),
                                   SColor(255,255,255,255));
                        font->draw(L"Télésièges simples",
                                   rect<s32>(0.5 * m_deskres.Width + 290, 460, 0.5 * m_deskres.Width + 490, 480),
                                   SColor(255,255,255,255));
                        font->draw(L"Téléskis",
                                   rect<s32>(0.5 * m_deskres.Width + 290, 510, 0.5 * m_deskres.Width + 490, 530),
                                   SColor(255,255,255,255));
                        break;
        case SELECT_PATH: font->draw(L"Précisez votre itinéraire",
                                     rect<s32>(0.5 * m_deskres.Width - 100,250, 0.5 * m_deskres.Width + 100,280),
                                     SColor(255,255,255,255));
                        font->draw(L"Lieu de départ",
                                   rect<s32>(0.5 * m_deskres.Width - 290 - 201,310,0.5 * m_deskres.Width - 290,345),
                                   SColor(255,105,105,105));
                        font->draw(L"Lieu d'arrivée",
                                   rect<s32>(0.5 * m_deskres.Width - 30 - 201,310,0.5 * m_deskres.Width - 30,345),
                                   SColor(255,105,105,105));
                        font->draw(L"Préférence",
                                   rect<s32>(0.5 * m_deskres.Width + 30,310,0.5 * m_deskres.Width + 30 + 201,345),
                                   SColor(255,105,105,105));
                        font->draw(L"Valider",
                                   rect<s32>(0.5 * m_deskres.Width + 290,310,0.5 * m_deskres.Width + 290 + 201,345),
                                   SColor(255,105,105,105));
                        break;
        default: ;
        }

         /* GUI Elements are already created from prepareMenu() or toggleView
          * We then draw them here */
        m_guienv->drawAll();
}

PauseMenu::~PauseMenu()
{
        /* Removes the textures allocated */
        for (u32 i = 0; i < buttonTextures.size(); i++)
                m_driver->removeTexture(buttonTextures[i]);
        buttonTextures.clear();

        /* Remove the buttons and everything that is gui related */
        m_guienv->clear();

}
