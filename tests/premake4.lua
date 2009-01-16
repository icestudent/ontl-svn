-- STLx tests premake file
-- To build use premake4 tool (http://industriousone.com/premake)

-- Configure paths to the dependent projects
-- Use the --tutdir=/path/to/tut or --tutdir="\"c:\program files\tut\"" to specify a custom TUT location
newoption {
  trigger = "tutdir",
  description = "Provide directory to the TUT-framework",
  value = "path"
}

solution "stlx-tests"
  configurations { "test" }
  targetdir "out"
  objdir    "out"

project  "stlx-tests"
  language  "C++"
  kind      "ConsoleApp"
  includedirs { "..", _OPTIONS["tutdir"] }
  flags     { "ExtraWarnings", "NoPCH", "Optimize" }

configuration "windows"
  flags { "StaticRuntime", "No64BitChecks", "NoEditAndContinue", "NoManifest" }
  buildoptions { "-wd4512" }
  linkoptions  { "/incremental:no" }

-- project configuration file
files {
  -- test runner
  -- "main.cpp",
  -- common header for tests
  "tests.hxx",
  -- proper tests
  "**.cpp"
}

local function run_tests()
  local ext = ''
  if OS == windows then ext = '.exe' end
  local testapp = path.translate('out/stlx-tests' .. ext)
  if not os.isfile(testapp) then
    print("Error: compile tests first!")
    return
  end
  os.execute(testapp)
end

-- Run tests: premake4 test
newaction {
  trigger = "test",
  description = "Run STLx tests",
  execute = run_tests
}
