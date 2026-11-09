//
//  main.cpp
//  RunParticles
//
//  Created by Doug Letterman on 9/27/13.
//
//

#include <QApplication>
#include <QSurfaceFormat>
#include "GLWidget.h"
#include "MainWindow.h"
#include "Map.h"

int main(int argc, char **argv)
{
    // The renderer needs geometry shaders, which on macOS means a core profile
    QSurfaceFormat format;
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    
    QApplication app(argc, argv);
    app.setApplicationName("RunParticles");
    app.setOrganizationDomain("renderfast.com");
    app.setOrganizationName("Renderfast");
    app.setApplicationVersion("1.01");
    
    MainWindow mainWindow;
    
    app.exec();
}

