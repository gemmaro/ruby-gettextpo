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

assert 'entry' do
  header = <<~END_HEADER
    Project-Id-Version: Ruby GettextPO
    PO-Revision-Date: 2026-03-07 13:29+0900
    Last-Translator: gemmaro <gemmaro.dev@gmail.com>
    Language-Team: none
    MIME-Version: 1.0
    Content-Type: text/plain; charset=UTF-8
    Content-Transfer-Encoding: 8bit
  END_HEADER
  assert_equal "Ruby GettextPO", GettextPO.header_entry_value(header, "Project-Id-Version")

  updated = <<~END_HEADER
    Project-Id-Version: updated
    PO-Revision-Date: 2026-03-07 13:29+0900
    Last-Translator: gemmaro <gemmaro.dev@gmail.com>
    Language-Team: none
    MIME-Version: 1.0
    Content-Type: text/plain; charset=UTF-8
    Content-Transfer-Encoding: 8bit
  END_HEADER
  assert_equal updated,
               GettextPO.header_with_updated_entry_value(header,
                                                         "Project-Id-Version", "updated")
  true
end
