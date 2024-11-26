Experimental [THUMP](https://datatracker.ietf.org/doc/html/draft-kunze-thump-03) interface

This runs with Amberfish or Isearch or both at the same time.

```
git clone https://gitlab.com/amberfish/amberfish.git

git clone -b nassib https://gitlab.com/nassib/isearch2.git

mkdir index
```

This should result in three directories side-by-side: `amberfish`,
`index`, and `isearch2`.

Build Amberfish and the THUMP interface:

```
cd amberfish

./configure

gmake        # or "make" on some systems

cd src/interface && gmake        # or "make"

cd ../../..
```

Build Isearch:

```
cd isearch2

gmake        # or "make"

cd ..
```

Make sure that the `af` and `Isearch` executables are in the PATH.
This can be done using `gmake install` for both packages.

Create Amberfish and/or Isearch indexes in directories under `index/`.
Note that the directory name and the database name should match.

```
mkdir index/aftest

af -i -d index/aftest/aftest -C -v *.txt

mkdir index/istest

Iindex -d index/istest/istest *.txt
```

Start the THUMP server:

```
cd amberfish/src/interface

./thumpd &
```

The `cd` above is necessary to set the current working directory to
`amberfish/src/interface`, so that `thumpd` can find everything.

Using a web browser, run Boolean queries:

```
http://localhost:8660/?in(aftest)find(ludwig :or van :or beethoven)

http://localhost:8660/?in(istest)find(ludwig :or van :or beethoven)
```

Or using curl:

```
curl --get --data-urlencode 'in(aftest)find(ludwig :or van :or beethoven)' 'http://localhost:8660/'

curl --get --data-urlencode 'in(istest)find(ludwig :or van :or beethoven)' 'http://localhost:8660/'
```

