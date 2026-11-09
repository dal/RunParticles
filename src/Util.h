
#include <QString>
#include <QDataStream>
#include <QString>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

namespace Util
{
    unsigned int parseTime(const char *timeCStr);
    
    QString secondsToString(int seconds);
    
    void loadGLSLShader(QOpenGLShader::ShaderType, const QString&,
                        QOpenGLShaderProgram*);
};

