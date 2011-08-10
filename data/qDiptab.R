qDiptab <- (if(getRversion() < "2.13.0") .readRDS else readRDS
	    )(system.file("extraData", "qDiptab.rds",
			  package="diptest", mustWork=TRUE))
