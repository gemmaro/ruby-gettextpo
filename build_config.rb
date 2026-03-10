# Copyright (C) 2026  gemmaro
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

MRuby::Build.new do |conf|
  toolchain :gcc
  conf.gem File.expand_path(File.dirname(__FILE__))
  conf.linker.libraries << 'gettextpo'
  conf.enable_test
  conf.gem core: 'hal-posix-io'
  conf.gem core: 'mruby-bin-mruby'
  conf.gem core: 'mruby-bin-mirb'
  conf.cc do |cc|
    cc.flags << '-O0'
  end
end
