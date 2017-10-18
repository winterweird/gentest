#ifndef INCLUDE_CLAP_HPP_HEADER_GUARD_28485232595815
#define INCLUDE_CLAP_HPP_HEADER_GUARD_28485232595815

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cctype>

namespace clargparser {
        
    // Credit: http://stackoverflow.com/a/17976541/4498826
    inline std::string trim(const std::string &s) {
       auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
       auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
       return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));

    }

    class Expectation {
        public:
            // Format string contains a single expectation
            Expectation(std::string format);

            std::string getArgName() const;
            std::string getErrorString() const;
            std::string getDefault() const;
            std::string getType() const;

            bool operator==(const std::string& arg) const;
        private:
            std::string argName;
            std::string expectedType;
            std::string errorString;
            std::string defaultValue;
    };

    class SimpleCommandLineArgumentParser {
        public:
            /**
             * Constructors.
             */
            SimpleCommandLineArgumentParser();
            SimpleCommandLineArgumentParser(int argc, char** argv);
            SimpleCommandLineArgumentParser(std::string format, int argc, char** argv);

            /**
             * Construct an argument collection based on argc and argv.
             */
            void parse(int argc, char** argv);

            /**
             * Add a format you expect to receive an argument in. Format strings use
             * the format specifiers that you are used to from functions such as
             * printf. Escape special characters with "\" (backslash).
             *
             * An optional default value can be added after the format string using
             * the | special character. Default valued arguments act as if they were
             * supplied unless overridden by the user. Example:
             * parser.expect("-a %s | noValueSpecified")
             *
             * An optional error message to return upon invalid data can be added
             * after the format string using the ? special character. Example:
             * parser.expect("-b %d ? Error: integer expected")
             *
             * There may be multiple expectations in a single format string - these
             * are delimited by a ; special character. Example:
             * parser.expect("-a %s; -b %d; --named %f | 0 ? Error: float expected")
             */
            void expect(std::string format);

            /**
             * Put the converted value of the argument in reference, if possible,
             * return whether or not the conversion was successful.
             */
            template <class Type> bool get(std::string arg, Type& reference);

            /**
             * State query methods.
             */
            bool hasShort(char arg) const; // no leading - (obviously)
            bool hasNamed(std::string name) const; // no leading --
            
            /**
             * Error handling/default value methods.
             *
             * Leading - or -- as appropriate. Will attempt to infer which kind if
             * omitted. Defaults to shortArg if name length == 1 or else namedArg.
             */
            bool hasError() const; // return true if there is any error at all
            bool hasError(std::string name) const;
            std::vector<std::string> getErrors() const; // get all errors
            std::string getError(std::string name) const; // returns empty string if no match
            bool hasDefault(std::string name) const;
            std::vector<std::string> getObjects() const; // return objects (not mapped to named arg)
        private:
            /**
             * First command line argument, process name.
             */
            std::string processName;

            /**
             * Short arguments of the form -a <val1> -b <val2> [...]
             *
             * May or may not have an associated value directly following. May or
             * may not be chained.
             */
            std::map<char, std::string> shortArgs;

            /**
             * Named arguments of the form --namedArg1 <val1> --namedArg2 <val2> [...]
             *
             * May or may not have an associated value directly following.
             */
            std::map<std::string, std::string> namedArgs;

            /**
             * List at the end which is not mapped to an arg.
             */
            std::vector<std::string> objects;

            /**
             * Errors to display if the input violates the expectations specified
             * with expect.
             *
             * Use hasError(std::string argName) to check if expectation was
             * actually violated.
             */
            std::map<std::string, std::string> errors;

            /**
             * Default values of potential arguments.
             *
             * The default value is just stored as a string, and will only be
             * accessed in the get method if the argument is not supplied.
             */
            std::map<std::string, std::string> defaults;

            /**
             * List of expectations.
             */
            std::vector<Expectation> expectations;

            /**
             * Special char const values
             */
            const char expectationDelimiter = ';';
            const char defaultValueDelimiter = '|';
            const char errorStringDelimiter = '?';

            /**
             * Helper function: Check if the given input matches the specified type
             */
            bool inputMatchesType(std::string argument, std::string type) const;

            /**
             * Helper function: Split ;-delimited format string into separate
             * strings.
             */
            std::vector<std::string> splitFormatStrings(std::string format) const;
    };
}


/**
 * METHOD IMPLEMENTATIONS
 */

// Constructors

clargparser::SimpleCommandLineArgumentParser::SimpleCommandLineArgumentParser() { }

clargparser::SimpleCommandLineArgumentParser::SimpleCommandLineArgumentParser(int argc, char** argv) {
    this->parse(argc, argv);
}

