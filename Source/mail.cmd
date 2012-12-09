/* */

                                        /* Register REXX APIs */
Call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
Call SysLoadFuncs

Call Beep 500,1000

Pull HttpRequest
Say 'Content-type: text/html'||'0A'x||'0A'x
Say '<!doctype html public "-//IETF//DTD HTML 2.0//EN">'
Say '<html>'
Say '<head>'
Say '<title>Online Automatic Update (Simulation Result)</title>'
Say '<meta HTTP-EQUIV="Pragma" content="no-cache">'
Say '<meta HTTP-EQUIV="Expires" content="0">'
Say '<meta HTTP-EQUIV="Cache-Control" content="no-cache">'
Say '</head>'
Say '<body>'
Command='@set >env.data'
Command
Say 'HttpRequest: '||HttpRequest
Say '<p>'
Say 'Zeit: '||time()
Say '<p>'
Say 'Environment:<p>'
Say '<xmp>'
'type env.data'
Say '</xmp>'
Say '</body>'
Say '</html>'
Call Beep 1000,1000
