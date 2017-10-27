#include "Graph Functions/domain.h"
#include "irrlichtLauncher.h"
#include "Splashscreen.h"
#include "Station.h"
#include "PauseMenu.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

int main()
{
        /* VERIFY SOFTWARE RESOURCES */
        /* Perform checksum verification before */
        char data_arcs[] = "data_arcs.txt";
        unsigned char checksum_data_arcs[20] =
        {0xB6, 0xC1, 0xB5, 0x84, 0x93, 0xA7, 0xF5, 0xE8, 0xF7, 0x9E,
        0xAF, 0x87, 0x42, 0x8D, 0x35, 0xB7, 0xA4, 0x57, 0x43, 0xF8};
        sha1_file_check(data_arcs, checksum_data_arcs);


        char payload_name[] = "payload.bin";
        unsigned char checksum_payload[20] =
        {0x15, 0x9D, 0x9B, 0x19, 0xB4, 0xCF, 0xF4, 0x49, 0x89, 0xAA,
        0x00, 0x3A, 0x61, 0xAA, 0x6A, 0x6B, 0xDF, 0x96, 0x0C, 0xC8};
        sha1_file_check(payload_name, checksum_payload);


        /* BEGIN SOFTWARE */
        /* Create the Irrlicht root object which will then be used for everything related to Irrlicht */
        IrrlichtDevice *device = createFullScreenDevice(L"Station 3D", /* Minimum Scrren Size */dimension2d<u32>(982, 740));
        device->getCursorControl()->setVisible(false);

        /* Load domain */
        domain_t *domain = create_domain(data_arcs);

        /* Open resources directory */
        device->getFileSystem()->addFileArchive(payload_name);


        /* Load some splashscreens */
        Splashscreen *splsh = new Splashscreen(device, SColor(255, 255, 255, 255), 1500, 3500, true);
        splsh->addSlide("ECE-logo.jpg");
        splsh->draw();
        delete splsh;


        /* Load Tri dimensional Station */
        Station *station = new Station(device, domain);

        /* Main Loop */
        while (device->run() && !station->isEnded())
                station->draw();

        /* Remove allocated data */
        delete station;
        unload_domain(domain);
        device->closeDevice(); /* Whole irrlicht deallocation, scene, guienv etc ... */

        /* Memory allocation feedback */
        /* show_refcounter(); */

        return 0;
}
