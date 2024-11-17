Amberfish
=========

Amberfish is a full-text search engine with a command-line interface.

This software is distributed under the terms of the MIT License.  See
the file
[LICENSE](https://github.com/nassibnassar/amberfish/blob/main/LICENSE)
for more information.

Example:

```
autoconf

./configure

gmake

man doc/af.1

./bin/af -i -C -d mydb --split '@spsection{}' doc/amberfish.texi

./bin/af -s -d mydb -q 'document types'

./bin/af --fetch doc/amberfish.texi 12266 13772
```

For more information, see the [recently updated
documentation](https://github.com/nassibnassar/amberfish/blob/main/doc/userguide.adoc)
(work in progress).

