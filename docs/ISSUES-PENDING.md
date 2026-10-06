# Staged issues — mu2ebintools

Findings from the September 2026 DAQ mega-review (`mu2e-sept-mega-review`),
2026-10-06.

**Why these are staged.** `Mu2e/mu2ebintools` is public with issues enabled.
Under CONTRACT.md §9.1 no GitHub write is attempted from a package agent; each
finding below is in the §6 body format, ready to file unchanged.

**Dedup.** `gh issue list -R Mu2e/mu2ebintools --state all --limit 300`
returned no issues on 2026-10-06. All findings are new.

**Method.** Both source files were read in full. Nothing was built or run:
the review host has no fhiclcpp, cetlib, cetbuildtools or UPS.

**Fingerprints** are `sha1("Mu2e/mu2ebintools:<path>:<rule_id>")[:8]`; inputs
are tabulated at the end.

5 findings: 2 × S3, 3 × S4.

---

## MBT-001

**Title:** `[fhicl-summary] default output gates the services count on physics.producers`

**Severity:** S3 — correctness or maintainability risk, no current outage
**Category:** bug
**Location:** `mu2ebintools/fhicl-summary.cc:294`
**Found by:** mega-review (`mu2e-sept-mega-review`), 2026-10-06

## What
At the default verbosity the `services:` line is guarded by `_hasProducers`
instead of `_hasServices`. A job with services but no `physics.producers`
prints `services: <not present>`, and a job with producers but no
`services` table prints `services: 0`.

## Evidence
```
$ sed -n 293,295p mu2ebintools/fhicl-summary.cc
  printSize ( os, _hasOutputs,       "outputs:       ", _outputs   );
  printSize ( os, _hasProducers,     "services:      ", _services  );
  printSize ( os, _hasProducers,     "producers:     ", _producers );
$ sed -n 262p mu2ebintools/fhicl-summary.cc
  _hasServices = getNames( _pset, "services", _services );
```
The `-v` and `-vv` levels use `_hasServices` correctly (`:312`, `:333`).

## Why it matters
The tool's purpose is to describe a configuration accurately at a glance.
This line is silently wrong for analysis-only or output-only jobs, which have
no producers. Rated S3 rather than S2: the tool is an offline diagnostic, not
part of data taking, and `-v` gives the correct listing.

## Suggested fix
```diff
-  printSize ( os, _hasProducers,     "services:      ", _services  );
+  printSize ( os, _hasServices,      "services:      ", _services  );
```
Add a test with a services-only `.fcl` (see MBT-004).

<!-- mega-review-fingerprint: 7e542edc -->

---

## MBT-002

**Title:** `[build] build depends on cetbuildtools/UPS only and declares CMake 2.8; sources rely on transitive standard headers`

**Severity:** S3 — correctness or maintainability risk, no current outage
**Category:** enhancement
**Location:** `CMakeLists.txt:16`, `CMakeLists.txt:22-26`, `ups/product_deps:8-19`
**Found by:** mega-review (`mu2e-sept-mega-review`), 2026-10-06

## What
The only supported build is cetbuildtools + UPS. `CMakeLists.txt` aborts
without `CETBUILDTOOLS_VERSION` and declares `cmake_minimum_required(VERSION
2.8)`. `product_deps` pins fhiclcpp `v4_12_02` with `e19`/`e20` qualifiers
(gcc 8/9, 2020). There is no Spack recipe, although the rest of the Mu2e
TDAQ stack is assembled with Spack MPD (`mu2e-tdaq-suite/CMakeLists.txt:1`).
The sources also use `std::array`, `std::max`, `std::find` and `exit` without
including `<array>`, `<algorithm>` or `<cstdlib>`, and are compiled with
`WERROR -pedantic`.

## Evidence
```
$ sed -n 16p CMakeLists.txt
cmake_minimum_required(VERSION 2.8)  # use cmake 2.8 or later
$ sed -n 22,25p CMakeLists.txt
set( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
if( NOT CETBUILDTOOLS_VERSION )
  message( FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules" )
endif()
$ grep -n 'include <' mu2ebintools/fhicl-summary.cc
4:#include <iostream>
5:#include <iomanip>
6:#include <stdexcept>
7:#include <string>
8:#include <vector>
$ grep -n 'std::array\|std::max\|find(\|exit(' mu2ebintools/fhicl-summary.cc | head -4
32:    maxlen_label = std::max( maxlen_label, label.size() );
65:    static std::array<std::string,3> names { "ambiguous", "trigger", "end" };
136:  std::array<std::vector<PathInfo>,3> _pathInfo;
184:    auto i = find( reservedNames.begin(), reservedNames.end(), s);
```
CMake 4.0 removed compatibility with `cmake_minimum_required` versions below
3.5, so a CMake 4 configure stops at line 16. This was not reproduced here,
because running a configure was not permitted in this session.

## Why it matters
The tools cannot be rebuilt with CMake 4 or in a Spack environment without
first porting the build, and the pinned `e19`/`e20` UPS products date from
2020. Whoever needs them next pays for that
porting under time pressure. The missing includes are the kind of break that
appears when a toolchain trims transitive headers, and `WERROR` turns related
warnings into failures.

## Suggested fix
Either retire the package (MBT-005), or port it:
- a plain `CMakeLists.txt` with `cmake_minimum_required(VERSION 3.19)` and
  `find_package(fhiclcpp)`/`find_package(cetlib)` as provided by the Spack
  art suite, plus a Spack `package.py`;
- explicit `#include <array>`, `<algorithm>` and `<cstdlib>`.

<!-- mega-review-fingerprint: 471ebe74 -->

---

## MBT-003

**Title:** `[cli] positional-only arguments and one exit status for every failure`

