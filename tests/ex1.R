library(diptest)

## Real data examples :

data(statfaculty)

str(dip(statfaculty, full = TRUE, debug = TRUE), vec.len = 8)

data(faithful)
fE <- faithful$eruptions
str(dip(fE, full = TRUE, debug = TRUE), vec.len= 8)

data(precip)
str(dip(precip, full = TRUE, debug = TRUE))
