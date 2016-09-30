package Mcv;
require Exporter;
require DynaLoader;
@ISA = qw(Exporter DynaLoader);
@EXPORT_OK = qw(IsOLE2 IsWfw IsVBA5 IsXl GetWfwCs GetWordDocVersion GetXlDocVersion WfwGetTemplateByte WfwSetTemplateByte IsXlDoc IsWordDoc JP TW KR CH GetWfwDBCSCountryCode);
$VERSION = "0.1"; #(based on the code for ibmavsn 351)

=head1 DESCRIPTION
IsOLE2($file): returns 1 if $file is an OLE2 file, else 0
IsWfw($file): returns 1 if $file is a Word6/7 document, else 0
IsVBA5($file):
IsXl($file): returns 1 if $file is an Excel 4/7 document
GetWfwCs($file): returns 2 if $file is a Word 6/7 DBCS document, 1 if a SBCS doc, else: 0
GetWordDocVersion: returns 6 or 7 for Word 6/7 documents, 8 for Word 8 documents (based on the first word of the WordDocument stream); else: 0
GetXlDocVersion: returns 7 if a "Book" stream is found, 8 if a "Workbook" stream is found, 0 else.
WfwGetTemplateByte: returns (template byte)&0x01, or -1 if an error occurs.
WfwSetTemplateByte($file, $value): sets the template byte of the document to $value; returns 1 if success, 0 if failure.
IsXlDoc: returns 1 if a "Book" or "Workbook" stream is found
IsWordDoc: returns 1 if a "WordDocument" stream is found
=cut

bootstrap Mcv;
1;


sub IsOLE2
{
  my $ole2 = pack ("C8", 0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1);

  local ($fn) = @_;
  open (FILE, "$fn");
  binmode FILE;
  read (FILE, $buf, 8);
  close FILE;
  #print unpack("C8", $ole2), "\n";
  #print unpack("C8", $buf), "\n";
   if ($ole2 eq $buf) {
      return 1;
   }
   return 0;
}
