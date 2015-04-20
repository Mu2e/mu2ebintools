all: fhicl-getpar
.PHONY: all

fhicl-getpar: fhicl-getpar.cc
	g++ $(FHICLCPP_CXXFLAGS) -I$(FHICLCPP_INC) -I$(CETLIB_INC) -I$(CPP0X_INC) -I$(BOOST_INC) -L$(FHICLCPP_LIB) -L$(CETLIB_LIB) -L$(CPP0X_LIB) -lfhiclcpp -lcetlib $< -o $@

.PHONY: clean
clean:
	$(RM) fhicl-getpar

#----------------
.DELETE_ON_ERROR:
#
