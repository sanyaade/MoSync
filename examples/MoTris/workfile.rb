#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

work = PipeExeWork.new
work.instance_eval do
	@SOURCES = ["."]
	@EXTRA_CPPFLAGS = " -Wno-shadow"
	@LSTFILES = ["Res/res.lst"]
	@LIBRARIES = ["mautil"]
	@NAME = "MoTris"
end

work.invoke
