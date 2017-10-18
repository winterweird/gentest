#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "clap.hpp"
#include <fstream>


const char* progname;
bool verbose;

enum class Type {
    ALNUM,
    ALPHA,
    BLANK,
    CNTRL,
    DIGIT,
    GRAPH,
    LOWER,
    PRINT,
    PUNCT,
    SPACE,
    UPPER,
    XDIGIT,
    INVALID
};

#define TYPE_FROM_STRING(s) (s=="alnum" ? Type::ALNUM : s=="alpha" ? Type::ALPHA : s=="blank" ? Type::BLANK : s=="cntrl" ? Type::CNTRL\
                           : s=="digit" ? Type::DIGIT : s=="graph" ? Type::GRAPH : s=="lower" ? Type::LOWER : s=="print" ? Type::PRINT\
                           : s=="punct" ? Type::PUNCT : s=="space" ? Type::SPACE : s=="upper" ? Type::UPPER : s=="xdigit" ? Type::XDIGIT : Type::INVALID) 
#define STRING_FROM_TYPE(t) (t==Type::ALNUM ? "alnum" : t==Type::ALPHA ? "alpha" : t==Type::BLANK ? "blank" : t==Type::CNTRL ? "cntrl"\
                           : t==Type::DIGIT ? "digit" : t==Type::GRAPH ? "graph" : t==Type::LOWER ? "lower" : t==Type::PRINT ? "print"\
                           : t==Type::PUNCT ? "punct" : t==Type::SPACE ? "space" : t==Type::UPPER ? "upper" : t==Type::XDIGIT ? "xdigit" : "invalid")
#define SELECT_GEN(t) (t==Type::ALNUM ? alnumgen : t==Type::ALPHA ? alphagen : t==Type::BLANK ? blankgen : t==Type::CNTRL ? cntrlgen\
                           : t==Type::DIGIT ? digitgen : t==Type::GRAPH ? graphgen : t==Type::LOWER ? lowergen : t==Type::PRINT ? printgen\
                           : t==Type::PUNCT ? punctgen : t==Type::SPACE ? spacegen : t==Type::UPPER ? uppergen : t==Type::XDIGIT ? xdigitgen : alphagen)

void usage() {
        printf("Usage: %s [OPTIONS]\n\n", progname);
        
        printf("Options:\n");
        printf("  -n <sample-size>   Specify number of samples (required)\n");
        printf("  --number-samples\n\n");
        
        printf("  -e <limit>         Specify limit (of int size, length, etc.) - default 1000\n");
        printf("  --limit-end\n\n");

        printf("  -b <limit>         Specify lowest value (of int size, length, etc) - default 1\n");
        printf("  --limit-begin\n\n");
        
        printf("  -o <file>          Specify output file\n");
        printf("  --output-file\n\n");
        
        printf("  -i                 Generate integers\n");
        printf("  --integer\n\n");
        
        printf("  -d                 Generate doubles\n");
        printf("  --double\n\n");
        
        printf("  -s                 Generate strings\n");
        printf("  --string\n\n");

        printf("  -t <string-type>   Specify type of characters in string - default alpha\n");
        printf("  --char-type\n\n");

        printf ("  -v                Print more information about generated data\n");
        printf("   --verbose\n\n");
        
        exit (0);
}

void generateDoubles(const std::string & outputfile, int nsamples, double low, double limit);
void generateStrings(const std::string & outputfile, int nsamples, int low, int limit, Type type);
void generateIntegers(const std::string & outputfile, int nsamples, int low, int limit);

template <typename GenFunc, typename ArgType>
void writeToFile(const std::string & outputfile, int nsamples, GenFunc genfunc, ArgType lowLimit, ArgType limit) {
    std::ofstream file(outputfile);
    for (int i = 0; i < nsamples; i++) {
        file << genfunc(lowLimit, limit) << std::endl;
    }
    file.close();
}

int intgen(int lowLimit, int limit) {
    int diff = limit - lowLimit;
    return lowLimit + (rand() % diff);
}

double doublegen(double lowLimit, double limit) {
    // *shrugs*
    double diff = limit - lowLimit;
    return lowLimit + (double(rand())/double(RAND_MAX)) * diff;
}

char charFromFunc(int (*func)(int)) {
    int ch;
    while (!func(ch = rand()%128));
    return (char)ch;
}

