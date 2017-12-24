# ZPR_ResMonitor
## Compilation
Requires `boost` library, `scons`, `flex` (for optional lexer modification), `python2` and `openssl` (for https support).

```
$ scons [{tests, monitor}]
$ ./{tests, monitor}
```

## Used additional libraries

1. SMTP client (with slight modifications): [http://codingways.blogspot.com](http://codingways.blogspot.com/2012/08/send-email-in-c-smtp-client-in-c.html)
2. HTTPS server and client: [Simple-Web-Server](https://github.com/eidheim/Simple-Web-Server)