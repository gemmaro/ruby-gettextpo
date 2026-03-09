# GettextPO

This is a Ruby library for the GNU gettext PO files.  This is a C
binding of the libgettextpo library, which is provided by the GNU
gettext package.  It is designed to promote Ruby's idiomatic coding
style while avoiding memory leaks.

## Installation

Prerequisites: `libgettextpo` library.  On Linux and similar systems,
you should be able to install it as a distribution package on most
platforms.  For example, on Ubuntu it was as follows
(2026-03-07T15:48:13+09:00):

``` shell
apt search libgettextpo
# Sorting... Done
# Full Text Search... Done
# libgettextpo-dev/noble 0.21-14ubuntu2 amd64 <----- This one!
#   process PO files - static libraries and headers
#
# libgettextpo0/noble 0.21-14ubuntu2 amd64
#   process PO files - shared library
```

Install the gem and add to the application's `Gemfile` by executing:

```bash
bundle add gettextpo
```

If Bundler is not being used to manage dependencies, install the gem by executing:

```bash
gem install gettextpo
```

## Usage

Say we have a PO file like:

```po
msgid "msgid1"
msgstr "msgstr1"
```

at `po_path`, then,

```ruby
require "gettextpo"

GettextPO::File.read(po_path).message_iterator.each do |message|
  pp({ source: message.msgid, translation: message.msgstr })
  #=> {source: "msgid1", translation: "msgstr1"}
end
```

Please refer to the [API documentation][api] and test cases for
details.

[api]: https://gemmaro.github.io/ruby-gettextpo/

## Development

`./bin/debug` to debug when segmentation fault.

After checking out the repo, run `bin/setup` to install
dependencies. Then, run `rake test` to run the tests. You can also run
`bin/console` for an interactive prompt that will allow you to
experiment.

To install this gem onto your local machine, run `bundle exec rake
install`.  To release a new version, update the version number in
`version.rb`, and then run `bundle exec rake release`, which will
create a git tag for the version, push git commits and the created
tag, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on [Disroot][disroot].

Other links:

* [RubyGems](https://rubygems.org/gems/gettextpo)
* [GitHub repository](https://github.com/gemmaro/ruby-gettextpo)

[disroot]: https://git.disroot.org/gemmaro/ruby-gettextpo

## License

Copyright (C) 2026  gemmaro

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
