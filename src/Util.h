
#include <QString>
#include <QDataStream>
#include <QString>
#include <QGLShader>
#include <QGLShaderProgram>

namespace Util
{
    unsigned int parseTime(const char *timeCStr);
    
    QString secondsToString(int seconds);
    
    void loadGLSLShader(QGLShader::ShaderType, const QString&,
                        QGLShaderProgram*);
};

