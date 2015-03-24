Here is traditional HelloWorld program written for 0NTL

```
#include <consoleapp.hxx>
#include <iostream>

int ntl::consoleapp::main()
{
  std::cout << "Hello from native template library!";
  return 0;
}
```
[r603](https://code.google.com/p/ontl/source/detail?r=603) and the following command line results in a 24.5 Kb exe file.
```
cl /DNDEBUG hw.cpp ntl/rtl/iostream.cpp ntl/rtl/crt.cpp ntl/rtl/wchar_mask_data.cpp /W4 /GL /GR- /Ob2gty /GzyS- 
```