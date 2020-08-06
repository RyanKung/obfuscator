#ifndef __MACRO_OBF__
#define __MACRO_OBF__



#define DEBUG_TYPE "funcobf"

#ifdef DEBUG
#define DEBUG_OUT(s) do { \
    DEBUG_WITH_TYPE(DEBUG_TYPE, dbgs() << __PRETTY_FUNCTION__ << ": " << s << "\n" ); \
    errs() << __PRETTY_FUNCTION__ << ": " << s << "\n";			\
} while(0)
#else
#define DEBUG_OUT(s) do { \
    DEBUG_WITH_TYPE(DEBUG_TYPE, dbgs() << __PRETTY_FUNCTION__ << ": " << s << "\n" ); \
} while(0)
#endif
#endif
