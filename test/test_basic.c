#include "testfw.h"
#include "apz.h"
#include "test_basic.h"

void test_basic(void)
{
    CUNITTEST_HOLD();
    CUNITTEST_DECLARE(&cu);
    CUNITTEST_INIT_WITH_TITLE("Basic");
    /* Chunkunit Size */
    {
#if defined(APZ_CHUNK_32)
        CUNITTEST_ASSERT_EQ(apz_inspect_chunk_size(), 4);
#elif defined(APZ_CHUNK_64)
        CUNITTEST_ASSERT_EQ(apz_inspect_chunk_size(), 8);
#endif
    }
    CUNITTEST_RESULT();
}

