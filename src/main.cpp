//
//  main.cpp
//  RunParticles
//
//  Created by Doug Letterman on 9/27/13.
//
//

#include <QApplication>
#include "DemoPlayer.h"
#include "GLWidget.h"
#include "MainWindow.h"
#include "Map.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("RunParticles");
    app.setOrganizationDomain("renderfast.com");
    app.setOrganizationName("Renderfast");
    
    Map *map = new Map();
    
    GLWidget *glwidget = new GLWidget(map);
    
    MainWindow *main = new MainWindow(glwidget);
    
    new DemoPlayer(main, glwidget);
    
    glwidget->show();
    main->show();
    
    app.exec();
    
    delete main;
}

