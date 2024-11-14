Amberfish
=========

Amberfish is a command-line tool for indexing and searching text.

This software is distributed under the terms of the MIT License.  See the file
[LICENSE](https://gitlab.com/amberfish/amberfish/-/blob/main/LICENSE?ref_type=heads)
for more information.

```
autoconf

./configure

gmake

man doc/af.1

./bin/af -i -C -d mydb --split '@spsection{}' doc/amberfish.texi

./bin/af -s -d mydb -q 'document types'

./bin/af --fetch doc/amberfish.texi 12266 13772
```
