# mu2ebintools

Two small command-line tools for inspecting Mu2e FHiCL configurations
without running `art`, built against fhiclcpp/cetlib as a UPS product.

| Tool | Does |
|---|---|
| `fhicl-getpar` | Print one fully expanded parameter: `--string`, `--int`, `--strlist` (one element per line) or `--keys` (member names of a table) |
| `fhicl-summary` | Summarise an art job: process name, source, outputs, services, producers/analyzers/filters, `trigger_paths`/`end_paths`, and every path classified as trigger, end or ambiguous (`-v`, `-vv` for more detail) |

```bash
export FHICL_FILE_PATH=.:/path/to/fcl/tree     # used for #include lookup
fhicl-getpar --string process_name job.fcl
fhicl-getpar --keys physics.producers job.fcl
fhicl-summary -v job.fcl
```

The file named on the command line is opened as given; files it includes
are searched along `FHICL_FILE_PATH`. Both tools exit 0 on success and 1 on
any error, with the message on standard error. Full options:
`man/man1/fhicl-getpar.1`, `man/man1/fhicl-summary.1`
(`man -l man/man1/fhicl-summary.1`).

## Status

- Version `v1_01_21` (`ups/product_deps`), last functional change 2020-11,
  built for fhiclcpp `v4_12_02` with `e19`/`e20` (gcc 8/9) qualifiers.
- The build uses cetbuildtools and UPS. There is no Spack recipe in this
  repository.
- No tests and no CI. Known defect: the default `fhicl-summary` output
  reports the services count wrongly when `physics.producers` is absent or
  services are missing (see `docs/ISSUES-PENDING.md`, MBT-001).
- It is a submodule of `mu2edaq-main`, but no DAQ package builds or calls it.

## Building

See [`docs/INSTALL.md`](docs/INSTALL.md). In short, with UPS and
cetbuildtools set up:

```bash
mkdir build && cd build
source ../mu2ebintools/ups/setup_for_development -p e20
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install \
      -DCMAKE_BUILD_TYPE=$CETPKG_TYPE "$CETPKG_SOURCE"
make && make install && make package
```

The original UPS build and CVMFS publishing notes are kept verbatim in
[`README`](README).

## Documentation

- [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md): what the tools do and how
- [`docs/INTERFACES.md`](docs/INTERFACES.md): command lines, output, exit
  status, build interface
- [`docs/ISSUES-PENDING.md`](docs/ISSUES-PENDING.md): review findings

## Authors

Andrei Gaponenko (`fhicl-getpar`, packaging) and Rob Kutschke
(`fhicl-summary`), with contributions from Ray Culbertson.

## License

No licence file is present (MBT-004).
