# Code style

## The naming convention

|Files||
|-|-|
| Header files | `headerfile.h` |
| Source files | `sourcefile.cpp` |

|Code||
|-|-|
| Classes | `class UpperCamelCase` |
| Namespaces | `namespace lowerCamelCase` |
| Type definitions | `typedef oldType TNewType` |
| Type definitions (function pointer types only) | `typedef void (*PointerType)()` |
| Methods | `void lowerCamelCase(int t_lowerCamelCase)` |
| Qt Signals and Slots | `void lowerCamelCase()` |
| Public or protected fields | `int lowerCamelCase` |
| Private fields | `int m_lowerCamelCase` |
| Constants | `const int lowerCamelCase` |
| Variables | `int lowerCamelCase` |

## Style

### General
* Every header file must contain a compilation guard `#pragma once` at the top of the file.
* Curly brackets belong on separate lines
* We use spaces for indentation, 4 of them
* Use descriptive names, avoid abbreviations

### Type modifiers

Type modifiers `*` and `&` should cling to the type.

```cpp
string* name; // Good

string * name; // Bad

string *name; // Bad
```

### The license header

Every file must begin with license header
``` C++
/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/
```

## Errors

Launcher code is using exceptions to handle errors.
If something in launcher fails it should throw an exception. Exception and it's context will be saved to log file and will make debugging process easier.

## Logging

### Functions

There are four functions that can be used for creating log:

* `qDebug` - very deep informations, mostly contains raw data
* `qInfo` - describing launcher operations
* `qWarning` - describing failures from which launcher can be rescued
* `qCritical` - describing failures that makes launcher unable to work

The usage of these functions depends on a situation. To display a very shot and simple message with no arguments:

``` C++
qInfo("A short and concise message.");
```

The log strings can be formatted in a manner simmilar to `printf`

``` C++
const char* testMessage = "So long! And thanks for all the fish!";
int testInteger = 42;
qDebug("The answer to the ultimate question: %d", testInteger);
qCritical("%s \n So sad that it should come to this.", testMessage);
```

**However** for more complex messages and/or those that have to output a `QString`, a stream operator should be used.

**Note: This will require you to include the `QtDebug` header**

``` C++
QString name = "John";
unsigned int age = 32;

qInfo() << "Hi my name is  " << name << ", I'm " << 32;
```

In situations when the resulting code line is too long, a following format is encouraged:

``` c++
qInfo() << "Some string: "
        << someVariable
        << " even more string"
        << andSoOn
        << " and so on "
        << adInfinitum;
```

### Rules

* Every important operation should start with log describing it's purpose but it shouldn't inform about it's result. For example:
``` C++
bool loadDataFile()
{
    qInfo() << "Loading data file from " << m_dataFilePath;
    ...
}
```
* Log can display result of function. For example:
``` C++
if(loadDataFile())
{
    qInfo("Loaded data file.");
    ...
}
else
{
    qWarning("Failed to load data file.");
    ...
}
```
* Use `qCritical` to display exception description. For example:
``` C++
try
{
    ...
}
catch(LauncherException &exception)
{
    qCritical(exception.what());
    ...
}
```
