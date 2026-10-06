# Building and installing mu2ebintools

This is the UPS/cetbuildtools procedure the package was written for,
restated from the top-level `README` and `CMakeLists.txt`. It was **not
re-run** for this document: the review host has no UPS, cetbuildtools or
fhiclcpp.

## Requirements

| Item | Version | Source |
|---|---|---|
| UPS (`UPS_DIR` set) | any | `ups/setup_for_development` refuses to run without it |
| cetbuildtools | `v7_17_01` (build only) | `ups/product_deps:9` |
| fhiclcpp (brings cetlib, cetlib_except) | `v4_12_02` | `ups/product_deps:8` |
| Compiler qualifier | `e19` or `e20` (gcc 8 / gcc 9), with `debug`, `opt` or `prof` | `ups/product_deps:14-19` |
| CMake | ≥ 2.8 declared; **< 4.0 required in practice** (CMake 4.0 rejects `cmake_minimum_required` below 3.5) | `CMakeLists.txt:16` |

## Build

```bash
mkdir -p /some/path/build /some/path/install
cd /some/path/build
source /path/to/mu2ebintools/ups/setup_for_development -p e20   # -d debug, -o opt, -p prof
# setup_for_development prints a suggested cmake line; use it, e.g.
env CC=gcc CXX=g++ FC=gfortran \
  cmake -DCMAKE_INSTALL_PREFIX=/some/path/install \
        -DCMAKE_BUILD_TYPE=${CETPKG_TYPE} "${CETPKG_SOURCE}"
make
make install
make package        # UPS tarball in the build directory
```

`CMakeLists.txt:22-25` stops with `ERROR: setup cetbuildtools to get the
cmake modules` if `CETBUILDTOOLS_VERSION` is not set.

## Publish to CVMFS (historical)

The `README` records the procedure used to publish to
`/cvmfs/mu2e.opensciencegrid.org/artexternals`: a `cvmfs_server
transaction`, untar the UPS tarball, `ups list -aK+ mu2ebintools`, then
`cvmfs_server publish`. The host, account and tarball names in it
(`oasiscfs.fnal.gov`, `slf6`, `e10`, a personal build path) are from the
SLF6 era and were not checked.

## Using an installed copy

```bash
setup mu2ebintools v1_01_21 -q e20:prof      # UPS, where that build is published (not checked)
export FHICL_FILE_PATH=.:/path/to/fcl/tree
fhicl-summary job.fcl
```

The man pages are not installed by the CMake build. Read them in place with
`man -l man/man1/fhicl-getpar.1`.
