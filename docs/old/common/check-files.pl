$failed = 0;
for $file (@ARGV)
{
    print "Checking for $file\n";
    open(FE,"perl file_exists.pl $file |") || die "Failed: $!\n";
    while ( <FE> )
    {
	if ($_ =~ /does not/)
	{
	    print "$file does not exist\n";
	    $failed = 1;
	}
    }
}
if (!$failed)
{
    print "All tests passed!\n";
}
