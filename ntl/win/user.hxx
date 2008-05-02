/**\file*********************************************************************
 *                                                                     \brief
 *  Win User API 
 *
 ****************************************************************************
 */

#ifndef NTL__WIN_USER
#define NTL__WIN_USER

#include "windef.hxx"
#include "application.hxx"
#include "../handle.hxx"

#pragma comment(lib,    "user32.lib")

namespace ntl {
namespace win {


/**\addtogroup  winuser ***************** Win32 User API library ************
 *@{*/


typedef uintptr_t wparam;
typedef intptr_t  lparam;
typedef intptr_t  lresult;

typedef uint16_t  atom;

typedef struct _opacue_wnd_handle { } * wnd_handle;
typedef struct _opacue_icon_handle { } * icon_handle;
typedef struct _opacue_cursor_handle { } * cursor_handle;
typedef struct _opacue_brush_handle { } * brush_handle;


template<typename charT = char>
struct wndclassex
{
  typedef char      char_type;

  typedef
    lresult __stdcall
      proc_t(wnd_handle hwnd, uint32_t uMsg, wparam wParam, lparam lParam);

  typedef proc_t * proc;

  uint32_t          cbSize;
  uint32_t          style;
  proc              lpfnWndProc;
  int               cbClsExtra;
  int               cbWndExtra;
  hinstance         hInstance;
  icon_handle       hIcon;
  cursor_handle     hCursor;
  brush_handle      hbrBackground;
  const char_type * lpszMenuName;
  const char_type * lpszClassName;
  icon_handle       hIconSm;

};//struct wndclass


///\note returns 32 bit value under debugger

NTL__EXTERNAPI
uint32_t __stdcall
  RegisterClassExA(const wndclassex<char> *);

NTL__EXTERNAPI
uint32_t __stdcall
  RegisterClassExW(const wndclassex<wchar_t> *);

NTL__EXTERNAPI
int __stdcall
  UnregisterClassA(
    const char *  lpClassName,
    hinstance     hInstance);

NTL__EXTERNAPI
int __stdcall
  UnregisterClassW(
    const wchar_t * lpClassName,
    hinstance       hInstance);

template<typename charT>
static inline
atom register_class(const wndclassex<charT> & wc);

template<>
static inline
atom register_class(const wndclassex<char> & wc)
{
  return static_cast<atom>(0xFFFF & RegisterClassExA(&wc));
}

template<>
static inline
atom register_class(const wndclassex<wchar_t> & wc)
{
  return static_cast<atom>(0xFFFF & RegisterClassExW(&wc));
}

template<typename charT>
static inline
bool unregister_class(
  const charT class_name[],
  hinstance instance = application<charT>::instance());

template<>
static inline
bool unregister_class(
  const char  class_name[],
  hinstance   instance)
{
  return 0 != UnregisterClassA(class_name, instance);
}

template<>
static inline
bool unregister_class(
  const wchar_t class_name[],
  hinstance     instance)
{
  return 0 != UnregisterClassW(class_name, instance);
}

static inline
bool unregister_class(
  atom      class_atom,
  hinstance instance    = application<char>::instance())
{
  return 0 != UnregisterClassA(reinterpret_cast<const char*>(class_atom), instance);
}


namespace aux {
void unregister_class(atom class_atom)
{ 
  win::unregister_class(class_atom);
}
}


template<typename charT = char>
class wnd_class : public basic_handle<atom, aux::unregister_class>
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef charT char_type;

    wnd_class()
    {
      wndclassex<char_type> wc;
      wc.cbSize        = sizeof(wndclassex<char_type>);
      wc.style         = 0;
      wc.lpfnWndProc   = dispatcher;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = 0;
      wc.hInstance     = application<char>::instance();
      wc.hIconSm       =
      wc.hIcon         = LoadIconA(0, IDI_WINLOGO);
      wc.hCursor       = LoadCursorA(0, IDC_ARROW);
      wc.hbrBackground = 0;
      wc.lpszMenuName  = 0;
      wc.lpszClassName = class_name;
      this->set(register_class(wc));
    }

    static
    lresult __stdcall
      dispatcher(wnd_handle hwnd, UINT msg, wparam wParam, lparam lParam)
    {
      if( msg == WM_NCCREATE )
      {
          ((window *)((CREATESTRUCTA *)lParam)->lpCreateParams)->handle = hWnd;

          SetWindowLongA(hWnd, GWL_USERDATA,
              (long)((CREATESTRUCTA *)lParam)->lpCreateParams);
      }
      else if(window * This = (window *)GetWindowLongA(hWnd, GWL_USERDATA))
          This->window_procedure(hWnd, msg, wParam, lParam);

      return DefWindowProcA(hWnd, msg, wParam, lParam);
    }



};// class wndclass_handle


/**@} winuser */


}//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_USER
