#include <stdio.h>
#include <iostream>

#include <X11/Xlib.h> // sudo apt-get install libx11-dev

#include <GL/gl.h>
#include <GL/glx.h>

//#include "ogl.h"

void Test()
{
    GLXContext context;
}

int main() {
    Display* display = XOpenDisplay(NULL);

    int screen = DefaultScreen(display);

    Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 100, 100, 1, BlackPixel(display, screen), WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(display, window);

    XEvent event;

    bool go = true;

    Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1);

    while (go) {
        XNextEvent(display, &event);

        switch (event.type)
        {
        case ConfigureNotify:
            fprintf(stdout, "Configure notification\n");
            break;
        case DestroyNotify:
            go = false;
            fprintf(stdout, "Destroy notification\n");
            break;
        case KeyPress:
            go = false;
            break;
        case ClientMessage:
            if (static_cast<unsigned int>(event.xclient.data.l[0]) == WM_DELETE_WINDOW) {
                go = false;
                fprintf(stdout, "WM_DELETE_WINDOW\n");
            }
            break;
        }
    }

    XCloseDisplay(display);

    return 0;
}