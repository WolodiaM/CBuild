((simpc-mode . ((simpc-flymake-overrides
								 . (
										("cbuild.h" . (:flags ("-DCBUILD_IMPLEMENTATION")))
										("tests/*.c" . (:flags ("-include" "../framework.h" "-DTEST_RUN_PLATFORM=")))
										)
								 ))
						 ))
