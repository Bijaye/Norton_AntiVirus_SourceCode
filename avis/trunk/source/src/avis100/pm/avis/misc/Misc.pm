package Macro::Misc;

1;

sub ShortHex2bin
{
return sprintf "%c", hex($_[0]);
}

sub Hex2bin
{
(my $hexdump = $_[0]) =~ tr/\s\n//d;
my $bindump;

  while($hexdump) {
    $byte = substr($hexdump, 0, 2);
    substr($hexdump, 0, 2) = "";
    $bindump .= ShortHex2bin($byte);
  }
return $bindump;
}