clargparser::SimpleCommandLineArgumentParser::SimpleCommandLineArgumentParser(std::string format, int argc, char** argv) {
    this->expect(format);
    this->parse(argc, argv);
}

// State queries and error handling

bool clargparser::SimpleCommandLineArgumentParser::hasShort(char arg) const {
    auto it = shortArgs.find(arg);
    return it != shortArgs.end();
}

bool clargparser::SimpleCommandLineArgumentParser::hasNamed(std::string arg) const {
    auto it = namedArgs.find(arg);
    return it != namedArgs.end();
}

bool clargparser::SimpleCommandLineArgumentParser::hasError() const {
    return errors.size() != 0;
}

bool clargparser::SimpleCommandLineArgumentParser::hasError(std::string arg) const {
    if (arg.find("-") == 0) {
        // named arg or short arg
        return errors.find(arg) != errors.end();
    }
    else if (arg.length() == 1) {
        // short arg
        return errors.find("-" + arg) != errors.end();
    }
    else {
        // named arg
        return errors.find("--" + arg) != errors.end();
    }
}

std::string clargparser::SimpleCommandLineArgumentParser::getError(std::string arg) const {
    auto it = errors.end();
    if (arg.find("-") == 0) {
        // named arg or short arg
        it = errors.find(arg);
    }
    else if (arg.length() == 1) {
        it = errors.find("-" + arg);
    }
    else {
        it = errors.find("--" + arg);
    }

    if (it != errors.end()) return it->second;
    
    return ""; // default to empty string
}

std::vector<std::string> clargparser::SimpleCommandLineArgumentParser::getErrors() const {
    std::vector<std::string> retv;
    retv.resize(errors.size());
    size_t i = 0;
    for (auto it = errors.begin(); it != errors.end(); it++)  {
        retv[i++] = it->second;
    }

    return retv;
}

bool clargparser::SimpleCommandLineArgumentParser::hasDefault(std::string arg) const {
    if (arg.find("-") == 0) {
        // named arg or short arg
        return defaults.find(arg) != defaults.end();
    }
    else if (arg.length() == 1) {
        return defaults.find("-" + arg) != defaults.end();
    }
    else {
        return defaults.find("--" + arg) != defaults.end();
    }
}

// generic getter

template <class Type>
bool clargparser::SimpleCommandLineArgumentParser::get(std::string arg, Type& reference) {
    // GET INEFFICIENCY NOTE
    // There are inefficiencies in this code (I am finding values twice) which
    // may be optimized if efficiency turns out to be an issue. However, for the
    // most part, the advantage readability probably makes it worth it imo.
    std::string value;
    
    if (get(arg, value)) {
        std::stringstream ss;
        ss << value;
        ss >> reference;
        return !ss.fail() && ss.rdbuf()->in_avail() == 0;
    }
    else {
        return false;
    }
    
}

// getter string specialization
// NOTE: wrapped in namespace because of, apparently, a g++ limitation.
// See http://stackoverflow.com/a/25594741/4498826
namespace clargparser{
    template <>
    bool clargparser::SimpleCommandLineArgumentParser::get<std::string>(std::string arg, std::string& reference) {
        if (arg.find("--") == 0) {
            // named arg
            if (hasNamed(arg)) {
                reference = namedArgs.find(arg.erase(0, 2))->second;
            }
        }
        else if (arg.find("-") == 0 && arg.length() > 1) {
            // short arg
            if (hasShort(arg[1])) {
                reference = shortArgs.find(arg[1])->second;
            }
        }
        else if (arg.length() == 1) {
            if (hasShort(arg[0])) {
                reference = shortArgs.find(arg[0])->second;
            }
        }
        else {
            if (hasNamed(arg)) {
                reference = namedArgs.find(arg)->second;
            }
        }

        if (reference.empty() && hasDefault(arg)) {
            reference = defaults[arg];
        }

        return !reference.empty();
    }
}

// methods of clusterfuck

