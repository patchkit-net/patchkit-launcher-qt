#ifndef LAUNCHERCANCELLEDEXCEPTION_H
#define LAUNCHERCANCELLEDEXCEPTION_H

#include "launcherexception.h"

class LauncherCancelledException : public LauncherException
{
public:
    LauncherCancelledException();
};

#endif // LAUNCHERCANCELLEDEXCEPTION_H
