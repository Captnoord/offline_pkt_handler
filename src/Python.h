#ifndef Python_h__
#define Python_h__

/**
 * Include all the pseudo python headers.
 */

/** we pack all the objects so we safe memory */
//#define ENABLE_PACKED_CLASSES

#include "PyObjects.h"
#include "PyString.h"
#include "PyUnicodeUCS2.h"

/* should we include the dumper? */
#include "PyObjectDumper.h"

#endif // Python_h__