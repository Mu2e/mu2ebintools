
cet_make_exec( fhicl-getpar
	       SOURCE fhicl-getpar.cc
	       LIBRARIES ${FHICLCPP} ${CETLIB} ${CETLIB_EXCEPT}
 )

cet_make_exec( fhicl-summary
	       SOURCE fhicl-summary.cc
	       LIBRARIES ${FHICLCPP} ${CETLIB} ${CETLIB_EXCEPT}
 )

install_headers()
install_source()
