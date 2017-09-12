#ifndef ATLASPACK_GLOBALS_H_INCLUDED
#define ATLASPACK_GLOBALS_H_INCLUDED

#ifdef WIN32
#define DECL_EXPORT         __declspec(dllexport)
#define DECL_IMPORT         __declspec(dllimport)
#else
#define DECL_EXPORT         __attribute__((visibility("default")))
#define DECL_IMPORT         __attribute__((visibility("default")))
#endif

// unused parameters
#define UNUSED(x)       (void)x

#ifdef ATLASPACK_LIBRARY
#   define ATLASPACK_EXPORT    DECL_EXPORT
#else
#   define ATLASPACK_EXPORT    DECL_IMPORT
#endif

#endif // ATLASPACK_GLOBALS_H_INCLUDED
