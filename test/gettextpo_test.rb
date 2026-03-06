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
  test "VERSION" do
    assert do
      ::GettextPO.const_defined?(:VERSION)
    end
  end

  test "GettextPO::File.new" do
    GettextPO::File.new
  end

  test "GettextPO::File.read" do
    GettextPO::File.read(File.join(__dir__, "resources/ok.po"), xerror: nil, xerror2: nil)

    xerrors = []
    assert_raise(GettextPO::Error) do
      GettextPO::File.read(File.join(__dir__, "resources/a.po"), xerror: proc { |**kwargs| xerrors << kwargs }, xerror2: nil)
    end
    assert_equal "keyword \"a\" unknown", xerrors[0][:message_text]

    xerrors = []
    xerrors2 = []
    assert_raise(GettextPO::Error) do
      GettextPO::File.read(File.join(__dir__, "resources/bad.po"),
                             xerror: proc { |**kwargs| xerrors << kwargs },
                             xerror2: proc { |**kwargs| xerrors2 << kwargs })
    end
    assert_equal 1, xerrors.size
    assert_equal "duplicate message definition", xerrors[0][:message_text1]
    assert_equal [], xerrors2
  end

  test "GettextPO::File#write" do
    GettextPO::File.new.write(File.join(__dir__, "tmp/write.po"))
  end

  test "GettextPO::File#domains" do
    assert_equal ["messages"],
                 GettextPO::File.read(File.join(__dir__, "resources/ok.po")).domains
  end

  test 'message iterator' do
    assert_raise(NoMethodError) do
      GettextPO::MessageIterator.new
    end
  end

  test "GettextPO::File#message_iterator" do
    assert_kind_of GettextPO::MessageIterator,
                   GettextPO::File.new.message_iterator
  end

  test 'GettextPO::MessageIterator#next' do
    iterator = GettextPO::File.read(File.join(__dir__, "resources/ok.po")).message_iterator
    assert_not_nil iterator.next
    assert_not_nil iterator.next
    assert_raise(StopIteration) do
      iterator.next
    end
  end

  test 'GettextPO::MessageIterator#each' do
    iterator = GettextPO::File.read(File.join(__dir__, "resources/ok.po")).message_iterator
    messages = []
    iterator.each { |message| messages << message }
    assert_equal 2, messages.size
  end

  test 'GettextPO::MessageIterator#insert' do
    file = GettextPO::File.new
    iterator = file.message_iterator
    iterator.insert("msgid1", "msgstr1")
    out_path = File.join(__dir__, "tmp/insert.po")
    file.write(out_path)
    assert_equal [["msgid1", "msgstr1"]],
                 GettextPO::File.read(out_path).message_iterator
                   .map { |message| [message.msgid, message.msgstr] }
  end

  test 'GettextPO::Message getter/setter' do
    assert_raise(NoMethodError) do
      GettextPO::Message.new
    end

    iterator = GettextPO::File.read(File.join(__dir__, "resources/ok.po")).message_iterator
    message = iterator.next
    message.msgctxt = "msgctxt1"
    assert_equal "msgctxt1", message.msgctxt
    message.msgid = "msgid1"
    assert_equal "msgid1", message.msgid
    message.msgid_plural = "msgid_plural1"
    assert_equal "msgid_plural1", message.msgid_plural
    message.msgstr = "msgstr1"
    assert_equal "msgstr1", message.msgstr
    message.set_msgstr_plural(1, "msgstr_plural1")
    assert_equal "msgstr_plural1", message.msgstr_plural(1)
    message.comments = "comments1"
    assert_equal "comments1\n", message.comments
    message.extracted_comments = "extracted_comments1"
    assert_equal "extracted_comments1\n", message.extracted_comments
    message.prev_msgctxt = "prev_msgctxt1"
    assert_equal "prev_msgctxt1", message.prev_msgctxt
    message.prev_msgid = "prev_msgid1"
    assert_equal "prev_msgid1", message.prev_msgid
    message.prev_msgid_plural = "prev_msgid_plural1"
    assert_equal "prev_msgid_plural1", message.prev_msgid_plural
    message.obsolete = true
    assert_true message.obsolete?
    message.fuzzy = true
    assert_true message.fuzzy?
    message.update_format("c-format")
    assert_true message.format?("c-format")
    message.update_format("objc-format", opposite: true)
    assert_false message.format?("objc-format")
    message.update_format("csharp-format")
    message.update_format("csharp-format", remove: true)
    assert_false message.format?("csharp-format")
    assert_raise(GettextPO::Error) do
      message.update_format("c++-format", opposite: true, remove: true)
    end
    message.range = 1..2
    assert_true message.range?(1..2)
  end

  test "header field" do
    assert_nil GettextPO::File.new.domain_header

    header = <<~END_HEADER
      Project-Id-Version: Ruby GettextPO
      PO-Revision-Date: 2026-03-07 13:29+0900
      Last-Translator: gemmaro <gemmaro.dev@gmail.com>
      Language-Team: none
      MIME-Version: 1.0
      Content-Type: text/plain; charset=UTF-8
      Content-Transfer-Encoding: 8bit
    END_HEADER
    assert_equal header,
                 GettextPO::File.read(File.join(__dir__, "resources/ok.po")).domain_header
    assert_nil GettextPO::File.read(File.join(__dir__, "resources/ok.po")).domain_header("invalid domain")
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
                 GettextPO.header_with_updated_entry_value(header, "Project-Id-Version", "updated")
  end

  test "filepos" do
    assert_raise(NoMethodError) do
      GettextPO::FilePos.new
    end

    message = GettextPO::File.read(File.join(__dir__, "resources/filepos.po")).message_iterator.next
    assert_equal "spec1.txt", message.filepos(0).file
    assert_equal 1188, message.filepos(1).start_line
    message.remove_filepos(0)
    assert_equal "spec2.txt", message.filepos(0).file
    assert_nil message.filepos(1)
    message.add_filepos("roku-nana.txt", 67)
    assert_equal "roku-nana.txt", message.filepos(1).file
  end

  test 'formats' do
    assert_true GettextPO.formats.member?("c-format")
    assert_equal "C#", GettextPO.format_pretty_name("csharp-format")
  end

  test 'check' do
    file = GettextPO::File.new
    iterator = file.message_iterator
    message = iterator.insert("msgid1", "msgstr1\n")
    assert_raise(GettextPO::Error) do
      message.check_all(iterator)
    end
    assert_raise(GettextPO::Error) do
      file.check_all
    end

    file = GettextPO::File.read(File.join(__dir__, "resources/format.po"))
    iterator = file.message_iterator
    message = iterator.next
    xerrors = []
    xerrors2 = []
    assert_raise(GettextPO::Error) do
      message.check_format(xerror: proc { |**kwargs| xerrors << kwargs },
                           xerror2: proc { |**kwargs| xerrors2 << kwargs })
    end
    xerrors => [{ filename: String,
                  lineno: 3,
                  message: GettextPO::Message,
                  message_text: "'msgstr' is not a valid C format string, unlike 'msgid'. Reason: The character that terminates the directive number 1 is not a valid conversion specifier.",
                  multiline: false,
                  severity: 1 }]
    assert_equal [], xerrors2
  end
end
