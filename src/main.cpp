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

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    GLWidget *glwidget = new GLWidget();
    
    MainWindow *main = new MainWindow(glwidget);
    
    glwidget->show();
    main->show();
    
    app.exec();
}

