#ifndef SINGLETON_H
#define SINGLETON_H

// from http://www.qtcentre.org/wiki/index.php?title=Singleton_pattern

#include <QObject>
 
template <class T>
class Singleton
{
public:
    static T* Instance()
    {
        static T* _instance;
        if (!_instance)
            _instance = new T();
        return _instance;   // return it
    }
 
private:
    Singleton();	// hide constructor
    ~Singleton();	// hide destructor
    Singleton(const Singleton &); // hide copy constructor
    Singleton& operator=(const Singleton &); // hide assign op
};

#endif

