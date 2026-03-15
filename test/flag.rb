assert 'basic' do
  assert_raise NoMethodError do
    GettextPO::FlagIterator.new
  end

  file = GettextPO::File.new
  message = file.message_iterator.insert('msgid1', 'msgstr1')
  message.update_workflow_flag('fuzzy')
  iter = message.workflow_flag_iterator
  assert_equal 'fuzzy', iter.next
  assert_nil iter.next
  true
end
