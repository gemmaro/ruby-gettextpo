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
  file = GettextPO::File.new
  iterator = file.message_iterator
  message = iterator.insert("msgid1", "msgstr1\n")
  assert_raise(GettextPO::Error) do
    message.check_all(iterator)
  end
  assert_raise(GettextPO::Error) do
    file.check_all
  end

  path = File.expand_path(File.join(__FILE__, "../../test.cruby/resources/format.po"))
  file = GettextPO::File.read(path)
  iterator = file.message_iterator
  message = iterator.next
  xerrors = []
  xerrors2 = []
  assert_raise(GettextPO::Error) do
    message.check_format(xerror: Proc.new { |**kwargs| xerrors << kwargs },
                         xerror2: Proc.new { |**kwargs| xerrors2 << kwargs })
  end
  assert_equal "'msgstr' is not a valid C format string, unlike 'msgid'. Reason: The character that terminates the directive number 1 is not a valid conversion specifier.",
    xerrors[0][:message_text]
  assert_equal [], xerrors2
  true
end
