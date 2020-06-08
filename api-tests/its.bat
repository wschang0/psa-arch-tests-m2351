del .\platform\targets\tgt_dev_apis_tfm_m2351\Makefile
cp .\platform\targets\tgt_dev_apis_tfm_m2351\Makefile_its .\platform\targets\tgt_dev_apis_tfm_m2351\Makefile
sh ./tools/scripts/setup_unix.sh --target tgt_dev_apis_tfm_m2351 --toolchain ARMCLANG --cpu_arch armv8m_bl --verbose 2 --suite internal_trusted_storage --include ../../trusted-firmware-m/interface/include --archive_tests

