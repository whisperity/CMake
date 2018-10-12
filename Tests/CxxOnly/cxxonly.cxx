#include "libcxx1.h"
#include "libcxx2.h"
#ifdef _MSC_VER
extern int testCPP;
#endif

#include <stdio.h>

#ifdef _MODULES_TS
import testCxxModuleTs;
#endif

int main()
{
#ifdef _MSC_VER
  testCPP = 1;
#endif

  if (LibCxx1Class::Method() != 2.0) {
    printf("Problem with libcxx1\n");
    return 1;
  }
  if (LibCxx2Class::Method() != 1.0) {
    printf("Problem with libcxx2\n");
    return 1;
  }

#ifdef _MODULES_TS
  if (!CxxModuleTsClass::Test()) {
    printf("Problem with module-ts test\n");
    return 1;
  }
#endif

  return 0;
}
