
#ifndef __TEST__
#define __TEST__

#define TEST_CLOUDMANAGER       0
#define TEST_TRUNKCONTROLLER    1

#if TEST_CLOUDMANAGER == 1
int test_CloudManager();
#endif

#if TEST_TRUNKCONTROLLER == 1
int test_TrunkController();
#endif


#endif

