#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__
#include "TextColor.h"

class Crash : public std::exception {
private:
    std::string errorMsg;

public:
    Crash(const std::string& _errorMsg)
    : errorMsg(_errorMsg)
    {}

    const char* what() const noexcept override { 
        AnsiCodes::showError("ERROR: " + errorMsg);
        return errorMsg.c_str(); 
    }
};

class Warning : public std::exception {
private:
    std::string suggestionMsg;
    std::string warningMsg;
public:
    Warning(const std::string& _warningMsg, bool defaultMsg = false)
    : warningMsg(_warningMsg), suggestionMsg("")
    {
        if (defaultMsg) suggestionMsg = "If needed you could use the command '--help'.";
    }

    Warning(const std::string& _warningMsg, const std::string& _suggestionMsg)
    : warningMsg(_warningMsg), suggestionMsg(_suggestionMsg)
    {}

    const char* what() const noexcept override { 
        AnsiCodes::showWarning("WARNING: " + warningMsg);
        AnsiCodes::showSuggestion(suggestionMsg);
        return warningMsg.c_str(); 
    }
};

class FileCrash : public Crash {
public:
    FileCrash(const std::string& filename) 
    : Crash("There was an error trying to open " + filename + "!")
    {}
};

class CloseDevice : public Warning {
public:
    CloseDevice(bool defaultSuggestion = false)
    : Warning("Please close the current block device!", defaultSuggestion)
    {}
};

class OpenDevice : public Warning {
public:
    OpenDevice(bool defaultSuggestion = false)
    : Warning("Please open a device first!", defaultSuggestion)
    {}
};

#endif //__EXCEPTIONS_H__