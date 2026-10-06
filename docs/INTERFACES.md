---
title: mu2ebintools interfaces
group: artdaq-data-path
tier: A
audience: expert
summary: Two read-only FHiCL inspection command-line tools, fhicl-getpar and fhicl-summary, built against fhiclcpp and cetlib as a cetbuildtools/UPS product; no DAQ package calls them
---

# mu2ebintools — interfaces

The package exposes two executables and nothing else: no library, header,
configuration file, network or IPC interface. Full option text is in
`man/man1/fhicl-getpar.1` and `man/man1/fhicl-summary.1`.

## 1. fhicl-getpar

```
fhicl-getpar {--string|--int|--strlist|--keys} <fully.qualified.name> <file.fcl>
fhicl-getpar {-h|--help}
```

| Option | fhiclcpp call (`fhicl-getpar.cc`) | stdout |
|---|---|---|
| `--string` | `pset.get<std::string>(name)` (`:50`) | one line |
| `--int` | `pset.get<int>(name)` (`:53`) | one line |
| `--strlist` | `pset.get<std::vector<std::string>>(name)` (`:56`) | one element per line |
| `--keys` | `pset.get<fhicl::ParameterSet>(name).get_names()` (`:62-63`) | one key per line |

- Strict positions: `argc != 4` is an error (`:30`), and the option must be
  `argv[1]`. `-h`/`--help` is honoured only as `argv[1]` (`:21-26`).
- Parsing: `cet::filepath_lookup_after1("FHICL_FILE_PATH")` (`:39`). The
  top-level file is opened as given; `#include`s are searched along
  `FHICL_FILE_PATH`.

## 2. fhicl-summary

```
fhicl-summary [-v|-vv] <file.fcl>
fhicl-summary {-h|--help}
```

- `argc` must be 2 or 3 (`fhicl-summary.cc:359`). With 3, `argv[1]` must be
  `-v` or `-vv` (`:380-386`).
- Output is labelled lines, `name: value`, with continuation lines indented
  under the value column. Module lines are `label : module_type`, with labels
  right-aligned to the longest label seen (`:41-45`). Placeholders:
  `<not present>` for absent tables, lists or `module_type`, and
  `<present but empty>` (`:24-25`).
- The output is meant for people. It has no machine-readable mode, and its
  field labels differ between levels (`process name:  ` vs
  `process name:    `).

## 3. Exit status (both programs)

| Status | When |
|---|---|
| 0 | output printed; or `-h`/`--help` |
| 1 | usage error (message plus usage line on stderr); file not found or unparsable; parameter absent; type mismatch; any fhiclcpp exception during classification (`fhicl-summary.cc:402`) |

No other status is produced. In particular, a script cannot tell from the
status of `fhicl-getpar` whether a parameter is absent or the file failed to
parse (MBT-003).

## 4. Environment

| Variable | Use |
|---|---|
| `FHICL_FILE_PATH` | colon-separated directories for `#include` lookup (both programs). Behaviour when unset is that of `cet::search_path` in the linked cetlib; set it explicitly. |

## 5. Build interface

| Item | Value | Source |
|---|---|---|
| UPS product | `mu2ebintools v1_01_21` | `ups/product_deps:3` |
| Runtime dependency | `fhiclcpp v4_12_02` | `ups/product_deps:8` |
| Build-only dependency | `cetbuildtools v7_17_01` | `ups/product_deps:9` |
| Qualifiers | `e19:{debug,opt,prof}`, `e20:{debug,opt,prof}` | `ups/product_deps:14-19` |
| CMake floor | `2.8` | `CMakeLists.txt:16` |
| Targets | `cet_make_exec(fhicl-getpar …)`, `cet_make_exec(fhicl-summary …)`, linked to `${FHICLCPP} ${CETLIB} ${CETLIB_EXCEPT}` | `mu2ebintools/CMakeLists.txt:2-10` |
| Compiler flags | `cet_set_compiler_flags(DIAGS CAVALIER WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic)` | `CMakeLists.txt:33-37` |
| Package | `make package` → UPS tarball via `UseCPack` | `CMakeLists.txt:55` |

`WERROR` plus missing direct includes (`<array>`, `<algorithm>`, `<cstdlib>`)
make the build sensitive to standard-library header hygiene in newer
toolchains (MBT-002).
