/**
 *	@file rtlcompress.cpp
 *	@brief Sample which shows how to work with console and files with NTL library
 *	@note Compilation command-line: cl /nologo /I../../branches/x64/ntl /DWIN32 /D_UNICODE /DUNICODE /GS- rtlcompress.cpp /link /subsystem:console /libpath:your_lib_path_with_ntdll.lib 
 *	@author icestudent
 *	@version 0.3
 **/

#include <consoleapp.hxx>
#include <nt/file.hxx>
#include <nt/debug.hxx>
#include <cstdio>

/* assume that we do not work with large files */
#pragma warning(disable:4267) // 'argument' : conversion from 'size_t' to 'std::uint32_t', possible loss of data. 

using namespace ntl;
using namespace ntl::nt;
using namespace std;

class app: consoleapp
{
  wstring infile, outfile;
  bool decompress;
public:
  int proceed();
protected:
  bool acquire_console()
  {
    return 1;//console::attach() || console::alloc();
  }
  void header()
  {
    console::write(" rtlcompress v0.1\n");
  }
  void usage()
  {
    console::write(" usage: rtlcompress.exe [-d] <infile> <outfile>\n");
  }

  bool parse_args()
  {
    decompress = false;
    command_line cmdl;
    if(cmdl.size() < 3)
      return false;
    for(command_line::const_iterator cmd = cmdl.cbegin()+1; cmd != cmdl.cend(); ++cmd){
      if(!wcscmp(*cmd, L"-d"))
        decompress = true;
      else if(infile.empty())
        infile = *cmd;
      else if(outfile.empty())
        outfile = *cmd;
    }
    return !infile.empty() && !outfile.empty();
  }
};

int app::proceed()
{
  acquire_console();
  header();

  if(!parse_args()){
    usage();
    return 2;
  }

  unicode_string fus1, fus2;
  rtl_relative_name relative1, relative2;
  if(!RtlDosPathNameToNtPathName_U(infile.c_str(), &fus1, NULL, &relative1)){
    console::write("wrong infile path\n");
    return 3;
  }
  if(!RtlDosPathNameToNtPathName_U(outfile.c_str(), &fus2, NULL, &relative2)){
    console::write("wrong outfile path\n");
    return 4;
  }
  const_unicode_string cus1(relative1.RelativeName), cusf(fus1);
  object_attributes oa_in1(relative1.ContainingDirectory, cus1), oa_in2(cusf);
  object_attributes& oa_in = !relative2.ContainingDirectory ? oa_in2 : oa_in1;
  file in(oa_in);
  if(!in){
    console::write("can't open infile\n");
    return 3;
  }
  const_unicode_string cus3(relative2.RelativeName), cus2(fus2);
  object_attributes oa_out1(relative2.ContainingDirectory, cus3), oa_out2(cus2);
  object_attributes& oa_out = !relative2.ContainingDirectory ? oa_out2 : oa_out1;
  file out(oa_out, file::supersede, file::generic_write);
  if(!out){
    console::write("can't open outfile\n");
    return 4;
  }
  RtlFreeUnicodeString(&fus1),
  RtlFreeUnicodeString(&fus2);

  raw_data indata = in.get_data(), outdata;
  outdata.resize(indata.size()*(decompress ? 5 : 1));

  ntstatus st;
  uint32_t buf_cb;
  if(!decompress){
    uint32_t chunk_cb;
    st = RtlGetCompressionWorkSpaceSize(compression::default_format, &buf_cb, &chunk_cb);
    if(success(st)){
      raw_data tmp(buf_cb);
      st = RtlCompressBuffer(compression::default_format, indata.cbegin(), indata.size(), 
        outdata.begin(), outdata.size(), chunk_cb, &buf_cb, tmp.begin());
    }
  }else{
    st = RtlDecompressBuffer(compression::default_format, outdata.begin(), outdata.size(),
      indata.cbegin(), indata.size(), &buf_cb);
  }
  if(success(st))
    outdata.resize(buf_cb);

  out.write(outdata.cbegin(), outdata.size());

  char buf[128];
  size_t i = indata.size(), o = outdata.size();
  size_t l = _snprintf(buf, sizeof(buf)-1, "\n done. %d -> %d (%d%%)\n", i, o, decompress ? (i*100/o) : (o*100/i));
  console::write<char>(buf, l);
  return 0;
}

int consoleapp::main()
{
  app app_;
  return app_.proceed();
}
