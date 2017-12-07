// Andrei Gaponenko, 2015
// Based on parsefhicl.cc by Rob Kutschke

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "fhiclcpp/intermediate_table.h"
#include "fhiclcpp/parse.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

#include "cetlib/filepath_maker.h"

std::string usage() {
  return "Usage: fhicl-getpar {--string|--int|--strlist|--keys} fully.qualified.par.name file.fcl\n";
}

int main(int argc, const char* argv[]){
  if( (argc > 1) &&
      ((argv[1] == std::string("-h")) ||
       (argv[1] == std::string("--help"))) ) {
    std::cout<<usage();
    exit(0);
  }

  try {
    // Check and parse arguments.
    if ( argc != 4 ){
      throw std::runtime_error("Error: wrong number of parameters.\n" + usage());
    }

    const std::string opt(argv[1]);
    const std::string parname(argv[2]);
    const std::string infile(argv[3]);

    // Read input file to make a parameter set.
    cet::filepath_lookup_after1 policy("FHICL_FILE_PATH");

    fhicl::intermediate_table tbl;
    fhicl::parse_document(infile, policy, tbl);

    fhicl::ParameterSet pset;
    fhicl::make_ParameterSet(tbl, pset);

    typedef std::vector<std::string> strlist;

    if(opt == "--string") {
      std::cout<<pset.get<std::string>(parname)<<std::endl;

    } else if(opt == "--int") {
      std::cout<<pset.get<int>(parname)<<std::endl;

    } else if(opt == "--strlist") {
      const strlist res(pset.get<strlist>(parname));
      for(const auto& a: res) {
        std::cout<<a<<std::endl;
      }

    } else if(opt == "--keys") {
      const fhicl::ParameterSet nps(pset.get<fhicl::ParameterSet>(parname));
      const strlist res(nps.get_names());
      for(const auto& a: res) {
        std::cout<<a<<std::endl;
      }

    } else {
      throw std::runtime_error("Error: unknown option "+opt+"\n"+usage());
    }

    // Check if parameter is present and get it's value;
    std::string val;
    //bool hasParam = pset.get_if_present<std::string>( param, val );

  }
  catch(std::exception& e) {
    std::cerr<<e.what()<<std::endl;
    exit(1);
  }

  exit(0);
}