void clargparser::SimpleCommandLineArgumentParser::parse(int argc, char** argv) {
    // assume valid argc, argv
    processName = argv[0];

    for (int i = 1; i < argc; i++) {
        if (strlen(argv[i]) > 1 && argv[i][0] == '-') {
            if (argv[i][2] == '-') {
                // named arg
                std::string argName = argv[i]+2; // ignore first 2 chars
                auto xpct = std::find(expectations.begin(), expectations.end(), argv[i]);
                if (xpct != expectations.end()) {
                    if (xpct->getType() == "%b") {
                        namedArgs[argName] = "1";
                    }
                    else if (i + 1 < argc && inputMatchesType(argv[i+1], xpct->getType())) {
                        namedArgs[argName] = argv[i + 1];
                        i++; // skip this argument
                    }
                    else {
                        errors[argv[i]] = xpct->getErrorString();
                    }
                }
            }
            else {
                // start short arg processing
                int nextExpectedArgOffset = 1;
                for (size_t ch = 1; ch < strlen(argv[i]); ch++) {
                    std::string argName = std::string("-") + argv[i][ch];
                    auto xpct = std::find(expectations.begin(), expectations.end(), argName);
                    if (xpct != expectations.end()) {
                        if (xpct->getType() == "%b") {
                            shortArgs[argv[i][ch]] = "1";
                        }
                        else if (i + nextExpectedArgOffset < argc && 
                                inputMatchesType(argv[i+nextExpectedArgOffset], xpct->getType())) {
                            shortArgs[argv[i][ch]] = argv[i+nextExpectedArgOffset];
                            nextExpectedArgOffset++; // skip this argument
                        }
                        else {
                            errors[argName] = xpct->getErrorString();
                        }
                    }
                }
                i += nextExpectedArgOffset -1; // -1 bcz incremented by loop
            }
        }
        else {
            // We blatantly assume that anything not starting with a dash and
            // not having an expectation for an argument is an object.
            objects.push_back(argv[i]); // object
        }
    }
}

void clargparser::SimpleCommandLineArgumentParser::expect(std::string format) {
    auto individualFormatStrings = splitFormatStrings(format);
    for (auto it = individualFormatStrings.begin(); it != individualFormatStrings.end(); it++) {
        expectations.push_back(Expectation(*it));
    }
    for (auto it = expectations.begin(); it != expectations.end(); it++) {
        // if expectation has default, put in defaults
        if (!it->getDefault().empty()) {
            defaults[it->getArgName()] = it->getDefault();
        }
    }
}


// helper functions

std::vector<std::string> clargparser::SimpleCommandLineArgumentParser::splitFormatStrings(std::string format) const {
    std::vector<std::string> v;
    auto cpFrom = format.begin();
    for (auto chIt = format.begin() + 1; chIt != format.end(); chIt++) {
        if (*chIt == ';' && *(chIt-1) != '\\') {
            v.push_back(std::string(cpFrom, chIt));
            cpFrom = chIt + 1;
        }
    }
    v.push_back(std::string(cpFrom, format.end()));
    return v;
}

bool clargparser::SimpleCommandLineArgumentParser::inputMatchesType(std::string argument, std::string type) const {
    if (type == "%s") return true; // always true

    std::stringstream ss;
    ss << argument;
    if (type == "%d" || type == "%i") {
        int i;
        ss >> i;
        return !ss.fail() && ss.rdbuf()->in_avail() == 0;
    }
    if (type == "%u") {
        unsigned int i;
        ss >> i;
        return !ss.fail() && ss.rdbuf()->in_avail() == 0;
    }
    if (type == "%li" || type == "%ld") {
        long l;
        ss >> l;
        return !ss.fail() && ss.rdbuf()->in_avail() == 0;
    }
    if (type == "%lu") {
        unsigned long l;
        ss >> l;
        return !ss.fail() && ss.rdbuf()->in_avail() == 0;
    }
    if (type == "%f") {
        double d;
        ss >> d;
        return !ss.fail() && ss.rdbuf()->in_avail() == 0;
    }

    return false; // default to this
}

// expectation crap

clargparser::Expectation::Expectation(std::string format) {
    std::string trimmed = trim(format);
    
    auto space = std::find(trimmed.begin(), trimmed.end(), ' ');
    argName = std::string(trimmed.begin(), space);
    
    auto delim = std::find_if(space, trimmed.end(), [](int c){return c == '|' || c == '?';});
    expectedType = trim(std::string(space, delim));
    if (expectedType.empty()) {
        // boolean type
        expectedType = "%b";
    }

    // select last of the supplied error strings and/or default values, if
    // applicable
    while (delim != trimmed.end()) {
        auto delim2 = std::find_if(delim+1, trimmed.end(), [](int c){return c == '|' || c == '?';});
        if (*delim == '|') {
            defaultValue = trim(std::string(delim+1, delim2));
        }
        else {
            errorString = trim(std::string(delim+1, delim2));
        }
        delim = delim2;
    }

}

std::string clargparser::Expectation::getArgName() const {
    return argName;
}
std::string clargparser::Expectation::getErrorString() const {
    return errorString;
}

std::string clargparser::Expectation::getDefault() const {
    return defaultValue;
}

std::string clargparser::Expectation::getType() const {
    return expectedType;
}

bool clargparser::Expectation::operator==(const std::string& arg) const {
    return argName == arg;
}

#endif /* INCLUDE_CLAP_HPP_HEADER_GUARD_28485232595815 */
