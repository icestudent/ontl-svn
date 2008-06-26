; adds ntl::cxxrecord::catchguardhandler to the safeseh table
; compile with /safeseh

.386
.model flat, syscall
?catchguardhandler@cxxrecord@ntl@@SA?AW4disposition@exception@nt@2@PAUrecord@452@PAUregistration@452@PAUcontext@52@PAUdispatcher_context@452@@Z PROTO proc
.safeseh ?catchguardhandler@cxxrecord@ntl@@SA?AW4disposition@exception@nt@2@PAUrecord@452@PAUregistration@452@PAUcontext@52@PAUdispatcher_context@452@@Z
end
