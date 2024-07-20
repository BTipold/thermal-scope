################################################################################
#
# thermal-scope
#
################################################################################

THERMAL_SCOPE_VERSION = 1.0
#THERMAL_SCOPE_SOURCE = thermal-scope-$(LIBFOO_VERSION).tar.gz
THERMAL_SCOPE_SITE = ./package/thermal-scope
THERMAL_SCOPE_SITE_METHOD = local
THERMAL_SCOPE_INSTALL_STAGING = YES
THERMAL_SCOPE_INSTALL_TARGET = YES
THERMAL_SCOPE_SUBDIR = ./src
THERMAL_SCOPE_DEPENDENCIES = \
    opencv3 \
    libusb \
    jsoncpp

$(eval $(cmake-package))
