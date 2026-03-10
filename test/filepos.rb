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

assert 'basic' do
  assert_raise(NoMethodError) do
    GettextPO::FilePos.new
  end

  path = File.expand_path(File.join(__FILE__, "../../test.cruby/resources/filepos.po"))
  message = GettextPO::File.read(path).message_iterator.next
  assert_equal "spec1.txt", message.filepos(0).file
  assert_equal 1188, message.filepos(1).start_line
  message.remove_filepos(0)
  assert_equal "spec2.txt", message.filepos(0).file
  assert_nil message.filepos(1)
  message.add_filepos("roku-nana.txt", 67)
  assert_equal "roku-nana.txt", message.filepos(1).file
  true
end
