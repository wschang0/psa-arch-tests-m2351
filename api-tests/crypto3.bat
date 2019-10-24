del .\platform\targets\tgt_dev_apis_tfm_m2351\Makefile
cp .\platform\targets\tgt_dev_apis_tfm_m2351\Makefile_crypto .\platform\targets\tgt_dev_apis_tfm_m2351\Makefile

del .\dev_apis\crypto\testsuite.db
cp .\dev_apis\crypto\testsuite3.db   .\dev_apis\crypto\testsuite.db

sh ./tools/scripts/setup_unix.sh --target tgt_dev_apis_tfm_m2351 --toolchain ARMCLANG --cpu_arch armv8m_bl --verbose 3 --suite crypto --include ../../trusted-firmware-m/interface/include --archive_tests

