# ZPR_ResMonitor
## Compilation
Requires `boost` library, `cmake` and `flex` (for optional lexer modification). Compiler have to implement C++14 standard (at least).

```bash
$ mkdir BUILD && cd BUILD
$ cmake ..
$ {make, msbuild} {monitor, testsmonitor}
```

## Used additional libraries

1. SMTP client (with slight modifications for wp's SMTP sever): [http://codingways.blogspot.com](http://codingways.blogspot.com/2012/08/send-email-in-c-smtp-client-in-c.html)
2. HTTP server: [Simple-Web-Server](https://github.com/eidheim/Simple-Web-Server)
