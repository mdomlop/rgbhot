# Maintainer: Manuel Domínguez López <mdomlop at gmail dot com>

_pkgver_year=2020
_pkgver_month=04
_pkgver_day=03

pkgname=rgbtemp
pkgver=0.3
pkgrel=1
pkgdesc='Change RGB colors according to temperature.'
url='https://github.com/mdomlop/rgbtemp'
source=()
license=('GPL3')
arch=('any')
depends=('openrgb')
changelog=ChangeLog

build() {
    cd "$startdir"
    make
    }

package() {
    cd "$startdir"
    make arch_install DESTDIR="$pkgdir"
}
