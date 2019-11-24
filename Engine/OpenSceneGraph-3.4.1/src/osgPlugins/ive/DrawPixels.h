#ifndef IVE_DrawPixels
#define IVE_DrawPixels 1

#include <osg/DrawPixels>
#include "ReadWrite.h"

namespace ive{
class DrawPixels : public osg::DrawPixels, public ReadWrite {
public:
    void write(DataOutputStream* out);
    void read(DataInputStream* in);
};
}

#endif
