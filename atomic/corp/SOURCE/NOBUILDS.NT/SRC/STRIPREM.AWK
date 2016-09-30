# note - this is run with the field separator set to ;
                                            { s = 0 }
# lines beginning ;!; - copy to the script - these are end user comments
 (s == 0) && ($1 ~ /^[\t ]*$/) && ($2 ~ /^!$/) { s = 4; print $0 }
# skip lines beginning with ;
 (s == 0) && ($1 ~ /^[\t ]*$/)              { s = 1 }
# print field 1 for lines without " before the first ; - no quotes, then safe to strip stuff after ;
 (s == 0) && ($1 ~ /^[^"]*$/)               { s = 2; print $1 }
# print field 1 for lines with -"-"- in field 1 - matched ", then up to ; is safe
 (s == 0) && ($1 ~ /^[^"]*"[^"]*"[^"]*$/)   { s = 3; print $1 }
# print whole line if not otherwise handled (lines with a " before the ; - this means a ; inside a string, print entire line
 (s == 0)                                   { print $0 }
