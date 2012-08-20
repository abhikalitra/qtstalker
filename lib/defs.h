#ifndef DEFS_H
#define DEFS_H

#if defined(QTSTALKER_LIBRARY)
#  define QTSTALKER_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTSTALKER_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DEFS_H