**Severity:** S4 — cosmetic, stale, hygiene
**Category:** enhancement
**Location:** `mu2ebintools/fhicl-getpar.cc:20-83`, `mu2ebintools/fhicl-summary.cc:347-395`
**Found by:** mega-review (`mu2e-sept-mega-review`), 2026-10-06

## What
Both programs require arguments in fixed positions, recognise `-h` only as
`argv[1]`, and exit 1 for everything. That covers a usage error, an unreadable
or unparsable file, an absent parameter, a type mismatch, and (in
`fhicl-summary`) a fhiclcpp error during path classification. In
`fhicl-summary`, `job.fcl -v` treats `-v` as the file name. `fhicl-getpar` has
no `--double`/`--bool`, and lines 72-74 are dead code.

## Evidence
```
$ sed -n 30,31p mu2ebintools/fhicl-getpar.cc
    if ( argc != 4 ){
      throw std::runtime_error("Error: wrong number of parameters.\n" + usage());
$ sed -n 77,79p mu2ebintools/fhicl-getpar.cc
  catch(std::exception& e) {
    std::cerr<<e.what()<<std::endl;
    exit(1);
$ sed -n 362,363p mu2ebintools/fhicl-summary.cc
    const std::string opt = ( argc == 2 ) ? std::string() : (argv[1]);
    const std::string infile = (argc == 2 ) ? std::string(argv[1]) : std::string(argv[2]);
```

## Why it matters
`fhicl-getpar` exists for scripts. A script cannot distinguish "parameter not
set, use the default" from "file broken" without parsing stderr. This is
convenience, not correctness: S4.

## Suggested fix
Exit 2 for usage errors and 3 for an absent parameter (check with
`pset.has_key(name)` before `get`), keeping 1 for parse and type errors.
Document the codes in the man pages, which already list the current
behaviour.

<!-- mega-review-fingerprint: 0d39262c -->

---

## MBT-004

**Title:** `[hygiene] no tests, no CI, no LICENSE`

**Severity:** S4 — cosmetic, stale, hygiene
**Category:** enhancement
**Location:** repository root (`LICENSE`, `test/`, `.github/workflows/` absent)
**Found by:** mega-review (`mu2e-sept-mega-review`), 2026-10-06

## What
There is no test of either program, no CI workflow and no LICENSE file. The
test subdirectory is commented out (`CMakeLists.txt:49`). The plain `README`
publishing recipe names an SLF6/`e10` tarball, a personal build path and
`oasiscfs.fnal.gov`. This review adds `README.md`, `docs/INSTALL.md` and man
pages, and keeps the old `README`.

## Evidence
```
$ git ls-files | grep -Ei 'licen|^tests?/|\.github/' | wc -l
0
$ sed -n 49p CMakeLists.txt
#add_subdirectory(test)
$ gh repo view Mu2e/mu2ebintools --json licenseInfo
{"licenseInfo":null}
```

## Why it matters
MBT-001 would have been caught by a three-line `.fcl` fixture. Because the
repository is public with no licence, reuse terms are undefined.

## Suggested fix
Add the Fermilab-standard licence used elsewhere in Mu2e. If the package is
kept (MBT-005), add `test/` with a few `.fcl` fixtures and expected-output
files for each option and verbosity, run by `ctest`, and a CI job in the
Spack environment.

<!-- mega-review-fingerprint: 8672d413 -->

---

## MBT-005

**Title:** `[scope] declare why mu2ebintools is a mu2edaq-main submodule: no DAQ package builds or calls it`

**Severity:** S4 — cosmetic, stale, hygiene
**Category:** documentation
**Location:** `README.md` (purpose); `mu2edaq-main/.gitmodules:55-57`
**Found by:** mega-review (`mu2e-sept-mega-review`), 2026-10-06

## What
`mu2ebintools` is an Offline-era utility (2015–2020) and a submodule of
`mu2edaq-main`, but nothing in the DAQ tree references it. The superproject
build and test harness does not list it, and no script calls `fhicl-getpar`
or `fhicl-summary`.

## Evidence
```
$ grep -rIl 'fhicl-getpar\|fhicl-summary\|mu2ebintools' mu2edaq-main \
    --exclude-dir=.git --exclude-dir=mu2ebintools --exclude-dir=docs
.gitmodules
README.md
man/man1/mu2edaq-bootstrap.1
$ sed -n 23,50p mu2edaq-main/testing/common.sh | grep -c mu2ebintools
0
```

## Why it matters
An unused submodule is checked out, reviewed and carried through every
release tag at no benefit. If the DAQ does need it, for example for run
configuration scripts that read FHiCL, that dependency is undocumented.

## Suggested fix
State the intended DAQ use in `README.md` and add it to the superproject
build (after MBT-002), or remove it from `mu2edaq-main/.gitmodules` and leave
it to Offline.

<!-- mega-review-fingerprint: 99583ace -->

---

## Fingerprint inputs

| ID | Input to `sha1(...)[:8]` | Fingerprint |
|---|---|---|
| MBT-001 | `Mu2e/mu2ebintools:mu2ebintools/fhicl-summary.cc:services-count-wrong-guard` | 7e542edc |
| MBT-002 | `Mu2e/mu2ebintools:CMakeLists.txt:legacy-build-stack` | 471ebe74 |
| MBT-003 | `Mu2e/mu2ebintools:mu2ebintools/fhicl-getpar.cc:cli-exit-status` | 0d39262c |
| MBT-004 | `Mu2e/mu2ebintools:LICENSE:missing-license-tests-ci` | 8672d413 |
| MBT-005 | `Mu2e/mu2ebintools:README.md:undeclared-purpose` | 99583ace |
