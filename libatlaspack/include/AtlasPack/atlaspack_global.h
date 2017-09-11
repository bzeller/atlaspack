#ifndef ATLASPACK_GLOBALS_H_INCLUDED
#define ATLASPACK_GLOBALS_H_INCLUDED

// OSX and Linux use the same declaration mode for inport and export
#define DECL_EXPORT         __attribute__((visibility("default")))
#define DECL_IMPORT         __attribute__((visibility("default")))

// unused parameters
#define UNUSED(x)       (void)x

#ifdef ATLASPACK_LIBRARY
#   define ATLASPACK_EXPORT    DECL_EXPORT
#else
#   define ATLASPACK_EXPORT    DECL_IMPORT
#endif

#endif // ATLASPACK_GLOBALS_H_INCLUDED