char chargen(Type type) {
    char ch;
    switch (type) {
        case Type::ALNUM: ch = charFromFunc(isalnum); break;
        case Type::ALPHA: ch = charFromFunc(isalpha); break;
        case Type::BLANK: ch = charFromFunc(isblank); break;
        case Type::CNTRL: ch = charFromFunc(iscntrl); break;
        case Type::DIGIT: ch = charFromFunc(isdigit); break;
        case Type::GRAPH: ch = charFromFunc(isgraph); break;
        case Type::LOWER: ch = charFromFunc(islower); break;
        case Type::PRINT: ch = charFromFunc(isprint); break;
        case Type::PUNCT: ch = charFromFunc(ispunct); break;
        case Type::SPACE: ch = charFromFunc(isspace); break;
        case Type::UPPER: ch = charFromFunc(isupper); break;
        case Type::XDIGIT: ch = charFromFunc(isxdigit); break;
        default: fprintf(stderr, "Error: invalid char type %s\n", STRING_FROM_TYPE(type)); exit(0);
    }
    return ch;
}

std::string stringgen(int lowLimit, int lenlimit, Type type) {
    int actualLength = rand() % lenlimit;
    std::string ret;
    for (int i = -lowLimit; i < actualLength; i++) {
        ret += chargen(type);
    }
    return ret;
}

std::string alnumgen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::ALNUM); }
std::string alphagen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::ALPHA); }
std::string blankgen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::BLANK); }
std::string cntrlgen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::CNTRL); }
std::string digitgen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::DIGIT); }
std::string graphgen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::GRAPH); }
std::string lowergen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::LOWER); }
std::string printgen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::PRINT); }
std::string punctgen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::PUNCT); }
std::string spacegen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::SPACE); }
std::string uppergen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::UPPER); }
std::string xdigitgen(int lowlimit, int lenlimit) { return stringgen(lowlimit, lenlimit, Type::XDIGIT); }

int main(int argc, char** argv) {
    progname = argv[0];
    
    srand(time(NULL));
    
    clargparser::SimpleCommandLineArgumentParser clap;
    clap.expect("-o %s | input.txt; -t %s | alpha; -n %d ? integer expected; -e %d | 1000 ? integer expected; -b %d | 1 ? integer expected; -i; -d; -s; -v");
    clap.parse(argc, argv);

    if (clap.hasError()) {
        auto errs = clap.getErrors();
        for (auto it = errs.begin(); it != errs.end(); it++) {
            printf("Error: %s\n", it->c_str());
        }
        usage();
    }
    else if (!clap.hasShort('n')) {
        printf("No sample size argument provided.\n");
        usage();
    }
    else {
        std::string outputfile;
        int nsamples, lowLimit, limit;
        
        clap.get("-o", outputfile);
        clap.get("-n", nsamples);
        clap.get("-b", lowLimit);
        clap.get("-e", limit);
        verbose = clap.hasShort('v') || clap.hasNamed("verbose");
        
        if (clap.hasShort('d')) {
            generateDoubles(outputfile, nsamples, (double)lowLimit, (double)limit);
        }
        else if (clap.hasShort('s')) {
            std::string type;
            clap.get("-t", type);
            Type t = TYPE_FROM_STRING(type);
            generateStrings(outputfile, nsamples, lowLimit, limit, t);
        }
        else {
            generateIntegers(outputfile, nsamples, lowLimit, limit);
        }
    }
}


void generateDoubles(const std::string & outputfile, int nsamples, double lowLimit, double limit) {
    if (verbose) {
        printf("Writing doubles to file %s\n", outputfile.c_str());
        printf("Number of samples: %d\n", nsamples);
        printf("Range: [%g, %g)\n", lowLimit, limit);
    }
    writeToFile(outputfile, nsamples, doublegen, lowLimit, limit);
}
void generateStrings(const std::string & outputfile, int nsamples, int lowLimit, int limit, Type type) {
    if (verbose) {
        printf("Writing strings to file %s\n", outputfile.c_str());
        printf("Include characters: %s\n", STRING_FROM_TYPE(type));
        printf("Number of samples: %d\n", nsamples);
        printf("Length range: [%d, %d)\n", lowLimit, limit);
    }
    writeToFile(outputfile, nsamples, SELECT_GEN(type), lowLimit, limit);
}
void generateIntegers(const std::string & outputfile, int nsamples, int lowLimit, int limit) {
    if (verbose) {
        printf("Writing integers to file %s\n", outputfile.c_str());
        printf("Number of samples: %d\n", nsamples);
        printf("Range: [%d, %d)\n", lowLimit, limit);
    }
    writeToFile(outputfile, nsamples, intgen, lowLimit, limit);
}
