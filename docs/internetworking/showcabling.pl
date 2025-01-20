
sub samenet
{
    my ($x,$y) = @_;
    $x =~ s/\.\d+$//;
    $y =~ s/\.\d+$//;
    if ($x eq $y)
    {
	return 1;
    }
    else
    {
	return 0;
    }
}


%add={};
while(<STDIN>)
{
    if ($_ =~ /^\s+$/)
    {
	next;
    }
    @items = split /\s+/, $_;
    my $n;
    for($i=0; $i<$#items; $i++)
    {
	if ($items[$i] !~ /^eth/ && $items[$i] !~ /\d+\.\d+\.\d+\.\d+/)
	{
	    $n = $items[$i];
	    next;
	}
	else
	{
	    $add{$items[$i]}{'eth'} = $items[$i+1];
	    $add{$items[$i]}{'n'} = $n;
	    $i++;
	}
    }
}
%nets={};
for $x (sort {$a cmp $b} keys %add)
{
    for $y (sort {$a cmp $b} keys %add)
    {
	if (($x cmp $y) <= 0)
	{
	    next;
	}
	if (samenet($x, $y))
	{
	    #NWrouter eth4  <- is "wired" to ->  NWworkstation1 eth4
	    print "$add{$x}{'n'} $add{$x}{'eth'} <- is \"wired\" to -> $add{$y}{'n'} $add{$y}{'eth'}\n";
	}
    }
}
