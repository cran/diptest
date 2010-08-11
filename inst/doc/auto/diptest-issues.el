(TeX-add-style-hook "diptest-issues"
 (lambda ()
    (LaTeX-add-bibliographies
     "diptest")
    (LaTeX-add-labels
     "sec:Intro"
     "eq:Dn.F"
     "sec:dip_dip"
     "eq:P.Dn_min"
     "sec:Pvals"
     "sec:interpol"
     "sec:asymp")
    (TeX-run-style-hooks
     "natbib"
     "authoryear"
     "round"
     "longnamesfirst"
     "fullpage"
     "myVignette"
     "latex2e"
     "art10"
     "article")))

