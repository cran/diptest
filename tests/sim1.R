library(diptest)

P.p <- c(1, 5, 10, 25)/100
(P.p <- c(P.p, 1/2, rev(1 - P.p)))

N.sim <- 9999
set.seed(94)
.p0 <- proc.time()
dU100 <- sapply(1:N.sim, function(i) dip(runif(100)))
cat('Time elapsed: ', proc.time() - .p0,'\n')
## Lynne (P IV, 1.6 GHz): ~ 7 s

100 * round(q100 <- quantile(dU100, p = P.p), 4)

round(1e4 * quantile(dU100, p = seq(0,1, by = 0.01), names = FALSE))
