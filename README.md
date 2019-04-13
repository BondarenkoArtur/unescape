unescape.c
==========

Small utilite that helps you to unescape some JSON's escaped UTF-8 encoded strings.
`unescape_pipe` allows you to push some data in it and receive properly encoded string.

To build use Cmake or gcc
`gcc -o unescape_pipe unescape_pipe.c unescape.c`

To use, just pipe something into `unescape_pipe`
```
$ export test='\uD83D\uDE01 \uD83D\uDE18 \u2764\uFE0F \uD83D\uDE0A \uD83D\uDC4D'
$ printf '%s' "${test}" | ./unescape_pipe
😁 😘 ❤️ 😊 👍
```

### Old description
Simple implementation which unescapes JSON's escaped UTF-8 encoded strings.

Example:

    $ cat > example.c
    #include <stdio.h>
    const char *unescape(const char *, char *, size_t);
    main()
    {
        char buf[80];
        printf("%s\n",
            unescape("\\u3053\\u3093\\u306b\\u3061\\u306f\\u4e16\\u754c\\uff01",
            buf, sizeof(buf)));
    }
    ^D
    $
    $ cc -o example example.c unescape.c
    $
    $ ./example
    こんにちは世界！
    $

