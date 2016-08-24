/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "executableresources.h"

#include "exception.h"

#ifdef Q_OS_WIN

#include <tchar.h>
#include <Windows.h>

std::shared_ptr<QByteArray> ExecutableResources::extract(const QString& t_executableName, int t_resourceId, int t_resourceTypeId)
{
    HMODULE executableHandle = LoadLibraryA(t_executableName.toStdString().c_str());

    if (executableHandle == nullptr)
    {
        throw Exception("Cannot extract resource.");
    }

    HRSRC resource = FindResource(executableHandle, MAKEINTRESOURCE(t_resourceId), MAKEINTRESOURCE(t_resourceTypeId));

    if (resource == nullptr)
    {
        throw Exception("Cannot extract resource.");
    }

    HGLOBAL resourceMemory = LoadResource(executableHandle, resource);

    if (resourceMemory == nullptr)
    {
        throw Exception("Cannot extract resource.");
    }

    DWORD resourceBufferSize = SizeofResource(executableHandle, resource);
    void* resourceMemoryBuffer = LockResource(resourceMemory);

    if (resourceMemoryBuffer == nullptr || resourceBufferSize < 1)
    {
        throw Exception("Cannot extract resource.");
    }

    std::shared_ptr<QByteArray> resourceBuffer(new QByteArray(new char[resourceBufferSize], resourceBufferSize));

    memcpy(resourceBuffer->data(), resourceMemoryBuffer, resourceBufferSize);

    return resourceBuffer;
}

#endif
