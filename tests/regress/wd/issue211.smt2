(set-logic QF_LRA)
(declare-fun _substvar_59_ () Real)
(declare-const v2 Bool)
(declare-const v4 Bool)
(declare-const r8 Real)
(declare-const r10 Real)
(declare-const r20 Real)
(declare-const v11 Bool)
(assert (or v11 (= 0.0 r8 0.912532 0.0 r10) v11))
(assert (or (= r20 r8 0.912532 _substvar_59_ r10) (= true (not v2) true v2 true true v4 v2 true v4)))
(check-sat)
(exit)
