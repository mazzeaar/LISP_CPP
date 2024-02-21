#include "validation.h"

int checkArgsIs(const std::string& name, int expected, int got)
{
    if ( got != expected ) {
        throw ArgsError("\"" + name + "\" expects "
            + std::to_string(expected) + " args, "
            + std::to_string(got) + " supplied");
    }

    return got;
}

int checkArgsBetween(const std::string& name, int min, int max, int got)
{
    if ( got < min || got > max ) {
        throw ArgsError("\"" + name, "\" expects between " + std::to_string(min)
            + " and " + std::to_string(max) + " args, "
            + std::to_string(got) + " supplied");
    }

    return got;
}

int checkArgsAtLeast(const std::string& name, int min, int got)
{
    if ( got < min ) {
        throw ArgsError("\"" + name + "\" expects at least "
            + std::to_string(min) + " args, "
            + std::to_string(got) + " supplied");
    }

    return got;
}

int checkArgsEven(const std::string& name, int got)
{
    if ( got % 2 != 0 ) {
        throw ArgsError("\"" + name + "\" expects even number of args");
    }

    return got;
}