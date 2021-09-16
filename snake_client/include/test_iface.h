#pragma once

#if defined (UNITTEST)
    #define TEST_IFACE virtual
#else
    #define TEST_IFACE
#endif
