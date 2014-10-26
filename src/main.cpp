//
//  main.cpp
//  RunParticles
//
//  Created by Doug Letterman on 9/27/13.
//
//

#include <QApplication>
#include "GLWidget.h"
#include "MainWindow.h"
#include "Map.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("RunParticles");
    app.setOrganizationDomain("renderfast.com");
    app.setOrganizationName("Renderfast");
    
    MainWindow mainWindow;
    
    mainWindow.show();
    
    app.exec();
}

