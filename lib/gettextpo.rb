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

require_relative "gettextpo/version"
require_relative "gettextpo/gettextpo"

# The main entrypoints to parse PO files are GettextPO::File.new and
# GettextPO::File.read.
#
# == Error handling
#
# Some functions takes exception error handling paramters like
# +xerror+ and +xerror2+.  These parameters expect +Proc+ object which
# takes several parameters.  See also the description of
# GettextPO::File.read method.  The +severity+ parameter among these
# parameters is either SEVERITY_WARNING, SEVERITY_ERROR, or
# SEVERITY_FATAL_ERROR.
#
module GettextPO
  class Error < StandardError; end

  # This class doesn't provide the +new+ class method.  Refre to
  # GettextPO::MessageIterator#next or
  # GettextPO::MessageIterator#insert.
  class Message
    private_class_method :new
  end

  # This class doesn't provide the +new+ class method.  See also
  # GettextPO::File#message_iterator.
  class MessageIterator
    private_class_method :new

    def each # yields: message
      while true
        begin
          yield self.next
        rescue StopIteration
          return self
        end
      end
    end

    include Enumerable
  end

  # This class doesn't provide the +new+ class method.  See also
  # GettextPO::Message#filepos.
  class FilePos
    private_class_method :new
  end
end
