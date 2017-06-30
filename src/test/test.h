
#ifndef __TEST__
#define __TEST__

#define TEST_CLOUDMANAGER       0
#define TEST_TRUNKCONTROLLER    0
#define TEST_SERIALTERMINAL     1

#if TEST_CLOUDMANAGER == 1
int test_CloudManager();
#define DO_TEST   test_CloudManager
#endif

#if TEST_TRUNKCONTROLLER == 1
int test_TrunkController();
#define DO_TEST   test_TrunkController
#endif

#if TEST_SERIALTERMINAL == 1
int test_SerialTerminal();
#define DO_TEST   test_SerialTerminal
#endif

#endif

