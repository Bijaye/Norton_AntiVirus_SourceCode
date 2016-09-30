# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

BEGIN { $| = 1; print "1..8\n"; }
END {print "not ok 1\n" unless $loaded;}
use Mcv;
$loaded = 1;
print "loading ok\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):

# Test IsWfw, IsXl and IsVBA5
if (
  (Mcv::IsWfw("t/Word7doc") && !Mcv::IsXl("t/Word7doc") && !Mcv::IsVBA5("t/Word7doc")) &&
  (!Mcv::IsWfw("t/Xl7book") && Mcv::IsXl("t/Xl7book") && !Mcv::IsVBA5("t/Xl7book")) &&
  (!Mcv::IsWfw("t/Word8mdoc") && !Mcv::IsXl("t/Word8mdoc") && Mcv::IsVBA5("t/Word8mdoc"))
  ) {
    print "IsWfw, IsXl and IsVBA5 ok\n";
}
else {
    print "IsWfw, IsXl and IsVBA5 not ok\n";
}

#Test GetWfwCs
if (Mcv::GetWfwCs("t/Word7doc") == 1) {
    print "GetWfwCs ok\n";
}
else {
    print "GetWfwCs not ok\n";
}

#Test GetWordDocVersion
if ((Mcv::GetWordDocVersion("t/Word7doc") == 7) && (Mcv::GetWordDocVersion("t/Word8doc") == 8)) {
    print "GetWordDocVersion ok\n";
}
else {
    print "GetWordDocVersion not ok\n";
}

#Test GetXlDocVersion
if (Mcv::GetXlDocVersion("t/Xl7book") == 7) {
    print "GetXlDocVersion ok\n";
}
else {
    print "GetXlDocVersion not ok\n";
}

#Test WfwGetTemplateByte
if (!Mcv::WfwGetTemplateByte("t/Word7doc") && Mcv::WfwGetTemplateByte("t/Word7tpl")) {
    print "WfwGetTemplateByte ok\n";
}
else {
    print "WfwGetTemplateByte not ok\n";
}

#Test IsWordDoc
if (Mcv::IsWordDoc("t/Word7doc") && Mcv::IsWordDoc("t/Word8doc")) {
    print "IsWordDoc ok\n";
}
else {
    print "IsWordDoc not ok\n";
}

#Test IsXlDoc
if (Mcv::IsXlDoc("t/Xl7book")) {
    print "IsXlDoc ok\n";
}
else {
    print "IsXlDoc not ok\n";
}

