# 6Harmonics Inc.
# by Qige: qige@6harmonics.com, qige.zhao@6wilink.com
# 2016.10.27/2016.11.09
# 2017.11.20
# 2017.12.11 v3.1-6
# 2017.12.27 v3.3-* "git"

include $(TOPDIR)/rules.mk

APP_NAME:=arn-wb
PKG_NAME:=arn-proto-wb
PKG_VERSION:=3.3
PKG_RELEASE:=12

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Service for LCD Controller
  MAINTAINER:=Qige Zhao <qigezhao@gmail.com>
  DEPENDS:=+libgws2 +libiwinfo +libnl-tiny
endef

define Package/$(PKG_NAME)/description
  Service for LCD Controller, depends on libgws2, arn-scripts.
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/init.d

	$(INSTALL_CONF) \
		$(PKG_BUILD_DIR)/$(APP_NAME).conf \
		$(1)/etc/config/$(APP_NAME)
	$(INSTALL_BIN) \
		$(PKG_BUILD_DIR)/$(APP_NAME) \
		$(1)/usr/sbin/
	$(INSTALL_BIN) \
		$(PKG_BUILD_DIR)/$(APP_NAME).init \
		$(1)/etc/init.d/$(APP_NAME)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))


