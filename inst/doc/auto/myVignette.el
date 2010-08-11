(TeX-add-style-hook "myVignette"
 (lambda ()
    (LaTeX-add-index-entries
     "\\RR~function #1"
     "class #1"
     "\\RR~package #1")
    (LaTeX-add-environments
     "example"
     "smallexample"
     "display"
     "smallverbatim")
    (TeX-add-symbols
     '("ERW" ["argument"] 1)
     '("PR" ["argument"] 1)
     '("makeright" 2)
     '("noFootnote" 1)
     '("myOp" 1)
     '("TODO" 1)
     '("FIXME" 1)
     '("strong" 1)
     '("acronym" 1)
     '("dfn" 1)
     '("file" 1)
     '("var" 1)
     '("key" 1)
     '("kbd" 1)
     '("pkg" 1)
     '("class" 1)
     '("Rfun" 1)
     '("email" 1)
     "Slang"
     "RR"
     "code"
     "samp"
     "isD"
     "iid"
     "env"
     "command"
     "option"
     "pkg")
    (TeX-run-style-hooks
     "alltt"
     "bm"
     "amsmath"
     "url"
     "hyperref")))

