-- STLx tests premake file
-- To build use premake tool (http://premake.sf.net)

-- Configure paths to the dependent projects
-- Use the --tutdir=/path/to/tut or --tutdir="\"c:\program files\tut\"" to specify a custom TUT location
addoption('tutdir', "Provide directory to the TUT-framework")

-- Run tests: premake --test
addoption('test', "Run STLx tests")

-- Project properties
project.name = "stlx-tests"
project.bindir = 'out'
project.configs = { "test" }

package = newpackage()
package.name = "stlx-tests"
package.objdir = 'out'
package.kind   = 'exe'
package.language = 'c++'
package.buildflags ={ "extra-warnings", "no-pch", "no-symbols", "optimize" }
package.includepaths = { ".." }
if(options['tutdir']) then table.insert(package.includepaths, options['tutdir']) end

if(windows) then
  table.insert(package.buildflags, { "static-runtime", "no-64bit-checks", "no-edit-and-continue" } )
  package.buildoptions={ "-wd4512" }
  package.linkoptions ={ "/incremental:no" }
end

-- project configuration file
package.files = {
  -- test runner
  -- "main.cpp",
  -- common header for tests
  "tests.hxx",
  -- proper tests
  matchrecursive("*.cpp")
}

-- 
function dotest(cmd,arg)
  local ext,sep = '', '/'
  if windows then ext,sep = '.exe','\\' end
  local testapp = string.format('out%sstlx-tests%s', sep, ext)
  if not os.fileexists(testapp) then
    print("Error: compile tests first!")
    return
  end
  os.execute(testapp)
end
