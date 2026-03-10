# frozen_string_literal: true

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

require_relative "test_helper"

class GettextPOTest < Test::Unit::TestCase
  def load_tests(name)
    path = File.join(__dir__, "../test/#{name}.rb")
    instance_eval(File.read(path), path)
  end

  test 'common with mruby' do
    %w[
      check
      file
      filepos
      format
      header
      message
      message_iterator
      version
    ].each do |target|
      load_tests(target)
    end
  end

  def _create_message_and_drop_file
    file = GettextPO::File.new
    iterator = file.message_iterator
    iterator.insert("msgid1", "msgstr1")
  end

  test "dangling pointer" do
    message = _create_message_and_drop_file
    GC.start(full_mark: true, immediate_sweep: true)
    assert_equal "msgid1", message.msgid
  end
end
