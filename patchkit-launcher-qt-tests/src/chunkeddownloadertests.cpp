/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QtNetwork>
#include <QApplication>

#include "src/chunkeddownloader.h"
#include "src/timeoutexception.h"
#include "src/contentsummary.h"
#include "src/staledownloadexception.h"

#include "mockednam.h"
#include "custommacros.h"
